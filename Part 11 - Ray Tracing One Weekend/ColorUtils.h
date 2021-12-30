#pragma once
#include <fstream>

class Vector3D;
class ColorUtils
{
	typedef std::ofstream FileStream;

	public:
		static void writeColor(FileStream& outFile, Vector3D pixelColor);
};

