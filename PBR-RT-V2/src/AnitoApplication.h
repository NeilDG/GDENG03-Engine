#pragma once

#include <vector>

#include "InputSystem.h"
#include "Renderer.h"
#include "SurfelRendering/NightSceneLightingPreset.h"
#include "SurfelRendering/SurfelDebugView.h"
#include "SurfelRendering/SurfelGatherPass.h"
#include "SurfelRendering/SurfelGIRenderPass.h"
#include "SurfelRendering/SurfelProbeGrid.h"
#include "SurfelRendering/SurfelSceneBuilder.h"
#include "SurfelRendering/SurfelSpatialGrid.h"

namespace Diligent
{
class SampleBase;
}

namespace PbrRtV2
{
class AnitoApplication
{
public:
	static Diligent::SampleBase* CreateSample();

private:
	void InitializeSurfelPrototypeArchitecture();
	void UpdateSurfelPrototypeArchitecture();

private:
	Renderer m_Renderer;
	InputSystem m_InputSystem;
	SurfelSceneBuilder m_SurfelSceneBuilder;
	SurfelSpatialGrid m_SurfelSpatialGrid;
	SurfelProbeGrid m_SurfelProbeGrid;
	SurfelGatherPass m_SurfelGatherPass;
	SurfelGIRenderPass m_SurfelGIRenderPass;
	SurfelDebugView m_SurfelDebugView;
	NightSceneLightingPreset m_NightSceneLightingPreset;
	std::vector<Surfel> m_Surfels;
};
} // namespace PbrRtV2
