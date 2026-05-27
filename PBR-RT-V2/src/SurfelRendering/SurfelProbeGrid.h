#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace PbrRtV2
{
/// @brief Represents a single GI probe in the scene
struct ProbeData
{
	std::array<float, 3> Position;           // World-space position
	std::array<float, 3> GatheredIrradiance; // RGB irradiance gathered from nearby surfels
};

/// @brief Manages a static 3D grid of GI probes covering the scene volume
class SurfelProbeGrid
{
public:
	/// @brief Initialize the probe grid with specified bounds and dimensions
	/// @param sceneMin Minimum corner of the scene AABB
	/// @param sceneMax Maximum corner of the scene AABB
	/// @param gridX Number of probes along X axis
	/// @param gridY Number of probes along Y axis
	/// @param gridZ Number of probes along Z axis
	void Initialize(const std::array<float, 3>& sceneMin,
					const std::array<float, 3>& sceneMax,
					int gridX,
					int gridY,
					int gridZ);

	/// @brief Get all probes in the grid (mutable for irradiance updates)
	std::vector<ProbeData>& GetProbes();

	/// @brief Get all probes in the grid (read-only)
	const std::vector<ProbeData>& GetProbes() const;

	/// @brief Get total number of probes in the grid
	std::size_t GetProbeCount() const;

	/// @brief Get grid dimensions
	std::array<int, 3> GetGridDimensions() const;

	/// @brief Get scene bounds used for probe placement
	void GetSceneBounds(std::array<float, 3>& outMin, std::array<float, 3>& outMax) const;

private:
	std::vector<ProbeData> m_Probes;
	std::array<int, 3> m_GridDimensions = {0, 0, 0};
	std::array<float, 3> m_SceneMin = {0.0f, 0.0f, 0.0f};
	std::array<float, 3> m_SceneMax = {0.0f, 0.0f, 0.0f};
};
} // namespace PbrRtV2
