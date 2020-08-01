#include "AppWindow.h"
#include "EngineTime.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "Vector3D.h"
#include "Matrix4x4.h"

struct Vertex {
	Vector3D position;
	Vector3D position2;
	Vector3D color;
	Vector3D color2;

};
_declspec(align(16)) //make CBData a size of 16-bytes.
struct CBData {
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projMatrix;
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

	this->updateQuads();
	
	//std::cout << "On update \n";
}

void AppWindow::updateQuads()
{
	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();
	DeviceContext* deviceContext = graphEngine->getImmediateContext();

	RECT windowRect = this->getClientWindowRect();
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	CBData cbData = {};
	cbData.time = this->ticks * 10.0f;

	if (this->deltaPos > 1.0f) {
		this->deltaPos = 0.0f;
	}
	else {
		this->deltaPos += EngineTime::getDeltaTime() * 0.1f;
	}

	cbData.worldMatrix.setTranslation(Vector3D::lerp(Vector3D(-1.5f, -1.5f, 0), Vector3D(1.5f, 1.5f, 0), deltaPos));
	Matrix4x4 scaleMatrix; scaleMatrix.setScale(Vector3D::lerp(Vector3D(0.5, 0.5, 0), Vector3D(1.0f, 1.0f, 0), (sin(deltaPos * 20.0f) + 1.0f) / 2.0f));
	cbData.worldMatrix = cbData.worldMatrix.multiplyTo(scaleMatrix);
	cbData.viewMatrix.setIdentity();
	cbData.projMatrix.setOrthoLH(width / 400.0f, height / 400.0f, -4.0f, 4.0f);
	
	/*std::cout << "CB time is: " << cbData.time << " World matrix: \n";
	cbData.worldMatrix.debugPrint();

	std::cout << "Proj matrix: \n";
	cbData.projMatrix.debugPrint();*/

	this->constantBuffer->update(deviceContext, &cbData);
	deviceContext->setConstantBuffer(this->vertexShader, this->constantBuffer);
	deviceContext->setConstantBuffer(this->pixelShader, this->constantBuffer);

	deviceContext->setVertexBuffer(this->secondBuffer);
	UINT listSize = this->secondBuffer->getListSize();
	deviceContext->drawTriangleStrip(listSize, 0);
	this->swapChain->present(true);
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
	Vertex quadList[] = {
		//X, Y, Z
		{Vector3D(-0.5f,-0.5f,0.0f),    Vector3D(-0.32f,-0.11f,0.0f),   Vector3D(0,0,0), Vector3D(0,1,0) }, // POS1
		{Vector3D(-0.5f,0.5f,0.0f),     Vector3D(-0.11f,0.78f,0.0f),   Vector3D(1,1,0), Vector3D(0,1,1) }, // POS2
		{Vector3D(0.5f,-0.5f,0.0f),     Vector3D(0.75f,-0.73f,0.0f), Vector3D(0,0,1),  Vector3D(1,0,0) },// POS2
		{Vector3D(0.5f,0.5f,0.0f),     Vector3D(0.88f,0.77f,0.0f),    Vector3D(1,1,1), Vector3D(0,0,1) }
	};

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

