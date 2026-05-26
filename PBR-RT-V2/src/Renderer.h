#pragma once

#include <array>
#include <cstddef>
#include <vector>

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

	// Multi-probe irradiance storage
	void SetGatheredSurfelIrradiances(const std::vector<std::array<float, 3>>& irradiances);
	const std::vector<std::array<float, 3>>& GetGatheredSurfelIrradiances() const;

	// Legacy single-probe support (deprecated - use multi-probe methods)
	void SetGatheredSurfelIrradiance(const std::array<float, 3>& irradiance);
	const std::array<float, 3>& GetGatheredSurfelIrradiance() const;

	void SetSurfelGIStrength(float strength);
	float GetSurfelGIStrength() const;

	void SetSurfelGIAmplification(float amplification);
	float GetSurfelGIAmplification() const;

	// Multi-probe status tracking
	void SetActiveProbeCount(std::size_t activeCount);
	std::size_t GetActiveProbeCount() const;

private:
	SurfelDebugMode m_SurfelDebugMode = SurfelDebugMode::Disabled;
	std::size_t m_SurfelCount = 0;
	std::size_t m_VisibleSurfelCount = 0;
	bool m_SurfelGIEnabled = false;
	std::vector<std::array<float, 3>> m_GatheredSurfelIrradiances;  // Multi-probe irradiance storage
	std::array<float, 3> m_GatheredSurfelIrradiance = {0.0f, 0.0f, 0.0f};  // Legacy single-probe (deprecated)
	float m_SurfelGIStrength = 0.75f;
	float m_SurfelGIAmplification = 1000.0f;  // High amplification to make GI dominant over reduced direct lights
	std::size_t m_ActiveProbeCount = 0;  // Number of probes contributing non-negligible irradiance
};
} // namespace PbrRtV2
