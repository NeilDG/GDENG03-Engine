#include "ColorUtils.h"

float ColorUtils::clamp(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

void ColorUtils::writeColor(FileStream& outFile, Color pixelColor, int samplesPerPixel)
{
    float r = pixelColor.getX();
    float g = pixelColor.getY();
    float b = pixelColor.getZ();

	//gamma correction
    float scale = 1.0f / samplesPerPixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);
	
    // Write the translated [0,255] value of each color component.
    outFile << static_cast<int>(256 * clamp(r, 0.0, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999f)) << '\n';
}
