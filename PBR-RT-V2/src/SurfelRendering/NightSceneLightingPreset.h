#pragma once

#include <array>
#include <vector>

namespace PbrRtV2
{
struct PointLightRig
{
	std::array<float, 3> Position = {0.0f, 0.0f, 0.0f};
	std::array<float, 3> Color    = {1.0f, 1.0f, 1.0f};
	float Intensity              = 1.0f;
	float Radius                 = 1.0f;
};

class NightSceneLightingPreset
{
public:
	// Generate a default night-time lighting rig with colored point lights
	// Light positions are randomly distributed around the camera position
	// Colors are randomized uniformly
	void GenerateNightTimeLightRig(
		const std::array<float, 3>& cameraPosition,  // Camera position to place lights around
		int lightCount,
		float lightIntensity,
		float lightRadius);

	void SetAmbientLevel(float ambientLevel);
	float GetAmbientLevel() const;

	void SetPointLights(std::vector<PointLightRig> pointLights);
	const std::vector<PointLightRig>& GetPointLights() const;

private:
	float m_AmbientLevel = 0.02f;
	std::vector<PointLightRig> m_PointLights;
};
} // namespace PbrRtV2
