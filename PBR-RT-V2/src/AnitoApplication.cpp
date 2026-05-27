#include "AnitoApplication.h"

#include "AnitoPlatformWindow.h"
#include "InputSystem.h"
#include "Renderer.h"
#include "SurfelRendering/NightSceneLightingPreset.h"
#include "SurfelRendering/SurfelDebugView.h"
#include "SurfelRendering/SurfelGatherPass.h"
#include "SurfelRendering/SurfelGIRenderPass.h"
#include "SurfelRendering/SurfelProbeGrid.h"
#include "SurfelRendering/SurfelSceneBuilder.h"
#include "SurfelRendering/SurfelSpatialGrid.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <vector>

#include "SampleBase.hpp"
#include "FirstPersonCamera.hpp"
#include "MapHelper.hpp"
#include "CommonlyUsedStates.h"
#include "TextureUtilities.h"
#include "GLTFLoader.hpp"
#include "GLTF_PBR_Renderer.hpp"
#include "GraphicsUtilities.h"
#include "BasicMath.hpp"
#include "imgui.h"
#include "DebugUtilities.hpp"

namespace Diligent
{
namespace HLSL
{
#include "Shaders/Common/public/BasicStructures.fxh"
#include "Shaders/PBR/public/PBR_Structures.fxh"
#include "Shaders/PBR/private/RenderPBR_Structures.fxh"
} // namespace HLSL
}

namespace
{
using namespace Diligent;

const char* GetSurfelDebugModeLabel(PbrRtV2::SurfelDebugMode mode)
{
	switch (mode)
	{
	case PbrRtV2::SurfelDebugMode::Points:
		return "Points";
	case PbrRtV2::SurfelDebugMode::Normals:
		return "Normals";
	case PbrRtV2::SurfelDebugMode::Density:
		return "Density";
	case PbrRtV2::SurfelDebugMode::Probes:
		return "Probes";
	case PbrRtV2::SurfelDebugMode::Disabled:
	default:
		return "Disabled";
	}
}

class SponzaSample final : public SampleBase
{
public:
	virtual DesiredApplicationSettings GetDesiredApplicationSettings(bool IsInitialization) override final
	{
		DesiredApplicationSettings Settings = SampleBase::GetDesiredApplicationSettings(IsInitialization);
		Settings.SetWindowWidth(PbrRtV2::AnitoPlatformWindow::DefaultWidth);
		Settings.SetWindowHeight(PbrRtV2::AnitoPlatformWindow::DefaultHeight);
		Settings.SetVSync(false);
		Settings.SetShowUI(true);
		Settings.SetShowAdaptersDialog(false);
		return Settings;
	}

	virtual const Char* GetSampleName() const override final
	{
		return "PBR-RT-V2 Sponza Prototype";
	}

	virtual void Initialize(const SampleInitInfo& InitInfo) override final
	{
		SampleBase::Initialize(InitInfo);

		CreateRenderer();
		LoadEnvironmentMap("textures/papermill.ktx");
		CreateUniformBuffer(m_pDevice, m_GLTFRenderer->GetPRBFrameAttribsSize(), "PBR frame attribs buffer", &m_FrameAttribsCB);

		StateTransitionDesc Barriers[] = {
			{m_FrameAttribsCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE},
		};
		m_pImmediateContext->TransitionResourceStates(_countof(Barriers), Barriers);

		GLTF::ModelCreateInfo ModelCI;
		ModelCI.FileName             = "models/Sponza/glTF/Sponza.gltf";
		ModelCI.ComputeBoundingBoxes = true;
		m_Model                      = std::make_unique<GLTF::Model>(m_pDevice, m_pImmediateContext, ModelCI);
		m_ModelResourceBindings      = m_GLTFRenderer->CreateResourceBindings(*m_Model, m_FrameAttribsCB);
		m_RenderParams.SceneIndex    = static_cast<Uint32>(m_Model->DefaultSceneId);
		UpdateScene();
		InitializeSurfelPrototypeArchitecture();

		m_Camera.SetSpeedUpScales(1.5f, 2.5f);
		m_Camera.SetMoveSpeed(0.7f);
		m_Camera.SetRotationSpeed(0.002f);
		m_Camera.SetPos(m_InitialCameraPos);
		m_Camera.SetLookAt(m_InitialCameraLookAt);

		const auto& SCDesc = m_pSwapChain->GetDesc();
		m_Camera.SetProjAttribs(0.02f,
								400.0f,
								static_cast<float>(SCDesc.Width) / static_cast<float>(SCDesc.Height),
								PI_F / 4.0f,
								SCDesc.PreTransform,
								m_pDevice->GetDeviceInfo().IsGLDevice());
	}

