#pragma once
#include <fstream>
#include "Vector3D.h"


class ColorUtils
{
	typedef std::ofstream FileStream;

	public:
		static float clamp(float x, float min, float max);
		static void writeColor(FileStream& outFile, Color pixelColor, int samplesPerPixel);
};

