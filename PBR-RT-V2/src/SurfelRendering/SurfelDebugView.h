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
	Density
};

struct SurfelGIParameters
{
	bool Enabled = true;
	float Strength = 0.75f;
	float Amplification = 250.0f;
	float GatherRadius = 1.5f;
	float ProbeDistance = 0.4f;
	float ProbeVerticalOffset = 0.0f;
};

class SurfelDebugView
{
public:
	void SetEnabled(bool enabled);
	bool IsEnabled() const;

	void SetMode(SurfelDebugMode mode);
	SurfelDebugMode GetMode() const;

	void SetSurfelGIParameters(const SurfelGIParameters& parameters);
	const SurfelGIParameters& GetSurfelGIParameters() const;
	bool DrawSurfelGIControls();

	void UpdateOverlay(const std::vector<Surfel>& surfels, float densityCellSize);
	const std::vector<std::string>& GetOverlayLines() const;
	std::size_t GetVisibleSurfelCount() const;

private:
	std::array<int, 3> ComputeDensityGridCoord(const std::array<float, 3>& position, float cellSize) const;

private:
	bool m_Enabled = false;
	SurfelDebugMode m_Mode = SurfelDebugMode::Disabled;
	SurfelGIParameters m_SurfelGIParameters;
	std::vector<std::string> m_OverlayLines;
	std::size_t m_VisibleSurfelCount = 0;
};
} // namespace PbrRtV2