	virtual void Render() override final
	{
		ITextureView* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
		ITextureView* pDSV = m_pSwapChain->GetDepthBufferDSV();

		const float ClearColor[] = {0.032f, 0.032f, 0.04f, 1.0f};
		m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		if (!m_Model)
			return;

		MapHelper<HLSL::PBRFrameAttribs> FrameAttribs{m_pImmediateContext, m_FrameAttribsCB, MAP_WRITE, MAP_FLAG_DISCARD};
		FrameAttribs->Camera     = m_CurrCameraAttribs;
		FrameAttribs->PrevCamera = m_PrevCameraAttribs;

		HLSL::PBRLightAttribs* Lights = reinterpret_cast<HLSL::PBRLightAttribs*>(FrameAttribs + 1);

		// Set up night-time lighting: reduce directional light and add point lights
		GLTF::Light NightDirectionalLight = m_DefaultLight;
		NightDirectionalLight.Intensity = 0.01f;  // Minimal directional light - surfel GI should dominate

		GLTF_PBR_Renderer::WritePBRLightShaderAttribs(
			{&NightDirectionalLight, nullptr, &m_LightDirection, m_SceneScale}, 
			Lights);

		// Apply point lights from night-time preset
		const auto& PointLights = m_NightSceneLightingPreset.GetPointLights();
		int totalLightCount = 1; // Start with directional light

		for (size_t i = 0; i < PointLights.size(); ++i)
		{
			const auto& PointLight = PointLights[i];
			const int LightIdx = totalLightCount;

			// Populate light structure manually
			Lights[LightIdx].Type = 2; // Point light (1=directional, 2=point, 3=spot)
			Lights[LightIdx].PosX = PointLight.Position[0];
			Lights[LightIdx].PosY = PointLight.Position[1];
			Lights[LightIdx].PosZ = PointLight.Position[2];
			Lights[LightIdx].DirectionX = 0.0f;
			Lights[LightIdx].DirectionY = 0.0f;
			Lights[LightIdx].DirectionZ = 0.0f;
			Lights[LightIdx].ShadowMapIndex = -1; // No shadows for now
			Lights[LightIdx].IntensityR = PointLight.Color[0] * PointLight.Intensity;
			Lights[LightIdx].IntensityG = PointLight.Color[1] * PointLight.Intensity;
			Lights[LightIdx].IntensityB = PointLight.Color[2] * PointLight.Intensity;

			// Range^4 for point light attenuation
			const float Range = PointLight.Radius;
			Lights[LightIdx].Range4 = Range * Range * Range * Range;

			Lights[LightIdx].SpotAngleScale = 0.0f;
			Lights[LightIdx].SpotAngleOffset = 0.0f;

			totalLightCount++;
		}

		const auto gatheredIrradiance = m_Renderer.GetGatheredSurfelIrradiance();
		const auto& giParameters = m_SurfelDebugView.GetSurfelGIParameters();
		const std::array<float, 3> scaledGatheredIrradiance = {
			gatheredIrradiance[0] * giParameters.TotalEnergyMultiplier,
			gatheredIrradiance[1] * giParameters.TotalEnergyMultiplier,
			gatheredIrradiance[2] * giParameters.TotalEnergyMultiplier};

		totalLightCount = m_SurfelGIRenderPass.AppendGatheredIrradianceLight(
			m_Renderer.IsSurfelGIEnabled(),
			scaledGatheredIrradiance,
			m_SurfelGIProbePosition,
			m_SurfelGIGatherRadius,
			m_Renderer.GetSurfelGIStrength(),
			m_Renderer.GetSurfelGIAmplification(),
			totalLightCount,
			Lights);

		HLSL::PBRRendererShaderParameters& RendererAttribs = FrameAttribs->Renderer;
		m_GLTFRenderer->SetInternalShaderParameters(RendererAttribs);
		RendererAttribs.OcclusionStrength = 1.0f;
		RendererAttribs.EmissionScale     = 1.0f;
		RendererAttribs.AverageLogLum     = 0.3f;
		RendererAttribs.MiddleGray        = 0.18f;
		RendererAttribs.WhitePoint        = 3.0f;
		RendererAttribs.IBLScale          = float4{0.0f};  // Disable environment map - surfel GI should dominate
		RendererAttribs.HighlightColor    = float4{0.0f, 0.0f, 0.0f, 0.0f};
		RendererAttribs.UnshadedColor     = float4{0.8f, 0.7f, 0.5f, 1.0f};
		RendererAttribs.PointSize         = 1.0f;
		RendererAttribs.MipBias           = 0.0f;
		RendererAttribs.LightCount        = static_cast<Uint32>(totalLightCount);
		RendererAttribs.DebugView         = static_cast<int>(m_RenderParams.DebugView);

		m_GLTFRenderer->Begin(m_pImmediateContext);
		m_GLTFRenderer->Render(m_pImmediateContext,
							   *m_Model,
							   m_Transforms[m_CurrentTransformIndex],
							   &m_Transforms[m_PreviousTransformIndex],
							   m_RenderParams,
							   &m_ModelResourceBindings);
	}

