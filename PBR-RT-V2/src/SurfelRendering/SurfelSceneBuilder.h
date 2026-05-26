#pragma once

#include <vector>
#include <array>
#include <cmath>

#include "Surfel.h"

namespace Diligent
{
namespace GLTF
{
struct Model;
}
}

namespace PbrRtV2
{
class SurfelTriangleSampler
{
public:
	// Sample a surfel from a triangle given its three vertices
	// Returns the surfel placed at the triangle's center with interpolated normal
	static Surfel SampleFromTriangle(
		const std::array<float, 3>& v0,
		const std::array<float, 3>& v1,
		const std::array<float, 3>& v2,
		const std::array<float, 3>& n0,
		const std::array<float, 3>& n1,
		const std::array<float, 3>& n2,
		const std::array<float, 3>& albedo,
		float radius);

	// Calculate the area of a triangle
	static float CalculateTriangleArea(
		const std::array<float, 3>& v0,
		const std::array<float, 3>& v1,
		const std::array<float, 3>& v2);

	// Calculate the number of surfels to place on a triangle based on area and density
	static int CalculateSurfelCount(float triangleArea, float placementDensity);
};

class SurfelSceneBuilder
{
public:
	void SetSettings(const SurfelPrototypeSettings& settings);
	const SurfelPrototypeSettings& GetSettings() const;

	// Set the GLTF model to bake surfels from
	void SetModel(Diligent::GLTF::Model* pModel);

	// Build surfels from the static Sponza scene
	std::vector<Surfel> BuildStaticSponzaSurfels() const;

private:
	SurfelPrototypeSettings m_Settings = {};
	Diligent::GLTF::Model* m_pModel = nullptr;
};
} // namespace PbrRtV2
