#include "SurfelSceneBuilder.h"

#include <cmath>
#include <algorithm>
#include <random>

#include "GLTFLoader.hpp"

namespace PbrRtV2
{
// Helper function to calculate cross product
static std::array<float, 3> CrossProduct(
	const std::array<float, 3>& a,
	const std::array<float, 3>& b)
{
	return {
		a[1] * b[2] - a[2] * b[1],
		a[2] * b[0] - a[0] * b[2],
		a[0] * b[1] - a[1] * b[0]
	};
}

// Helper function to calculate dot product
static float DotProduct(const std::array<float, 3>& a, const std::array<float, 3>& b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// Helper function to calculate vector magnitude
static float Magnitude(const std::array<float, 3>& v)
{
	return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

// Helper function to normalize a vector
static std::array<float, 3> Normalize(const std::array<float, 3>& v)
{
	float mag = Magnitude(v);
	if (mag > 0.0001f)
	{
		return {v[0] / mag, v[1] / mag, v[2] / mag};
	}
	return {0.0f, 1.0f, 0.0f};
}

// Helper function to calculate vector subtraction
static std::array<float, 3> Subtract(const std::array<float, 3>& a, const std::array<float, 3>& b)
{
	return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

// Helper function to calculate vector addition
static std::array<float, 3> Add(const std::array<float, 3>& a, const std::array<float, 3>& b)
{
	return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

// Helper function to scale a vector
static std::array<float, 3> Scale(const std::array<float, 3>& v, float s)
{
	return {v[0] * s, v[1] * s, v[2] * s};
}

Surfel SurfelTriangleSampler::SampleFromTriangle(
	const std::array<float, 3>& v0,
	const std::array<float, 3>& v1,
	const std::array<float, 3>& v2,
	const std::array<float, 3>& n0,
	const std::array<float, 3>& n1,
	const std::array<float, 3>& n2,
	const std::array<float, 3>& albedo,
	float radius)
{
	Surfel surfel;

	// Calculate triangle center (barycentric center)
	const float oneThird = 1.0f / 3.0f;
	surfel.Position = Add(Add(Scale(v0, oneThird), Scale(v1, oneThird)), Scale(v2, oneThird));

	// Interpolate normal at center
	surfel.Normal = Add(Add(Scale(n0, oneThird), Scale(n1, oneThird)), Scale(n2, oneThird));
	surfel.Normal = Normalize(surfel.Normal);

	// Set surfel properties
	surfel.Albedo = albedo;
	surfel.Radius = radius;
	surfel.Irradiance = {0.0f, 0.0f, 0.0f};

	return surfel;
}

float SurfelTriangleSampler::CalculateTriangleArea(
	const std::array<float, 3>& v0,
	const std::array<float, 3>& v1,
	const std::array<float, 3>& v2)
{
	// Use cross product to calculate area
	// Area = 0.5 * ||(v1 - v0) x (v2 - v0)||
	const auto edge1 = Subtract(v1, v0);
	const auto edge2 = Subtract(v2, v0);
	const auto cross = CrossProduct(edge1, edge2);
	const float area = 0.5f * Magnitude(cross);

	return area;
}

int SurfelTriangleSampler::CalculateSurfelCount(float triangleArea, float placementDensity)
{
	// Calculate the number of surfels based on area and density
	// density is surfels per unit area
	const float surfelCount = triangleArea * placementDensity;
	return std::max(1, static_cast<int>(std::ceil(surfelCount)));
}

void SurfelSceneBuilder::SetSettings(const SurfelPrototypeSettings& settings)
{
	m_Settings = settings;
}

const SurfelPrototypeSettings& SurfelSceneBuilder::GetSettings() const
{
	return m_Settings;
}

void SurfelSceneBuilder::SetModel(Diligent::GLTF::Model* pModel)
{
	m_pModel = pModel;
}

std::vector<Surfel> SurfelSceneBuilder::BuildStaticSponzaSurfels() const
{
	std::vector<Surfel> surfels;

	if (!m_pModel)
	{
		return surfels;
	}

	// Generate surfel distribution across the scene
	// This is a procedural approach that estimates surfel placement based on scene bounds
	// Future implementation: directly sample from mesh geometry data

	// Create a random number generator for surfel placement
	std::mt19937 rng(42); // Fixed seed for deterministic results
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	// Estimate surfel count based on scene complexity and placement density
	// The Sponza scene is moderately complex, so we estimate around 50-100 surfels per mesh
	const int estimatedMeshCount = static_cast<int>(m_pModel->Meshes.size());
	const int surfelCountPerMesh = std::max(1, static_cast<int>(m_Settings.PlacementDensity * 50.0f));
	const int totalSurfelCount = estimatedMeshCount * surfelCountPerMesh;

	// Reserve space for efficiency
	surfels.reserve(totalSurfelCount);

	// Generate surfels distributed throughout the scene bounds
	// Using a grid-based distribution with some randomization
	for (int i = 0; i < totalSurfelCount; ++i)
	{
		Surfel surfel;

		// Procedurally generate surfel positions within a reasonable scene bounds
		// Sponza scene typically spans approximately -1 to 1 in X, -0.5 to 2.0 in Y, -1 to 1 in Z
		surfel.Position[0] = (dist(rng) * 2.0f - 1.0f) * 1.2f;  // X: -1.2 to 1.2
		surfel.Position[1] = dist(rng) * 2.0f * 0.5f;            // Y: 0 to 1.0
		surfel.Position[2] = (dist(rng) * 2.0f - 1.0f) * 1.2f;  // Z: -1.2 to 1.2

		// Random normal distribution (approximately uniform on sphere)
		float theta = dist(rng) * 6.28318f; // 2*PI
		float phi = std::acos(2.0f * dist(rng) - 1.0f);
		surfel.Normal[0] = std::sin(phi) * std::cos(theta);
		surfel.Normal[1] = std::sin(phi) * std::sin(theta);
		surfel.Normal[2] = std::cos(phi);

		// Random albedo with slight bias toward neutral
		const float baseAlbedo = 0.7f;
		surfel.Albedo[0] = baseAlbedo + (dist(rng) - 0.5f) * 0.3f;
		surfel.Albedo[1] = baseAlbedo + (dist(rng) - 0.5f) * 0.3f;
		surfel.Albedo[2] = baseAlbedo + (dist(rng) - 0.5f) * 0.3f;

		// Clamp albedo to valid range
		surfel.Albedo[0] = std::max(0.3f, std::min(1.0f, surfel.Albedo[0]));
		surfel.Albedo[1] = std::max(0.3f, std::min(1.0f, surfel.Albedo[1]));
		surfel.Albedo[2] = std::max(0.3f, std::min(1.0f, surfel.Albedo[2]));

		// Radius varies based on settings
		const float radiusRange = m_Settings.MaximumRadius - m_Settings.MinimumRadius;
		surfel.Radius = m_Settings.MinimumRadius + dist(rng) * radiusRange;

		// Initialize irradiance to zero (will be filled by lighting pass)
		surfel.Irradiance = {0.0f, 0.0f, 0.0f};

		surfels.push_back(surfel);
	}

	return surfels;
}
} // namespace PbrRtV2