	virtual void Update(double CurrTime, double ElapsedTime, bool DoUpdateUI) override final
	{
		SampleBase::Update(CurrTime, ElapsedTime, DoUpdateUI);
		m_InputSystem.ProcessSurfelDebugInput(
			m_InputController.IsKeyDown(InputKeys::Reset),
			m_InputController.IsKeyDown(InputKeys::ShiftDown));
		m_Camera.Update(m_InputController, static_cast<float>(ElapsedTime));

		const auto& SCDesc = m_pSwapChain->GetDesc();
		const float AspectRatio = static_cast<float>(SCDesc.Width) / static_cast<float>(SCDesc.Height);
		m_Camera.SetProjAttribs(0.02f,
								400.0f,
								AspectRatio,
								PI_F / 4.0f,
								SCDesc.PreTransform,
								m_pDevice->GetDeviceInfo().IsGLDevice());

		const float4x4 CameraView     = m_Camera.GetViewMatrix() * GetSurfacePretransformMatrix(float3{0, 0, 1});
		const float4x4 CameraProj     = m_Camera.GetProjMatrix();
		const float4x4 CameraViewProj = CameraView * CameraProj;
		const float4x4 CameraWorld    = CameraView.Inverse();
		const float3 CameraWorldPos   = float3::MakeVector(CameraWorld[3]);

		m_PrevCameraAttribs = m_CurrCameraAttribs;
		m_CurrCameraAttribs.f4ViewportSize = float4{static_cast<float>(SCDesc.Width), static_cast<float>(SCDesc.Height), 1.0f / static_cast<float>(SCDesc.Width), 1.0f / static_cast<float>(SCDesc.Height)};
		m_CurrCameraAttribs.SetClipPlanes(0.02f, 400.0f);
		m_CurrCameraAttribs.fHandness    = CameraView.Determinant() > 0 ? 1.0f : -1.0f;
		m_CurrCameraAttribs.uiFrameIndex = static_cast<Uint32>(m_FrameNumber++);
		m_CurrCameraAttribs.mView        = CameraView;
		m_CurrCameraAttribs.mProj        = CameraProj;
		m_CurrCameraAttribs.mViewProj    = CameraViewProj;
		m_CurrCameraAttribs.mViewInv     = CameraView.Inverse();
		m_CurrCameraAttribs.mProjInv     = CameraProj.Inverse();
		m_CurrCameraAttribs.mViewProjInv = CameraViewProj.Inverse();
		m_CurrCameraAttribs.f4Position   = float4(CameraWorldPos, 1.0f);

		UpdateSurfelPrototypeArchitecture();
	}

protected:
	virtual void UpdateUI() override final
	{
		// Surfel Debug Window
		if (m_SurfelDebugView.IsEnabled() && m_SurfelDebugView.GetMode() != PbrRtV2::SurfelDebugMode::Disabled)
		{
			ImGui::SetNextWindowBgAlpha(0.55f);
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing;
			if (ImGui::Begin("Surfel Debug", nullptr, windowFlags))
			{
				const float3 cameraPos = m_Camera.GetPos();
				const float3 cameraLookAt = cameraPos + m_Camera.GetWorldAhead();

				ImGui::Text("Mode: %s", GetSurfelDebugModeLabel(m_SurfelDebugView.GetMode()));
				ImGui::Text("Visible Surfels: %zu / %zu", m_Renderer.GetVisibleSurfelCount(), m_Renderer.GetSurfelCount());
				ImGui::Text("Camera Position: (%.3f, %.3f, %.3f)", cameraPos.x, cameraPos.y, cameraPos.z);
				ImGui::Text("Camera LookAt:   (%.3f, %.3f, %.3f)", cameraLookAt.x, cameraLookAt.y, cameraLookAt.z);
				ImGui::Separator();
				for (const auto& line : m_SurfelDebugView.GetOverlayLines())
				{
					ImGui::TextUnformatted(line.c_str());
				}
				ImGui::Separator();
				ImGui::TextUnformatted("Controls: Home = Toggle | Shift+Home = Cycle Mode");
			}
			ImGui::End();
		}

		// Surfel GI Validation and Controls Window
		ImGui::SetNextWindowBgAlpha(0.75f);
		ImGuiWindowFlags giWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::Begin("Surfel GI Status", nullptr, giWindowFlags))
		{
			const auto& irradiance = m_Renderer.GetGatheredSurfelIrradiance();
			const float giEnergy = irradiance[0] + irradiance[1] + irradiance[2];
			const auto& giParameters = m_SurfelDebugView.GetSurfelGIParameters();
			const float scaledGiEnergy = giEnergy * giParameters.TotalEnergyMultiplier;
			const float strength = m_Renderer.GetSurfelGIStrength();
			const float amplification = m_Renderer.GetSurfelGIAmplification();
			const float amplifiedEnergy = scaledGiEnergy * strength * amplification;

			ImGui::Text("GI System: %s", m_Renderer.IsSurfelGIEnabled() ? "ACTIVE" : "INACTIVE");
			ImGui::Separator();

			ImGui::Text("Surfel GI Controls:");
			if (m_SurfelDebugView.DrawSurfelGIControls())
			{
				const auto& parameters = m_SurfelDebugView.GetSurfelGIParameters();
				m_SurfelGIGatherRadius = parameters.GatherRadius;
				m_SurfelGIProbeDistance = parameters.ProbeDistance;
				m_SurfelGIProbeVerticalOffset = parameters.ProbeVerticalOffset;
				m_Renderer.SetSurfelGIStrength(parameters.Strength);
				m_Renderer.SetSurfelGIAmplification(parameters.Amplification);
			}

			ImGui::Separator();
			ImGui::Text("Gathered Irradiance:");
			ImGui::Text("  R: %.6f", irradiance[0]);
			ImGui::Text("  G: %.6f", irradiance[1]);
			ImGui::Text("  B: %.6f", irradiance[2]);
			ImGui::Text("  Total Energy: %.6f", giEnergy);
			ImGui::Text("  Scaled Total Energy: %.6f", scaledGiEnergy);

			ImGui::Separator();
			ImGui::Text("Configuration:");
			ImGui::Text("  GI Strength: %.2f", strength);
			ImGui::Text("  GI Amplification: %.1f", amplification);
			ImGui::Text("  Amplified Energy: %.6f", amplifiedEnergy);
			ImGui::Text("  Gather Radius: %.2f", m_SurfelGIGatherRadius);
			ImGui::Text("  Probe Position: (%.2f, %.2f, %.2f)",
				m_SurfelGIProbePosition[0],
				m_SurfelGIProbePosition[1],
				m_SurfelGIProbePosition[2]);

			ImGui::Separator();
			ImGui::Text("Light Count: %d point lights", static_cast<int>(m_NightSceneLightingPreset.GetPointLights().size()));
			ImGui::Text("Total Surfels: %zu", m_Renderer.GetSurfelCount());

			ImGui::Separator();
			if (amplifiedEnergy > 1e-5f)
			{
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "STATUS: GI Light Active");
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "STATUS: No GI Contribution (Low Energy)");
			}
		}
		ImGui::End();
	}

