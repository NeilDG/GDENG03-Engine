#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Surfel.h"

namespace PbrRtV2
{
class SurfelSpatialGrid
{
public:
	void Build(const std::vector<Surfel>& surfels, float cellSize);
	std::vector<std::size_t> QueryNearby(const std::array<float, 3>& position, float radius) const;

	const std::vector<Surfel>& GetSurfels() const;
	float GetCellSize() const;

private:
	std::array<int, 3> ComputeCellCoord(const std::array<float, 3>& position) const;
	std::int64_t ComputeCellKey(const std::array<int, 3>& coord) const;

private:
	std::vector<Surfel> m_Surfels;
	std::unordered_map<std::int64_t, std::vector<std::size_t>> m_CellToSurfelIndices;
	float m_CellSize = 0.25f;
};
} // namespace PbrRtV2
