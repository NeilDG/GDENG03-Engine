#pragma once
#include <system_error>
#include <d3d11.h>

class LogUtils
{
public:
	static void PrintHRResult(HRESULT res);
};