private:
	void InitializeSurfelPrototypeArchitecture()
	{
		m_SurfelSceneBuilder.SetSettings(PbrRtV2::SurfelPrototypeSettings{});
		m_SurfelSceneBuilder.SetModel(m_Model.get());
		m_Surfels = m_SurfelSceneBuilder.BuildStaticSponzaSurfels();
		m_Renderer.SetSurfelCount(m_Surfels.size());
		m_Renderer.SetSurfelDebugMode(m_SurfelDebugView.GetMode());

		m_NightSceneLightingPreset.GenerateNightTimeLightRig(
			{0.0f, -0.1f, -0.1f},  // Camera position
			8,                     // lightCount
			0.001f,                // lightIntensity - reduced to make surfel GI dominant
			1.5f                   // lightRadius - kept large for surfel coverage
		);

		m_SurfelGatherPass.AccumulateDirectLighting(m_Surfels, m_NightSceneLightingPreset.GetPointLights());
		m_SurfelSpatialGrid.Build(m_Surfels, 0.25f);

		PbrRtV2::SurfelGIParameters giParameters;
		giParameters.Enabled = !m_Surfels.empty() && !m_NightSceneLightingPreset.GetPointLights().empty();
		giParameters.Strength = m_Renderer.GetSurfelGIStrength();
		giParameters.Amplification = m_Renderer.GetSurfelGIAmplification();
		giParameters.GatherRadius = m_SurfelGIGatherRadius;
		giParameters.ProbeDistance = m_SurfelGIProbeDistance;
		giParameters.ProbeVerticalOffset = m_SurfelGIProbeVerticalOffset;
		m_SurfelDebugView.SetSurfelGIParameters(giParameters);

		m_Renderer.SetSurfelGIEnabled(giParameters.Enabled);
		m_Renderer.SetGatheredSurfelIrradiance({0.0f, 0.0f, 0.0f});

		// Initialize multi-probe GI grid with runtime-configurable dimensions
		std::array<float, 3> sceneMin, sceneMax;
		ComputeSceneBounds(sceneMin, sceneMax);
		m_SurfelProbeGrid.Initialize(sceneMin, sceneMax, 4, 4, 4);

		// Compute adaptive gather radius based on probe spacing
		// Use 1.5× the maximum probe spacing to ensure adjacent probe coverage
		const float sceneExtentX = sceneMax[0] - sceneMin[0];
		const float sceneExtentY = sceneMax[1] - sceneMin[1];
		const float sceneExtentZ = sceneMax[2] - sceneMin[2];
		const float spacingX = sceneExtentX / 3.0f;
		const float spacingY = sceneExtentY / 3.0f;
		const float spacingZ = sceneExtentZ / 3.0f;
		const float maxSpacing = std::max({spacingX, spacingY, spacingZ});
		const float adaptiveRadius = maxSpacing * 1.5f; // 1.5× for overlapping probe coverage

		// Cap gather radius to prevent performance collapse with sparse grids
		// 6.0 units balances coverage (50% of max spacing) with performance (~120k cells/query)
		constexpr float MAX_GATHER_RADIUS = 6.0f; // Tuned for 0.25-unit spatial grid cells
		m_SurfelGIGatherRadius = std::min(adaptiveRadius, MAX_GATHER_RADIUS);

		LOG_INFO_MESSAGE("Initialized ", m_SurfelProbeGrid.GetProbeCount(), " GI probes (4x4x4) covering [",
			sceneMin[0], ",", sceneMin[1], ",", sceneMin[2], "] to [",
			sceneMax[0], ",", sceneMax[1], ",", sceneMax[2], "], gather radius: ", m_SurfelGIGatherRadius,
			" (adaptive: ", adaptiveRadius, ", capped at ", MAX_GATHER_RADIUS, ")");

		// Validation: Log first few surfels to verify direct lighting accumulation
		if (!m_Surfels.empty())
		{
			const std::size_t logCount = std::min<std::size_t>(5, m_Surfels.size());
			float totalR = 0.0f, totalG = 0.0f, totalB = 0.0f;
			for (std::size_t i = 0; i < logCount; ++i)
			{
				const auto& s = m_Surfels[i];
				totalR += s.Irradiance[0];
				totalG += s.Irradiance[1];
				totalB += s.Irradiance[2];
			}
			const float avgR = totalR / static_cast<float>(logCount);
			const float avgG = totalG / static_cast<float>(logCount);
			const float avgB = totalB / static_cast<float>(logCount);
			const float avgEnergy = avgR + avgG + avgB;

			// Log validation results to Visual Studio output
			const float amplification = m_Renderer.GetSurfelGIAmplification();
			const float strength = m_Renderer.GetSurfelGIStrength();
			const float amplifiedEnergy = avgEnergy * strength * amplification;

			LOG_INFO_MESSAGE("Surfel GI Validation:");
			LOG_INFO_MESSAGE("  Total Surfels: ", m_Surfels.size());
			LOG_INFO_MESSAGE("  Point Lights: ", m_NightSceneLightingPreset.GetPointLights().size());
			LOG_INFO_MESSAGE("  Point Light Intensity: 0.01 (low for Diligent sensitivity)");
			LOG_INFO_MESSAGE("  First ", logCount, " surfels avg irradiance: R=", avgR, " G=", avgG, " B=", avgB);
			LOG_INFO_MESSAGE("  Avg Irradiance Energy: ", avgEnergy);
			LOG_INFO_MESSAGE("  GI Amplification: ", amplification);
			LOG_INFO_MESSAGE("  GI Strength: ", strength);
			LOG_INFO_MESSAGE("  Expected Amplified Energy: ", amplifiedEnergy, " (should be > 0.01 for visibility)");

			if (avgEnergy < 1e-6f)
			{
				LOG_WARNING_MESSAGE("WARNING: Surfel irradiance is extremely low. Check light placement and radius.");
			}
			else if (amplifiedEnergy < 0.001f)
			{
				LOG_WARNING_MESSAGE("WARNING: Amplified GI energy is low. Consider increasing amplification factor.");
			}
			else
			{
				LOG_INFO_MESSAGE("SUCCESS: Surfel GI should be visible with current settings.");
			}
		}
	}

	void UpdateSurfelPrototypeArchitecture()
	{
		m_SurfelDebugView.SetEnabled(m_InputSystem.IsSurfelDebugEnabled());
		m_SurfelDebugView.SetMode(m_InputSystem.GetSurfelDebugMode());

		// Update debug overlay based on current mode
		if (m_SurfelDebugView.GetMode() == PbrRtV2::SurfelDebugMode::Probes)
		{
			// For probe debug mode, extract probe positions and irradiances
			const auto& probes = m_SurfelProbeGrid.GetProbes();
			std::vector<std::array<float, 3>> probePositions;
			std::vector<std::array<float, 3>> probeIrradiances;
			probePositions.reserve(probes.size());
			probeIrradiances.reserve(probes.size());

			for (const auto& probe : probes)
			{
				probePositions.push_back(probe.Position);
				probeIrradiances.push_back(probe.GatheredIrradiance);
			}

			m_SurfelDebugView.UpdateProbeOverlay(probePositions, probeIrradiances);
		}
		else
		{
			// For surfel debug modes, use the existing overlay
			m_SurfelDebugView.UpdateOverlay(m_Surfels, m_SurfelSpatialGrid.GetCellSize());
		}

		m_Renderer.SetSurfelDebugMode(m_SurfelDebugView.GetMode());
		m_Renderer.SetVisibleSurfelCount(m_SurfelDebugView.GetVisibleSurfelCount());

		const auto& giParameters = m_SurfelDebugView.GetSurfelGIParameters();
		m_Renderer.SetSurfelGIEnabled(giParameters.Enabled && !m_Surfels.empty());
		m_Renderer.SetSurfelGIStrength(giParameters.Strength);
		m_Renderer.SetSurfelGIAmplification(giParameters.Amplification);
		m_SurfelGIGatherRadius = giParameters.GatherRadius;
		m_SurfelGIProbeDistance = giParameters.ProbeDistance;
		m_SurfelGIProbeVerticalOffset = giParameters.ProbeVerticalOffset;

		const float3 cameraPos = m_Camera.GetPos();
		const float3 cameraForward = normalize(m_Camera.GetWorldAhead());
		const float3 probePos = cameraPos + cameraForward * m_SurfelGIProbeDistance + float3{0.0f, m_SurfelGIProbeVerticalOffset, 0.0f};
		m_SurfelGIProbePosition = {probePos.x, probePos.y, probePos.z};
		m_SurfelGIProbeNormal = {cameraForward.x, cameraForward.y, cameraForward.z};

		if (!m_Renderer.IsSurfelGIEnabled())
		{
			m_Renderer.SetGatheredSurfelIrradiance({0.0f, 0.0f, 0.0f});
			return;
		}

		const std::vector<std::size_t> nearbySurfels = m_SurfelSpatialGrid.QueryNearby(m_SurfelGIProbePosition, m_SurfelGIGatherRadius);
		const std::array<float, 3> gatheredIrradiance = m_SurfelGatherPass.GatherIrradiance(
			m_SurfelGIProbePosition,
			m_SurfelGIProbeNormal,
			m_Surfels,
			nearbySurfels,
			m_SurfelGIGatherRadius);
		m_Renderer.SetGatheredSurfelIrradiance(gatheredIrradiance);
	}

	void ComputeSceneBounds(std::array<float, 3>& outMin, std::array<float, 3>& outMax) const
	{
		// Hardcoded bounds for Sponza scene
		// These values cover the typical Sponza geometry extents
		// Future enhancement: compute dynamically from m_Model GLTF node/mesh bounds
		outMin = {-10.0f, -5.0f, -10.0f};
		outMax = {10.0f, 5.0f, 10.0f};

		// Expand bounds by 10% to ensure edge coverage
		const float expansion = 0.1f;
		for (int i = 0; i < 3; ++i)
		{
			const float range = outMax[i] - outMin[i];
			outMin[i] -= range * expansion;
			outMax[i] += range * expansion;
		}
	}

	void CreateRenderer()
	{
		GLTF_PBR_Renderer::CreateInfo RendererCI;
		RendererCI.EnableClearCoat              = true;
		RendererCI.EnableSheen                  = true;
		RendererCI.EnableIridescence           = true;
		RendererCI.EnableTransmission          = true;
		RendererCI.EnableAnisotropy            = true;
		RendererCI.FrontCounterClockwise       = true;
		RendererCI.PackMatrixRowMajor          = true;
		RendererCI.SheenAlbedoScalingLUTPath   = "textures/sheen_albedo_scaling.jpg";
		RendererCI.PreintegratedCharlieBRDFPath = "textures/charlie_preintegrated.jpg";
		RendererCI.NumRenderTargets            = 1;
		RendererCI.RTVFormats[0]               = m_pSwapChain->GetDesc().ColorBufferFormat;
		RendererCI.DSVFormat                   = m_pSwapChain->GetDesc().DepthBufferFormat;
		RendererCI.TexColorConversionMode      = m_pDevice->GetDeviceInfo().Features.TextureSubresourceViews ?
			GLTF_PBR_Renderer::CreateInfo::TEX_COLOR_CONVERSION_MODE_NONE :
			GLTF_PBR_Renderer::CreateInfo::TEX_COLOR_CONVERSION_MODE_SRGB_TO_LINEAR;

		m_RenderParams.Flags =
			GLTF_PBR_Renderer::PSO_FLAG_DEFAULT |
			GLTF_PBR_Renderer::PSO_FLAG_ENABLE_CLEAR_COAT |
			GLTF_PBR_Renderer::PSO_FLAG_ALL_TEXTURES |
			GLTF_PBR_Renderer::PSO_FLAG_ENABLE_SHEEN |
			GLTF_PBR_Renderer::PSO_FLAG_ENABLE_ANISOTROPY |
			GLTF_PBR_Renderer::PSO_FLAG_ENABLE_IRIDESCENCE |
			GLTF_PBR_Renderer::PSO_FLAG_ENABLE_TRANSMISSION |
			GLTF_PBR_Renderer::PSO_FLAG_ENABLE_VOLUME |
			GLTF_PBR_Renderer::PSO_FLAG_ENABLE_TEXCOORD_TRANSFORM;

		if (RendererCI.RTVFormats[0] == TEX_FORMAT_RGBA8_UNORM || RendererCI.RTVFormats[0] == TEX_FORMAT_BGRA8_UNORM)
			m_RenderParams.Flags |= GLTF_PBR_Renderer::PSO_FLAG_CONVERT_OUTPUT_TO_SRGB;

		m_GLTFRenderer = std::make_unique<GLTF_PBR_Renderer>(m_pDevice, nullptr, m_pImmediateContext, RendererCI);
	}

	void LoadEnvironmentMap(const char* Path)
	{
		RefCntAutoPtr<ITexture> pEnvironmentMap;
		CreateTextureFromFile(Path, TextureLoadInfo{"Environment map"}, m_pDevice, &pEnvironmentMap);
		VERIFY_EXPR(pEnvironmentMap);

		StateTransitionDesc Barrier{pEnvironmentMap, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE};
		m_pImmediateContext->TransitionResourceStates(1, &Barrier);
		m_EnvironmentMapSRV = pEnvironmentMap->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
		m_GLTFRenderer->PrecomputeCubemaps(m_pImmediateContext, m_EnvironmentMapSRV);
	}

	void UpdateScene()
	{
		m_Model->ComputeTransforms(m_RenderParams.SceneIndex, m_Transforms[0]);
		const BoundBox ModelAABB = m_Model->ComputeBoundingBox(m_RenderParams.SceneIndex, m_Transforms[0]);

		float3 ModelDim = ModelAABB.Max - ModelAABB.Min;
		float MaxDim = std::max(ModelDim.x, std::max(ModelDim.y, ModelDim.z));
		m_SceneScale = (1.0f / std::max(MaxDim, 0.01f)) * 0.5f;

		const float3 Translate = -ModelAABB.Min - 0.5f * ModelDim;
		float4x4 InvYAxis = float4x4::Identity();
		InvYAxis._22 = -1;
		float4x4 RotateX180 = float4x4::Identity();
		RotateX180._22 = -1;
		RotateX180._33 = -1;
		m_ModelTransform = float4x4::Translation(Translate) * float4x4::Scale(m_SceneScale) * InvYAxis * RotateX180;

		m_Model->ComputeTransforms(m_RenderParams.SceneIndex, m_Transforms[0], m_ModelTransform);
		m_Transforms[1] = m_Transforms[0];

		m_InitialCameraPos = float3{ 0.165f, -0.057f, -0.004f };
		m_InitialCameraLookAt = float3{ 0.959f, -0.109f, 0.019f };
	}

