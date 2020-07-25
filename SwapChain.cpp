#include "SwapChain.h"

SwapChain::SwapChain()
{

}

SwapChain::~SwapChain()
{
}

bool SwapChain::init(HWND windowHandle, UINT width, UINT height)
{
	GraphicsEngine::getInstance()->initializeSwapChain(windowHandle, this->chainRef, width, height);
	return this->chainRef != NULL;
}

bool SwapChain::release()
{
	if (this->chainRef != NULL) {
		this->chainRef->Release();
		delete this;
		return true;
	}
}
