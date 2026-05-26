#include "Renderer.h"

namespace PbrRtV2
{
void Renderer::SetSurfelDebugMode(SurfelDebugMode mode)
{
	m_SurfelDebugMode = mode;
}

SurfelDebugMode Renderer::GetSurfelDebugMode() const
{
	return m_SurfelDebugMode;
}

void Renderer::SetSurfelCount(std::size_t surfelCount)
{
	m_SurfelCount = surfelCount;
}

std::size_t Renderer::GetSurfelCount() const
{
	return m_SurfelCount;
}

void Renderer::SetVisibleSurfelCount(std::size_t visibleSurfelCount)
{
	m_VisibleSurfelCount = visibleSurfelCount;
}

std::size_t Renderer::GetVisibleSurfelCount() const
{
	return m_VisibleSurfelCount;
}

void Renderer::SetSurfelGIEnabled(bool enabled)
{
	m_SurfelGIEnabled = enabled;
}

bool Renderer::IsSurfelGIEnabled() const
{
	return m_SurfelGIEnabled;
}

void Renderer::SetGatheredSurfelIrradiance(const std::array<float, 3>& irradiance)
{
	m_GatheredSurfelIrradiance = irradiance;
}

const std::array<float, 3>& Renderer::GetGatheredSurfelIrradiance() const
{
	return m_GatheredSurfelIrradiance;
}

void Renderer::SetSurfelGIStrength(float strength)
{
	m_SurfelGIStrength = strength;
}

float Renderer::GetSurfelGIStrength() const
{
	return m_SurfelGIStrength;
}

void Renderer::SetSurfelGIAmplification(float amplification)
{
	m_SurfelGIAmplification = amplification;
}

float Renderer::GetSurfelGIAmplification() const
{
	return m_SurfelGIAmplification;
}
} // namespace PbrRtV2
