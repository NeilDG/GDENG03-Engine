#include "SurfelGatherPass.h"

#include <algorithm>
#include <cmath>

namespace PbrRtV2
{
namespace
{
float Dot3(const std::array<float, 3>& lhs, const std::array<float, 3>& rhs)
{
	return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

std::array<float, 3> Normalize3(const std::array<float, 3>& value)
{
	const float lengthSquared = Dot3(value, value);
	if (lengthSquared <= 1e-8f)
	{
		return {0.0f, 1.0f, 0.0f};
	}

	const float invLength = 1.0f / std::sqrt(lengthSquared);
	return {value[0] * invLength, value[1] * invLength, value[2] * invLength};
}
} // namespace

std::array<float, 3> SurfelLightingResolver::ResolveDirectIrradiance(const Surfel& surfel,
													 const std::vector<PointLightRig>& pointLights) const
{
	std::array<float, 3> irradiance = {0.0f, 0.0f, 0.0f};
	const std::array<float, 3> normal = Normalize3(surfel.Normal);

	for (const auto& light : pointLights)
	{
		const float toLightX = light.Position[0] - surfel.Position[0];
		const float toLightY = light.Position[1] - surfel.Position[1];
		const float toLightZ = light.Position[2] - surfel.Position[2];
		const float distanceSquared = toLightX * toLightX + toLightY * toLightY + toLightZ * toLightZ;
		if (distanceSquared <= 1e-8f)
		{
			continue;
		}

		const float distance = std::sqrt(distanceSquared);
		const float invDistance = 1.0f / distance;
		const std::array<float, 3> toLightDir = {toLightX * invDistance, toLightY * invDistance, toLightZ * invDistance};
		const float nDotL = std::max(0.0f, Dot3(normal, toLightDir));
		if (nDotL <= 0.0f)
		{
			continue;
		}

		const float lightRange = std::max(light.Radius, 0.001f);
		const float normalizedDistance = distance / lightRange;
		if (normalizedDistance >= 1.0f)
		{
			continue;
		}

		const float rangeFalloff = 1.0f - (normalizedDistance * normalizedDistance);
		const float attenuation = (rangeFalloff * rangeFalloff) / (1.0f + distanceSquared);
		const float strength = light.Intensity * attenuation * nDotL;

		irradiance[0] += light.Color[0] * strength * surfel.Albedo[0];
		irradiance[1] += light.Color[1] * strength * surfel.Albedo[1];
		irradiance[2] += light.Color[2] * strength * surfel.Albedo[2];
	}

	return irradiance;
}

void SurfelGatherPass::AccumulateDirectLighting(std::vector<Surfel>& surfels,
										const std::vector<PointLightRig>& pointLights) const
{
	for (auto& surfel : surfels)
	{
		surfel.Irradiance = m_LightingResolver.ResolveDirectIrradiance(surfel, pointLights);
	}
}

std::array<float, 3> SurfelGatherPass::GatherIrradiance(const std::array<float, 3>& shadingPosition,
												 const std::array<float, 3>& shadingNormal,
												 const std::vector<Surfel>& surfels,
												 const std::vector<std::size_t>& nearbyIndices,
												 float gatherRadius) const
{
	std::array<float, 3> gathered = {0.0f, 0.0f, 0.0f};
	if (nearbyIndices.empty() || surfels.empty())
	{
		return gathered;
	}

	const float safeRadius = std::max(gatherRadius, 0.001f);
	const float invRadius = 1.0f / safeRadius;
	const std::array<float, 3> surfaceNormal = Normalize3(shadingNormal);
	float totalWeight = 0.0f;

	for (std::size_t surfelIndex : nearbyIndices)
	{
		if (surfelIndex >= surfels.size())
		{
			continue;
		}

		const auto& surfel = surfels[surfelIndex];
		const float dx = surfel.Position[0] - shadingPosition[0];
		const float dy = surfel.Position[1] - shadingPosition[1];
		const float dz = surfel.Position[2] - shadingPosition[2];
		const float distanceSquared = dx * dx + dy * dy + dz * dz;
		const float distance = std::sqrt(distanceSquared);
		if (distance >= safeRadius)
		{
			continue;
		}

		const float distanceWeight = 1.0f - (distance * invRadius);
		const std::array<float, 3> surfelNormal = Normalize3(surfel.Normal);
		const float normalWeight = std::max(0.0f, Dot3(surfaceNormal, surfelNormal));
		const float weight = distanceWeight * normalWeight;
		if (weight <= 1e-6f)
		{
			continue;
		}

		gathered[0] += surfel.Irradiance[0] * weight;
		gathered[1] += surfel.Irradiance[1] * weight;
		gathered[2] += surfel.Irradiance[2] * weight;
		totalWeight += weight;
	}

	if (totalWeight > 1e-6f)
	{
		gathered[0] /= totalWeight;
		gathered[1] /= totalWeight;
		gathered[2] /= totalWeight;
	}

	return gathered;
}
} // namespace PbrRtV2
