#include <iostream>
#include <fstream>
#include "ColorUtils.h"
#include "HittableList.h"
#include "MathUtils.h"
#include "Sphere.h"

Color rayColor(const Ray r, const HittableList world)
{
    HitRecord rec;
    if (world.hit(r, 0.0f, MathUtils::infinity(), rec)) {
        return (rec.normal + Color(1.0f, 1.0f, 1.0f)) * 0.5f;
    }
	
    Vector3D unitDir = Vector3D::unitVector(r.getDirection());
    float t = 0.5f * (unitDir.getY() + 1.0f);
    return Color(1.0f, 1.0f, 1.0f) * (1.0f - t) + Color(0.5f, 0.7f, 1.0f) * t;
}

int main()
{
    // Image
    const float aspectRatio = 16.0f / 9.0f;
    const int imageWidth = 400;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);

	//World
    HittableList world;
    world.add(make_shared<Sphere>(Point3D(0.0f, 0.0f, -1.0f), 0.5f));
    world.add(make_shared<Sphere>(Point3D(0.0f, -100.5f, -1.0f), 100.0f));
	
	//Camera
    float viewportHeight = 2.0f;
    float viewportWidth = aspectRatio * viewportHeight;
    float focalLength = 1.0f;

    Point3D origin = Point3D(0.0f, 0.0f, 0.0f);
    Vector3D horizontal = Vector3D(viewportWidth, 0.0f, 0.0f);
    Vector3D vertical = Vector3D(0.0f, viewportHeight, 0.0f);
    Vector3D lowerLeft = origin - horizontal / 2.0f - vertical / 2.0f - Vector3D(0.0f, 0.0f, focalLength);

    typedef std::ofstream FileStream;
    FileStream imageFile;
    imageFile.open("D:/Documents/GithubProjects/GDENG2-Engine/Part 11 - Ray Tracing One Weekend/ImageRender.ppm");
	
    // Render
    imageFile << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
    for (int j = imageHeight - 1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++) {
        	
            auto u = double(i) / (imageWidth - 1);
            auto v = double(j) / (imageHeight - 1);

            Ray r(origin, lowerLeft + horizontal * u + vertical * v - origin);

            Color pixels = rayColor(r, world);
            ColorUtils::writeColor(imageFile, pixels);
        }
    }

    imageFile.close();
    std::cout << "Rendered scene saved to file." << std::endl;
}