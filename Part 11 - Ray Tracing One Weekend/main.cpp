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

HittableList generateRandomScene()
{
    HittableList world;
    std::shared_ptr<DiffuseMaterial> groundMat = make_shared<DiffuseMaterial>(Color(0.5f, 0.5f, 0.5f));
    world.add(make_shared<Sphere>(Point3D(0.0f, -1000.0f, 0.0f), 1000.0f, groundMat));

	for(int a = -11; a< 11; a++)
	{
		for(int b= -11; b < 11; b++)
		{
            float matVal = MathUtils::randomFloat();
            Point3D center(a + 0.9f * MathUtils::randomFloat(), 0.2, b + 0.9 * MathUtils::randomFloat());

			if((center - Point3D(4.0, 0.2f, 0.0f)).length() > 0.9f)
			{
                shared_ptr<AMaterial> refMaterial;

				if(matVal < 0.8)
				{
                    Color albedo = static_cast<Color> (Vector3D::random() * Vector3D::random());
                    refMaterial = make_shared<DiffuseMaterial>(albedo);
                    world.add(make_shared<Sphere>(center, 0.2f, refMaterial));
				}
                else if(matVal < 0.95)
                {
                    Color albedo = static_cast<Color> (Vector3D::random() * Vector3D::random());
                    float fuzziness = MathUtils::randomFloat(0.0f, 0.5f);
                    refMaterial = make_shared<MetalMaterial>(albedo, fuzziness);
                    world.add(make_shared<Sphere>(center, 0.2f, refMaterial));
                }
                else
                {
                    refMaterial = make_shared<DielectricMaterial>(1.5f);
                    world.add(make_shared<Sphere>(center, 0.2f, refMaterial));
                }
			}
		}
	}
	
    return world;
}

int main()
{
    // Image
    const float aspectRatio = 3.0f / 2.0f;
    const int imageWidth = 1200;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    const int raySamplesPerPixel = 100;
    const int bounces = 50;

	//World
    HittableList world;

    std::shared_ptr<DiffuseMaterial> groundMat = make_shared<DiffuseMaterial>(Color(0.8f, 0.8f, 0.0f));
    std::shared_ptr<DiffuseMaterial> centerCircleMat = make_shared<DiffuseMaterial>(Color(0.4f, 0.2f, 0.1f));
    std::shared_ptr<DielectricMaterial> leftCircleMat = make_shared<DielectricMaterial>(1.5f);
    std::shared_ptr<MetalMaterial> rightCircleMat = make_shared<MetalMaterial>(Color(0.7f, 0.9f, 0.9f), 0.0f);

    world = generateRandomScene();
	
    world.add(make_shared<Sphere>(Point3D(-4.0f, 1.0f, 0.0f), 1.0f, centerCircleMat));
    world.add(make_shared<Sphere>(Point3D(0.0f, 1.0f, 0.0f), 1.0f, leftCircleMat));
    world.add(make_shared<Sphere>(Point3D(0.0f, 1.0f, 0.0f), -0.9f, leftCircleMat));
    world.add(make_shared<Sphere>(Point3D(4.0f, 1.0f, 0.0f), 1.0f, rightCircleMat));
	
	//Camera
    Point3D lookFrom(13.0f, 2.0f, 3.0f);
    Point3D lookAt(0.0f, 0.0f, 0.0f);
    Vector3D vUp(0.0f, 1.0f, 0.0f);
    float distToFocus = (lookFrom - lookAt).length();
    float aperture = 0.1f;
	
    Camera cam(lookFrom, lookAt, vUp, 20, aspectRatio, aperture, distToFocus); //far view
    //Camera cam(Point3D(-2.0f, 2.0f, 1.0f), Point3D(0.0f, 0.0f, -1.0f), Vector3D(0.0f, 1.0f, 0.0f), 20, aspectRatio); //zoom view

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