#include "AnitoApplication.h"

#include "AnitoPlatformWindow.h"

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

class SponzaSample final : public SampleBase
{
public:
	virtual DesiredApplicationSettings GetDesiredApplicationSettings(bool IsInitialization) override final
	{
		DesiredApplicationSettings Settings = SampleBase::GetDesiredApplicationSettings(IsInitialization);
		Settings.SetWindowWidth(PbrRtV2::AnitoPlatformWindow::DefaultWidth);
		Settings.SetWindowHeight(PbrRtV2::AnitoPlatformWindow::DefaultHeight);
		Settings.SetVSync(false);
		Settings.SetShowUI(false);
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
		GLTF_PBR_Renderer::WritePBRLightShaderAttribs({&m_DefaultLight, nullptr, &m_LightDirection, m_SceneScale}, Lights);

		HLSL::PBRRendererShaderParameters& RendererAttribs = FrameAttribs->Renderer;
		m_GLTFRenderer->SetInternalShaderParameters(RendererAttribs);
		RendererAttribs.OcclusionStrength = 1.0f;
		RendererAttribs.EmissionScale     = 1.0f;
		RendererAttribs.AverageLogLum     = 0.3f;
		RendererAttribs.MiddleGray        = 0.18f;
		RendererAttribs.WhitePoint        = 3.0f;
		RendererAttribs.IBLScale          = float4{1.0f};
		RendererAttribs.HighlightColor    = float4{0.0f, 0.0f, 0.0f, 0.0f};
		RendererAttribs.UnshadedColor     = float4{0.8f, 0.7f, 0.5f, 1.0f};
		RendererAttribs.PointSize         = 1.0f;
		RendererAttribs.MipBias           = 0.0f;
		RendererAttribs.LightCount        = 1;
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
	}

private:
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

		const float3 SceneDim  = ModelDim * m_SceneScale;
		const float3 HalfDim   = SceneDim * 0.5f;
		const float  FloorY    = -HalfDim.y;
		const float  EyeY      = FloorY + SceneDim.y * 0.18f;
		m_InitialCameraPos     = float3{-HalfDim.x * 0.10f, EyeY, -HalfDim.z * 0.55f};
		m_InitialCameraLookAt  = float3{HalfDim.x * 0.55f, EyeY - SceneDim.y * 0.02f, 0.0f};
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
	float3                             m_InitialCameraPos = float3{0.0f, 0.08f, -0.35f};
	float3                             m_InitialCameraLookAt = float3{0.16f, 0.06f, 0.02f};
	HLSL::CameraAttribs                m_CurrCameraAttribs = {};
	HLSL::CameraAttribs                m_PrevCameraAttribs = {};
	GLTF::Light                        m_DefaultLight = [](){ GLTF::Light L; L.Type = GLTF::Light::TYPE::DIRECTIONAL; L.Intensity = 3.0f; return L; }();
	float3                             m_LightDirection = normalize(float3{0.5f, 0.6f, -0.2f});
	float4x4                           m_ModelTransform = float4x4::Identity();
	float                              m_SceneScale = 1.0f;
	Uint32                             m_CurrentTransformIndex = 0;
	Uint32                             m_PreviousTransformIndex = 1;
	Uint64                             m_FrameNumber = 0;
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
