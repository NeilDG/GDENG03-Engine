#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include "NightSceneLightingPreset.h"
#include "Surfel.h"

namespace PbrRtV2
{
class SurfelLightingResolver
{
public:
	std::array<float, 3> ResolveDirectIrradiance(const Surfel& surfel,
										 const std::vector<PointLightRig>& pointLights) const;
};

class SurfelGatherPass
{
public:
	void AccumulateDirectLighting(std::vector<Surfel>& surfels,
							const std::vector<PointLightRig>& pointLights) const;

	std::array<float, 3> GatherIrradiance(const std::array<float, 3>& shadingPosition,
								 const std::array<float, 3>& shadingNormal,
								 const std::vector<Surfel>& surfels,
								 const std::vector<std::size_t>& nearbyIndices,
								 float gatherRadius) const;

private:
	SurfelLightingResolver m_LightingResolver;
};
} // namespace PbrRtV2
