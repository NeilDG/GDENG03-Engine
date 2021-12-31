#pragma once
#include <fstream>
#include "Vector3D.h"


class ColorUtils
{
	typedef std::ofstream FileStream;

	public:
		static void writeColor(FileStream& outFile, Color pixelColor);
};

