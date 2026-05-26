#include "SurfelSpatialGrid.h"

#include <algorithm>
#include <cmath>

namespace PbrRtV2
{
void SurfelSpatialGrid::Build(const std::vector<Surfel>& surfels, float cellSize)
{
	m_Surfels = surfels;
	m_CellSize = std::max(cellSize, 0.0001f);
	m_CellToSurfelIndices.clear();

	for (std::size_t surfelIndex = 0; surfelIndex < m_Surfels.size(); ++surfelIndex)
	{
		const auto coord = ComputeCellCoord(m_Surfels[surfelIndex].Position);
		const std::int64_t key = ComputeCellKey(coord);
		m_CellToSurfelIndices[key].push_back(surfelIndex);
	}
}

std::vector<std::size_t> SurfelSpatialGrid::QueryNearby(const std::array<float, 3>& position, float radius) const
{
	std::vector<std::size_t> nearbyIndices;
	if (m_Surfels.empty())
	{
		return nearbyIndices;
	}

	const float safeRadius = std::max(radius, 0.0f);
	const float queryRadiusSquared = safeRadius * safeRadius;
	const int cellRadius = static_cast<int>(std::ceil(safeRadius / m_CellSize));
	const auto centerCoord = ComputeCellCoord(position);

	for (int z = centerCoord[2] - cellRadius; z <= centerCoord[2] + cellRadius; ++z)
	{
		for (int y = centerCoord[1] - cellRadius; y <= centerCoord[1] + cellRadius; ++y)
		{
			for (int x = centerCoord[0] - cellRadius; x <= centerCoord[0] + cellRadius; ++x)
			{
				const std::int64_t key = ComputeCellKey({x, y, z});
				auto cellIt = m_CellToSurfelIndices.find(key);
				if (cellIt == m_CellToSurfelIndices.end())
				{
					continue;
				}

				for (std::size_t surfelIndex : cellIt->second)
				{
					const auto& surfel = m_Surfels[surfelIndex];
					const float dx = surfel.Position[0] - position[0];
					const float dy = surfel.Position[1] - position[1];
					const float dz = surfel.Position[2] - position[2];
					const float distanceSquared = dx * dx + dy * dy + dz * dz;
					if (distanceSquared <= queryRadiusSquared)
					{
						nearbyIndices.push_back(surfelIndex);
					}
				}
			}
		}
	}

	return nearbyIndices;
}

const std::vector<Surfel>& SurfelSpatialGrid::GetSurfels() const
{
	return m_Surfels;
}

float SurfelSpatialGrid::GetCellSize() const
{
	return m_CellSize;
}

std::array<int, 3> SurfelSpatialGrid::ComputeCellCoord(const std::array<float, 3>& position) const
{
	return {
		static_cast<int>(std::floor(position[0] / m_CellSize)),
		static_cast<int>(std::floor(position[1] / m_CellSize)),
		static_cast<int>(std::floor(position[2] / m_CellSize))
	};
}

std::int64_t SurfelSpatialGrid::ComputeCellKey(const std::array<int, 3>& coord) const
{
	return
		(static_cast<std::int64_t>(coord[0]) << 42) ^
		(static_cast<std::int64_t>(coord[1]) << 21) ^
		(static_cast<std::int64_t>(coord[2]));
}
} // namespace PbrRtV2
