#include <iostream>
#include <fstream>
#include "Vector3D.h"
#include "ColorUtils.h"

int main()
{
    // Image
    const int image_width = 256;
    const int image_height = 256;

    typedef std::ofstream FileStream;
    FileStream imageFile;
    imageFile.open("D:/Documents/GithubProjects/GDENG2-Engine/Part 11 - Ray Tracing One Weekend/ImageRender.ppm");
	
    // Render
    imageFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = image_height - 1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
        	
            auto r = double(i) / (image_width - 1);
            auto g = double(j) / (image_height - 1);
            auto b = i * j;

            Vector3D pixels(r, g, b);
            ColorUtils::writeColor(imageFile, pixels);
        }
    }

    imageFile.close();
    std::cout << "Rendered scene saved to file." << std::endl;
}