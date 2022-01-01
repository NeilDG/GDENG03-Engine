#include <iostream>
#include <fstream>
#include "ColorUtils.h"
#include "HittableList.h"
#include "MathUtils.h"
#include "Sphere.h"
#include "Camera.h"
#include "DiffuseMaterial.h"
#include "MetalMaterial.h"
#include "DielectricMaterial.h"

Color rayColor(const Ray r, const HittableList world, int bounces)
{
    HitRecord rec;

	//If the ray bounce limit is exceeded, no more light needs to be gathered
	if(bounces <= 0)
	{
        return Color(0.0f, 0.0f, 0.0f);
	}
	
    if (world.hit(r, 0.001f, MathUtils::infinity(), rec)) {
        Ray scattered;
        Color attenuation;

    	if(rec.refMaterial->scatter(r, rec, attenuation, scattered))
    	{
            return attenuation * rayColor(scattered, world, bounces - 1);
    	}
        // Point3D target = rec.p + rec.normal + Vector3D::randomInHemisphere(rec.normal);
        // return rayColor(Ray(rec.p, target - rec.p), world, bounces - 1) * 0.5f;
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
    const int raySamplesPerPixel = 100;
    const int bounces = 100;

	//World
    HittableList world;

    std::shared_ptr<DiffuseMaterial> groundMat = make_shared<DiffuseMaterial>(Color(0.8f, 0.8f, 0.0f));
    std::shared_ptr<DiffuseMaterial> centerCircleMat = make_shared<DiffuseMaterial>(Color(0.7f, 0.3f, 0.3f));
    // std::shared_ptr<MetalMaterial> leftCircleMat = make_shared<MetalMaterial>(Color(0.8f, 0.8f, 0.8f), 0.01f);
    // std::shared_ptr<MetalMaterial> rightCircleMat = make_shared<MetalMaterial>(Color(0.8f, 0.6f, 0.2f), 1.0f);
    std::shared_ptr<DielectricMaterial> leftCircleMat = make_shared<DielectricMaterial>(1.5f);
    std::shared_ptr<DielectricMaterial> rightCircleMat = make_shared<DielectricMaterial>(1.5f);
	
    world.add(make_shared<Sphere>(Point3D(0.0f, -100.5f, -1.0f), 100.0f, groundMat));
    world.add(make_shared<Sphere>(Point3D(0.0f, 0.0f, -1.0f), 0.5f, centerCircleMat));
    world.add(make_shared<Sphere>(Point3D(-1.0f, 0.0f, -1.0f), 0.5f, leftCircleMat));
    world.add(make_shared<Sphere>(Point3D(1.0f, 0.0f, -1.0f), 0.5f, rightCircleMat));
    //world.add(make_shared<Sphere>(Point3D(1.0f, 0.25f, -1.5f), 1.25f, matExtra1));
	
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
        	for (int s = 0; s < raySamplesPerPixel; s++)
        	{
                float u = (float(i) + MathUtils::randomFloat()) / (imageWidth - 1);
                float v = (float(j) + MathUtils::randomFloat()) / (imageHeight - 1);

                Ray r = cam.getRay(u, v);
                pixels = pixels + rayColor(r, world, bounces);
        	}   
            ColorUtils::writeColor(imageFile, pixels, raySamplesPerPixel);
        }
    }

    imageFile.close();
    std::cout << "Rendered scene saved to file." << std::endl;
}