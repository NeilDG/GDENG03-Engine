#include "NightSceneLightingPreset.h"

#include <utility>
#include <random>
#include <cmath>

namespace PbrRtV2
{
void NightSceneLightingPreset::GenerateNightTimeLightRig(
	const std::array<float, 3>& cameraPosition,
	int lightCount,
	float lightIntensity,
	float lightRadius)
{
	m_PointLights.clear();
	m_PointLights.reserve(lightCount);

	// Create a random number generator with fixed seed for reproducibility
	std::mt19937 rng(42);
	std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);  // Avoid too-dark colors
	std::uniform_real_distribution<float> posDist(-1.0f, 1.0f);   // Random offset in all directions

	// Place lights randomly around the camera position
	// This ensures lights are always visible regardless of camera placement
	const float lightDistanceFromCamera = 0.5f;  // How far from camera to place lights

	for (int i = 0; i < lightCount; ++i)
	{
		PointLightRig light;

		// Generate random direction vector for light placement
		// Use spherical distribution for natural light placement
		float theta = posDist(rng) * 3.14159265f;        // Azimuth angle
		float phi = posDist(rng) * 3.14159265f * 0.1f;   // Elevation angle (less extreme up/down)

		// Convert spherical coordinates to Cartesian
		float sinPhi = std::sin(phi);
		float x = std::cos(theta) * sinPhi;
		float y = std::sin(phi);
		float z = std::sin(theta) * sinPhi;

		// Normalize the direction (should already be close to normalized, but just in case)
		float length = std::sqrt(x * x + y * y + z * z);
		if (length > 0.0001f)
		{
			x /= length;
			y /= length;
			z /= length;
		}

		// Add randomization to the distance as well
		float distance = lightDistanceFromCamera * (0.5f + posDist(rng) * 0.25f);

		// Position light relative to camera
		light.Position[0] = cameraPosition[0] + x * distance;
		light.Position[1] = cameraPosition[1] + y * distance;
		light.Position[2] = cameraPosition[2] + z * distance;

		// Generate random RGB color with good saturation
		light.Color[0] = colorDist(rng);
		light.Color[1] = colorDist(rng);
		light.Color[2] = colorDist(rng);

		// Normalize color to avoid overly bright individual channels
		float maxChannel = std::max({light.Color[0], light.Color[1], light.Color[2]});
		if (maxChannel > 0.0f)
		{
			light.Color[0] /= maxChannel;
			light.Color[1] /= maxChannel;
			light.Color[2] /= maxChannel;
		}

		light.Intensity = lightIntensity;
		light.Radius = lightRadius;

		m_PointLights.push_back(light);
	}

	// Set ambient to a very low level for night-time atmosphere
	m_AmbientLevel = 0.005f;
}

void NightSceneLightingPreset::SetAmbientLevel(float ambientLevel)
{
	m_AmbientLevel = ambientLevel;
}

float NightSceneLightingPreset::GetAmbientLevel() const
{
	return m_AmbientLevel;
}

void NightSceneLightingPreset::SetPointLights(std::vector<PointLightRig> pointLights)
{
	m_PointLights = std::move(pointLights);
}

const std::vector<PointLightRig>& NightSceneLightingPreset::GetPointLights() const
{
	return m_PointLights;
}
} // namespace PbrRtV2
