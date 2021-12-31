#include "ColorUtils.h"

void ColorUtils::writeColor(FileStream& outFile, Color pixelColor)
{
    // Write the translated [0,255] value of each color component.
    outFile << static_cast<int>(255.999 * pixelColor.getX()) << ' '
        << static_cast<int>(255.999 * pixelColor.getY()) << ' '
        << static_cast<int>(255.999 * pixelColor.getZ()) << '\n';
}
