#pragma once

#include <array>
#include <string>
#include <vector>

#include "Surfel.h"

namespace PbrRtV2
{
enum class SurfelDebugMode
{
	Disabled,
	Points,
	Normals,
	Density,
	Probes  // Visualize probe positions and irradiance intensity
};

class SurfelDebugView
{
public:
	void SetEnabled(bool enabled);
	bool IsEnabled() const;

	void SetMode(SurfelDebugMode mode);
	SurfelDebugMode GetMode() const;

	void UpdateOverlay(const std::vector<Surfel>& surfels, float densityCellSize);

	// Update probe visualization overlay
	void UpdateProbeOverlay(const std::vector<std::array<float, 3>>& probePositions,
							const std::vector<std::array<float, 3>>& probeIrradiances);

	const std::vector<std::string>& GetOverlayLines() const;
	std::size_t GetVisibleSurfelCount() const;

private:
	std::array<int, 3> ComputeDensityGridCoord(const std::array<float, 3>& position, float cellSize) const;

private:
	bool m_Enabled = false;
	SurfelDebugMode m_Mode = SurfelDebugMode::Disabled;
	std::vector<std::string> m_OverlayLines;
	std::size_t m_VisibleSurfelCount = 0;
};
} // namespace PbrRtV2
