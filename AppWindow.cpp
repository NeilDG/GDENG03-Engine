#include "AppWindow.h"

struct Vector3 {
	float x, y, z;
};

struct Vertex {
	Vector3 position;
};

AppWindow* AppWindow::sharedInstance = NULL;

AppWindow* AppWindow::getInstance()
{
	return sharedInstance;
}

void AppWindow::initialize()
{
	sharedInstance = new AppWindow();
}

void AppWindow::onCreate()
{
	Window::onCreate();
	std::cout << "On create \n";
}

void AppWindow::onUpdate()
{
	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();
	DeviceContext* deviceContext = graphEngine->getImmediateContext();

	deviceContext->clearRenderTargetColor(this->swapChain, 0, 0.5, 0.5, 1);

	RECT windowRect = this->getClientWindowRect();
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	deviceContext->setViewportSize(width, height);
	GraphicsEngine::getInstance()->setShaders();
	deviceContext->setVertexBuffer(this->vertexBuffer);
	UINT listSize = this->vertexBuffer->getListSize();
	deviceContext->drawTriangle(listSize, 0);

	this->swapChain->present(true);
	//std::cout << "On update \n";
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->vertexBuffer->release();
	this->swapChain->release();
	GraphicsEngine::destroy();
	std::cout << "On destroy \n";
}

void AppWindow::createGraphicsWindow()
{
	GraphicsEngine::initialize();
	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();

	this->swapChain = GraphicsEngine::getInstance()->createSwapChain();
	RECT windowRect = this->getClientWindowRect();
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;
	std::cout << "Window rect width: " << width << "\n";
	std::cout << "Window rect height: " << height << "\n";

	this->swapChain->init(this->getWindowHandle(), width, height);

	Vertex list[] = {
		//X, Y, Z
		{-0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f}
	};
	
	graphEngine->createShaders();

	void* shaderByteCode = nullptr;
	UINT sizeShader = 0;
	graphEngine->getShaderBufferAndSize(&shaderByteCode, &sizeShader);

	this->vertexBuffer = GraphicsEngine::getInstance()->createVertexBuffer();
	this->vertexBuffer->load(list, sizeof(Vertex), ARRAYSIZE(list), shaderByteCode, sizeShader);

	DeviceContext* deviceContext = graphEngine->getImmediateContext();
	deviceContext->setVertexBuffer(this->vertexBuffer);
}

AppWindow::AppWindow():Window()
{
	this->createWindow();
}

AppWindow::~AppWindow()
{
	Window::~Window();
}

