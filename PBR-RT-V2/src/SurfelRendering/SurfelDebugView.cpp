#include "SurfelDebugView.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <unordered_map>

namespace PbrRtV2
{
void SurfelDebugView::SetEnabled(bool enabled)
{
	m_Enabled = enabled;
	if (!m_Enabled)
	{
		m_OverlayLines.clear();
		m_VisibleSurfelCount = 0;
	}
}

bool SurfelDebugView::IsEnabled() const
{
	return m_Enabled;
}

void SurfelDebugView::SetMode(SurfelDebugMode mode)
{
	m_Mode = mode;
}

SurfelDebugMode SurfelDebugView::GetMode() const
{
	return m_Mode;
}

void SurfelDebugView::UpdateOverlay(const std::vector<Surfel>& surfels, float densityCellSize)
{
	m_OverlayLines.clear();
	m_VisibleSurfelCount = surfels.size();

	if (!m_Enabled || m_Mode == SurfelDebugMode::Disabled)
	{
		m_VisibleSurfelCount = 0;
		return;
	}

	std::ostringstream header;
	header << "Surfel Debug: " << static_cast<int>(m_Mode) << " | Surfels: " << surfels.size();
	m_OverlayLines.push_back(header.str());

	const std::size_t sampleCount = std::min<std::size_t>(surfels.size(), 6);

	switch (m_Mode)
	{
	case SurfelDebugMode::Points:
	{
		m_OverlayLines.push_back("Mode Points: sampled surfel positions and radii");
		for (std::size_t i = 0; i < sampleCount; ++i)
		{
			const auto& surfel = surfels[i];
			std::ostringstream line;
			line << std::fixed << std::setprecision(3)
				 << "[" << i << "] P(" << surfel.Position[0] << ", " << surfel.Position[1] << ", " << surfel.Position[2] << ")"
				 << " R=" << surfel.Radius;
			m_OverlayLines.push_back(line.str());
		}
		break;
	}
	case SurfelDebugMode::Normals:
	{
		m_OverlayLines.push_back("Mode Normals: sampled surfel normals and lengths");
		for (std::size_t i = 0; i < sampleCount; ++i)
		{
			const auto& surfel = surfels[i];
			const float normalLength = std::sqrt(
				surfel.Normal[0] * surfel.Normal[0] +
				surfel.Normal[1] * surfel.Normal[1] +
				surfel.Normal[2] * surfel.Normal[2]);
			std::ostringstream line;
			line << std::fixed << std::setprecision(3)
				 << "[" << i << "] N(" << surfel.Normal[0] << ", " << surfel.Normal[1] << ", " << surfel.Normal[2] << ")"
				 << " |N|=" << normalLength;
			m_OverlayLines.push_back(line.str());
		}
		break;
	}
	case SurfelDebugMode::Density:
	{
		m_OverlayLines.push_back("Mode Density: cell occupancy and radius distribution");

		float minRadius = std::numeric_limits<float>::max();
		float maxRadius = 0.0f;
		float radiusSum = 0.0f;

		std::unordered_map<std::int64_t, std::size_t> cellOccupancy;
		const float safeCellSize = std::max(densityCellSize, 0.0001f);

		for (const auto& surfel : surfels)
		{
			minRadius = std::min(minRadius, surfel.Radius);
			maxRadius = std::max(maxRadius, surfel.Radius);
			radiusSum += surfel.Radius;

			const auto coord = ComputeDensityGridCoord(surfel.Position, safeCellSize);
			const std::int64_t key =
				(static_cast<std::int64_t>(coord[0]) << 42) ^
				(static_cast<std::int64_t>(coord[1]) << 21) ^
				(static_cast<std::int64_t>(coord[2]));
			++cellOccupancy[key];
		}

		const float avgRadius = surfels.empty() ? 0.0f : (radiusSum / static_cast<float>(surfels.size()));
		std::size_t densestCellCount = 0;
		for (const auto& [_, count] : cellOccupancy)
		{
			densestCellCount = std::max(densestCellCount, count);
		}

		std::ostringstream radiusLine;
		radiusLine << std::fixed << std::setprecision(3)
			<< "Radius min=" << (surfels.empty() ? 0.0f : minRadius)
			<< " avg=" << avgRadius
			<< " max=" << maxRadius;
		m_OverlayLines.push_back(radiusLine.str());

		std::ostringstream densityLine;
		densityLine << "CellSize=" << std::fixed << std::setprecision(3) << safeCellSize
			<< " OccupiedCells=" << cellOccupancy.size()
			<< " MaxCellDensity=" << densestCellCount;
		m_OverlayLines.push_back(densityLine.str());
		break;
	}
	case SurfelDebugMode::Disabled:
	default:
		m_VisibleSurfelCount = 0;
		m_OverlayLines.clear();
		break;
	}
}

const std::vector<std::string>& SurfelDebugView::GetOverlayLines() const
{
	return m_OverlayLines;
}

void SurfelDebugView::UpdateProbeOverlay(const std::vector<std::array<float, 3>>& probePositions,
										 const std::vector<std::array<float, 3>>& probeIrradiances)
{
	m_OverlayLines.clear();
	m_VisibleSurfelCount = 0;

	if (!m_Enabled || m_Mode != SurfelDebugMode::Probes)
		return;

	if (probePositions.size() != probeIrradiances.size())
	{
		m_OverlayLines.push_back("ERROR: Probe position/irradiance count mismatch");
		return;
	}

	std::ostringstream header;
	header << "Probe Debug: " << probePositions.size() << " probes";
	m_OverlayLines.push_back(header.str());
	m_OverlayLines.push_back("Mode Probes: visualizing probe positions and irradiance intensity");

	// Compute statistics
	std::size_t activeProbeCount = 0;
	float minEnergy = std::numeric_limits<float>::max();
	float maxEnergy = 0.0f;
	float totalEnergy = 0.0f;

	for (std::size_t i = 0; i < probeIrradiances.size(); ++i)
	{
		const auto& irrad = probeIrradiances[i];
		const float energy = irrad[0] + irrad[1] + irrad[2];

		if (energy > 1e-5f)
		{
			++activeProbeCount;
			minEnergy = std::min(minEnergy, energy);
			maxEnergy = std::max(maxEnergy, energy);
			totalEnergy += energy;
		}
	}

	const float avgEnergy = activeProbeCount > 0 ? (totalEnergy / static_cast<float>(activeProbeCount)) : 0.0f;

	std::ostringstream statsLine;
	statsLine << std::fixed << std::setprecision(3)
		<< "Active: " << activeProbeCount << "/" << probePositions.size()
		<< " | Energy min=" << (activeProbeCount > 0 ? minEnergy : 0.0f)
		<< " avg=" << avgEnergy
		<< " max=" << maxEnergy;
	m_OverlayLines.push_back(statsLine.str());

	// Sample a few probes for detailed display
	const std::size_t sampleCount = std::min<std::size_t>(probePositions.size(), 6);
	for (std::size_t i = 0; i < sampleCount; ++i)
	{
		const auto& pos = probePositions[i];
		const auto& irrad = probeIrradiances[i];
		const float energy = irrad[0] + irrad[1] + irrad[2];

		std::ostringstream line;
		line << std::fixed << std::setprecision(3)
			<< "[" << i << "] P(" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")"
			<< " I(" << irrad[0] << ", " << irrad[1] << ", " << irrad[2] << ")"
			<< " E=" << energy;
		m_OverlayLines.push_back(line.str());
	}

	m_VisibleSurfelCount = activeProbeCount;
}

std::size_t SurfelDebugView::GetVisibleSurfelCount() const
{
	return m_VisibleSurfelCount;
}

std::array<int, 3> SurfelDebugView::ComputeDensityGridCoord(const std::array<float, 3>& position, float cellSize) const
{
	const float safeCellSize = std::max(cellSize, 0.0001f);
	return {
		static_cast<int>(std::floor(position[0] / safeCellSize)),
		static_cast<int>(std::floor(position[1] / safeCellSize)),
		static_cast<int>(std::floor(position[2] / safeCellSize))
	};
}
} // namespace PbrRtV2
