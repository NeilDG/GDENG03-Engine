#include "ColorUtils.h"
#include "Vector3D.h"

void ColorUtils::writeColor(FileStream& outFile, Vector3D pixelColor)
{
    // Write the translated [0,255] value of each color component.
    outFile << static_cast<int>(255.999 * pixelColor.getX()) << ' '
        << static_cast<int>(255.999 * pixelColor.getY()) << ' '
        << static_cast<int>(255.999 * pixelColor.getZ()) << '\n';
}