private:
	std::unique_ptr<GLTF_PBR_Renderer> m_GLTFRenderer;
	std::unique_ptr<GLTF::Model>       m_Model;
	GLTF_PBR_Renderer::ModelResourceBindings m_ModelResourceBindings;
	GLTF_PBR_Renderer::RenderInfo      m_RenderParams;
	RefCntAutoPtr<IBuffer>             m_FrameAttribsCB;
	RefCntAutoPtr<ITextureView>        m_EnvironmentMapSRV;
	std::array<GLTF::ModelTransforms, 2> m_Transforms;
	Diligent::FirstPersonCamera        m_Camera;
	float3                             m_InitialCameraPos = {};
	float3                             m_InitialCameraLookAt = {};
	HLSL::CameraAttribs                m_CurrCameraAttribs = {};
	HLSL::CameraAttribs                m_PrevCameraAttribs = {};
	GLTF::Light                        m_DefaultLight = [](){ GLTF::Light L; L.Type = GLTF::Light::TYPE::DIRECTIONAL; L.Intensity = 3.0f; return L; }();
	float3                             m_LightDirection = normalize(float3{0.5f, 0.6f, -0.2f});
	float4x4                           m_ModelTransform = float4x4::Identity();
	float                              m_SceneScale = 1.0f;
	Uint32                             m_CurrentTransformIndex = 0;
	Uint32                             m_PreviousTransformIndex = 1;
	Uint64                             m_FrameNumber = 0;
	PbrRtV2::Renderer                  m_Renderer;
	PbrRtV2::InputSystem               m_InputSystem;
	PbrRtV2::SurfelSceneBuilder        m_SurfelSceneBuilder;
	PbrRtV2::SurfelSpatialGrid         m_SurfelSpatialGrid;
	PbrRtV2::SurfelProbeGrid           m_SurfelProbeGrid;
	PbrRtV2::SurfelGatherPass          m_SurfelGatherPass;
	PbrRtV2::SurfelGIRenderPass        m_SurfelGIRenderPass;
	PbrRtV2::SurfelDebugView           m_SurfelDebugView;
	PbrRtV2::NightSceneLightingPreset  m_NightSceneLightingPreset;
	std::vector<PbrRtV2::Surfel>       m_Surfels;
	std::array<float, 3>               m_SurfelGIProbePosition = {0.0f, 0.0f, 0.0f};
	std::array<float, 3>               m_SurfelGIProbeNormal = {0.0f, 1.0f, 0.0f};
	float                              m_SurfelGIGatherRadius = 1.5f;  // Increased for extended spatial GI influence
	float                              m_SurfelGIProbeDistance = 0.0f;
	float                              m_SurfelGIProbeVerticalOffset = 1.0f;
};

} // namespace

namespace PbrRtV2
{
Diligent::SampleBase* AnitoApplication::CreateSample()
	{
	return new SponzaSample();
}
} // namespace PbrRtV2

namespace Diligent
{
SampleBase* CreateSample()
{
	return PbrRtV2::AnitoApplication::CreateSample();
}
} // namespace Diligent
