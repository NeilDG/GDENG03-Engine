#pragma once

#include <array>
#include <cstddef>

#include "SurfelRendering/SurfelDebugView.h"

namespace PbrRtV2
{
class Renderer
{
public:
	virtual ~Renderer() = default;

	void SetSurfelDebugMode(SurfelDebugMode mode);
	SurfelDebugMode GetSurfelDebugMode() const;

	void SetSurfelCount(std::size_t surfelCount);
	std::size_t GetSurfelCount() const;

	void SetVisibleSurfelCount(std::size_t visibleSurfelCount);
	std::size_t GetVisibleSurfelCount() const;

	void SetSurfelGIEnabled(bool enabled);
	bool IsSurfelGIEnabled() const;

	void SetGatheredSurfelIrradiance(const std::array<float, 3>& irradiance);
	const std::array<float, 3>& GetGatheredSurfelIrradiance() const;

	void SetSurfelGIStrength(float strength);
	float GetSurfelGIStrength() const;

	void SetSurfelGIAmplification(float amplification);
	float GetSurfelGIAmplification() const;

private:
	SurfelDebugMode m_SurfelDebugMode = SurfelDebugMode::Disabled;
	std::size_t m_SurfelCount = 0;
	std::size_t m_VisibleSurfelCount = 0;
	bool m_SurfelGIEnabled = false;
	std::array<float, 3> m_GatheredSurfelIrradiance = {0.0f, 0.0f, 0.0f};
	float m_SurfelGIStrength = 1.0f;
	float m_SurfelGIAmplification = 30.0f;  // Amplify gathered irradiance when converting to light intensity
};
} // namespace PbrRtV2
