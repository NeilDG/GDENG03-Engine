#include <iostream>
#include <fstream>
#include "ColorUtils.h"
#include "HittableList.h"
#include "MathUtils.h"
#include "Sphere.h"
#include "Camera.h"


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
    const int samplesPerPixel = 1000;

	//World
    HittableList world;
    world.add(make_shared<Sphere>(Point3D(0.0f, 0.0f, -1.0f), 0.5f));
    world.add(make_shared<Sphere>(Point3D(0.0f, -100.5f, -1.0f), 100.0f));
	
	//Camera
    Camera cam;

    // Render
    typedef std::ofstream FileStream;
    FileStream imageFile;
    imageFile.open("D:/Documents/GithubProjects/GDENG2-Engine/Part 11 - Ray Tracing One Weekend/ImageRender.ppm");
    imageFile << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
    for (int j = imageHeight - 1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++) {

            Color pixels(0.0f, 0.0f, 0.0f);
        	for (int s = 0; s < samplesPerPixel; s++)
        	{
                float u = (float(i) + MathUtils::randomFloat()) / (imageWidth - 1);
                float v = (float(j) + MathUtils::randomFloat()) / (imageHeight - 1);

                Ray r = cam.getRay(u, v);
                pixels = pixels + rayColor(r, world);
        	}   
            ColorUtils::writeColor(imageFile, pixels, samplesPerPixel);
        }
    }

    imageFile.close();
    std::cout << "Rendered scene saved to file." << std::endl;
}