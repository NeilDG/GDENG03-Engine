#pragma once

#include <array>
#include <vector>

namespace Diligent
{
namespace HLSL
{
struct PBRLightAttribs;
} // namespace HLSL
} // namespace Diligent

namespace PbrRtV2
{
class SurfelGIRenderPass
{
public:
	// Multi-probe GI light injection (new)
	int AppendGatheredIrradianceLights(bool isEnabled,
									  const std::vector<std::array<float, 3>>& probeIrradiances,
									  const std::vector<std::array<float, 3>>& probePositions,
									  float gatherRadius,
									  float giStrength,
									  float giAmplification,
									  int currentLightCount,
									  int maxLights,
									  Diligent::HLSL::PBRLightAttribs* lights) const;

	// Legacy single-probe support (deprecated - for backward compatibility)
	int AppendGatheredIrradianceLight(bool isEnabled,
									 const std::array<float, 3>& gatheredIrradiance,
									 const std::array<float, 3>& probePosition,
									 float gatherRadius,
									 float giStrength,
									 float giAmplification,
									 int currentLightCount,
									 Diligent::HLSL::PBRLightAttribs* lights) const;
};
} // namespace PbrRtV2
