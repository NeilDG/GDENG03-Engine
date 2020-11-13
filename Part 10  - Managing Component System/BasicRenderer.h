#pragma once
#include "ABaseRenderer.h"

/// <summary>
/// Basic renderer that draws a simple color. Typically used for debugging purposes.
/// </summary>
class BasicRenderer: public ABaseRenderer
{
public:
	BasicRenderer();
	~BasicRenderer();
	void configureDeviceContext() const;
	
};

