#pragma once

#include <array>

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
