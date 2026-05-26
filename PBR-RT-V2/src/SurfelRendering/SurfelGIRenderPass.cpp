#include "SurfelGIRenderPass.h"

#include <algorithm>

#include "BasicMath.hpp"

namespace Diligent
{
namespace HLSL
{
#include "Shaders/Common/public/BasicStructures.fxh"
#include "Shaders/PBR/public/PBR_Structures.fxh"
#include "Shaders/PBR/private/RenderPBR_Structures.fxh"
} // namespace HLSL
} // namespace Diligent

namespace PbrRtV2
{
int SurfelGIRenderPass::AppendGatheredIrradianceLight(bool isEnabled,
												 const std::array<float, 3>& gatheredIrradiance,
												 const std::array<float, 3>& probePosition,
												 float gatherRadius,
												 float giStrength,
												 float giAmplification,
												 int currentLightCount,
												 Diligent::HLSL::PBRLightAttribs* lights) const
{
	if (!isEnabled || lights == nullptr)
	{
		return currentLightCount;
	}

	// Apply strength and amplification to convert irradiance back to light intensity
	// Amplification compensates for attenuation+albedo reduction in surfel lighting
	const float giR = std::max(0.0f, gatheredIrradiance[0]) * giStrength * giAmplification;
	const float giG = std::max(0.0f, gatheredIrradiance[1]) * giStrength * giAmplification;
	const float giB = std::max(0.0f, gatheredIrradiance[2]) * giStrength * giAmplification;
	const float giEnergy = giR + giG + giB;
	if (giEnergy <= 1e-5f)
	{
		return currentLightCount;
	}

	const int lightIdx = currentLightCount;
	lights[lightIdx].Type = 2;
	lights[lightIdx].PosX = probePosition[0];
	lights[lightIdx].PosY = probePosition[1];
	lights[lightIdx].PosZ = probePosition[2];
	lights[lightIdx].DirectionX = 0.0f;
	lights[lightIdx].DirectionY = 0.0f;
	lights[lightIdx].DirectionZ = 0.0f;
	lights[lightIdx].ShadowMapIndex = -1;
	lights[lightIdx].IntensityR = giR;
	lights[lightIdx].IntensityG = giG;
	lights[lightIdx].IntensityB = giB;

	const float safeRadius = std::max(gatherRadius * 2.0f, 0.01f);
	lights[lightIdx].Range4 = safeRadius * safeRadius * safeRadius * safeRadius;
	lights[lightIdx].SpotAngleScale = 0.0f;
	lights[lightIdx].SpotAngleOffset = 0.0f;

	return currentLightCount + 1;
}
} // namespace PbrRtV2
