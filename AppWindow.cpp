#include "AppWindow.h"
#include "EngineTime.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"

struct Vector3 {
	float x, y, z;
};

struct Vertex {
	Vector3 position;
	Vector3 position2;
	Vector3 color;
	Vector3 color2;

};
_declspec(align(16)) //make CBData a size of 16-bytes.
struct CBData {
	//unsigned int time; //size of only 4 bytes. this won't align in GPU device because device requires 16 bytes.
	float time;
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
	this->ticks += EngineTime::getDeltaTime();
	
	/*if (this->ticks > this->CHANGE_DELAY) {
		this->ticks = 0.0f;
		this->displayAlt = !this->displayAlt;
	}*/

	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();
	DeviceContext* deviceContext = graphEngine->getImmediateContext();
	deviceContext->setVertexShader(this->vertexShader);
	deviceContext->setPixelShader(this->pixelShader);
	deviceContext->clearRenderTargetColor(this->swapChain, 0, 0.5, 0.5, 1);

	RECT windowRect = this->getClientWindowRect();
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	deviceContext->setViewportSize(width, height);

	CBData cbData = {};
	cbData.time = this->ticks;

	std::cout << "CB time is: " << cbData.time << "\n";

	this->constantBuffer->update(deviceContext, &cbData);
	deviceContext->setConstantBuffer(this->vertexShader, this->constantBuffer);
	deviceContext->setConstantBuffer(this->pixelShader, this->constantBuffer);

	if (this->displayAlt) {
		deviceContext->setVertexBuffer(this->vertexBuffer);
		UINT listSize = this->vertexBuffer->getListSize();
		deviceContext->drawTriangle(listSize, 0);
		this->swapChain->present(true);
	}
	else {
		deviceContext->setVertexBuffer(this->secondBuffer);
		UINT listSize = this->secondBuffer->getListSize();
		deviceContext->drawTriangleStrip(listSize, 0);
		this->swapChain->present(true);
	}
	//std::cout << "On update \n";
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->vertexBuffer->release();
	this->swapChain->release();
	this->vertexShader->release();
	this->pixelShader->release();
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

	void* shaderByteCode = nullptr;
	size_t sizeShader = 0;

	//vertex stage
	graphEngine->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);
	this->vertexShader = graphEngine->createVertexShader(shaderByteCode, sizeShader);

	//create buffers for drawing. vertex data that needs to be drawn are temporarily placed here.
	Vertex triangleList[] = {
		//X, Y, Z, R, G, B
		{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
		{0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f}
	};

	Vertex quadList[] = {
		//X, Y, Z
		{-0.5f,-0.5f,0.0f,    -0.32f,-0.11f,0.0f,   0,0,0,  0,1,0 }, // POS1
		{-0.5f,0.5f,0.0f,     -0.11f,0.78f,0.0f,    1,1,0,  1,1,0 }, // POS2
		{ 0.5f,-0.5f,0.0f,     0.75f,-0.73f,0.0f,   0,0,1,  1,0,0 },// POS2
		{ 0.5f,0.5f,0.0f,      0.88f,0.77f,0.0f,    1,1,1,  0,0,1 }
	};

	this->vertexBuffer = GraphicsEngine::getInstance()->createVertexBuffer();
	this->vertexBuffer->load(triangleList, sizeof(Vertex), ARRAYSIZE(triangleList), shaderByteCode, sizeShader);

	this->secondBuffer = GraphicsEngine::getInstance()->createVertexBuffer();
	this->secondBuffer->load(quadList, sizeof(Vertex), ARRAYSIZE(quadList), shaderByteCode, sizeShader);

	graphEngine->releaseCompiledShader(); //this must be called after all buffers declared.

	//pixel shading stage
	graphEngine->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);
	this->pixelShader = graphEngine->createPixelShader(shaderByteCode, sizeShader);
	graphEngine->releaseCompiledShader();

	//create constant buffer
	CBData cbData = {};
	cbData.time = 0;
	this->constantBuffer = GraphicsEngine::getInstance()->createConstantBuffer();
	this->constantBuffer->load(&cbData, sizeof(CBData));
}

AppWindow::AppWindow():Window()
{
	this->createWindow();
}

AppWindow::~AppWindow()
{
	Window::~Window();
}

