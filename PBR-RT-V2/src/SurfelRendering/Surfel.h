#pragma once

#include <array>

namespace PbrRtV2
{
struct Surfel
{
	std::array<float, 3> Position = {0.0f, 0.0f, 0.0f};
	std::array<float, 3> Normal   = {0.0f, 1.0f, 0.0f};
	float Radius                 = 0.05f;
	std::array<float, 3> Albedo   = {1.0f, 1.0f, 1.0f};
	std::array<float, 3> Irradiance = {0.0f, 0.0f, 0.0f};
};

struct SurfelPrototypeSettings
{
	float PlacementDensity = 1.0f;
	float MinimumRadius    = 0.02f;
	float MaximumRadius    = 0.15f;
};

struct SurfelGISettings
{
	// Controls the number of surfels generated per unit area (default: 1.0)
	float SamplingQuality = 1.0f;

	// Cache resolution for spatial queries (in grid cells)
	int CacheResolution = 32;

	// Maximum number of surfels to cache per query
	int MaxCacheSize = 256;
};

struct SurfelLightSample
{
	// Position of the light sample in world space
	std::array<float, 3> Position = {0.0f, 0.0f, 0.0f};

	// Direct irradiance accumulated from all light sources
	std::array<float, 3> DirectIrradiance = {0.0f, 0.0f, 0.0f};

	// Distance-based attenuation factor (used for light falloff)
	float DistanceAttenuation = 1.0f;
};
} // namespace PbrRtV2
