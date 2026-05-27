#include "SurfelProbeGrid.h"

namespace PbrRtV2
{
void SurfelProbeGrid::Initialize(const std::array<float, 3>& sceneMin,
								 const std::array<float, 3>& sceneMax,
								 int gridX,
								 int gridY,
								 int gridZ)
{
	m_SceneMin = sceneMin;
	m_SceneMax = sceneMax;
	m_GridDimensions = {gridX, gridY, gridZ};

	// Calculate total probe count
	const std::size_t totalProbes = static_cast<std::size_t>(gridX) *
									static_cast<std::size_t>(gridY) *
									static_cast<std::size_t>(gridZ);
	m_Probes.clear();
	m_Probes.reserve(totalProbes);

	// Compute probe spacing for uniform grid
	// Use (gridDim - 1) to place probes at bounds edges
	const float spacingX = (gridX > 1) ? (sceneMax[0] - sceneMin[0]) / static_cast<float>(gridX - 1) : 0.0f;
	const float spacingY = (gridY > 1) ? (sceneMax[1] - sceneMin[1]) / static_cast<float>(gridY - 1) : 0.0f;
	const float spacingZ = (gridZ > 1) ? (sceneMax[2] - sceneMin[2]) / static_cast<float>(gridZ - 1) : 0.0f;

	// Generate probe positions in 3D grid
	for (int z = 0; z < gridZ; ++z)
	{
		for (int y = 0; y < gridY; ++y)
		{
			for (int x = 0; x < gridX; ++x)
			{
				ProbeData probe;
				probe.Position[0] = sceneMin[0] + static_cast<float>(x) * spacingX;
				probe.Position[1] = sceneMin[1] + static_cast<float>(y) * spacingY;
				probe.Position[2] = sceneMin[2] + static_cast<float>(z) * spacingZ;
				probe.GatheredIrradiance = {0.0f, 0.0f, 0.0f}; // Initialize to zero
				m_Probes.push_back(probe);
			}
		}
	}
}

std::vector<ProbeData>& SurfelProbeGrid::GetProbes()
{
	return m_Probes;
}

const std::vector<ProbeData>& SurfelProbeGrid::GetProbes() const
{
	return m_Probes;
}

std::size_t SurfelProbeGrid::GetProbeCount() const
{
	return m_Probes.size();
}

std::array<int, 3> SurfelProbeGrid::GetGridDimensions() const
{
	return m_GridDimensions;
}

void SurfelProbeGrid::GetSceneBounds(std::array<float, 3>& outMin, std::array<float, 3>& outMax) const
{
	outMin = m_SceneMin;
	outMax = m_SceneMax;
}
} // namespace PbrRtV2
