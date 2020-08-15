#include "AppWindow.h"
#include "EngineTime.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "MathUtils.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Point.h"
#include "SceneCameraHandler.h"
#include "UIManager.h"
#include "GameObjectManager.h"
#include "TextureManager.h"
#include "ShaderLibrary.h"
#include "BaseComponentSystem.h"
#include "PhysicsSystem.h"

static float f = 0.0f;
static int counter = 0;

struct Vertex {
	Vector3D position;
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
	srand(time(NULL));
	InputSystem::initialize();

	std::cout << "On create \n";
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	//std::cout << "App window update. \n";
	this->ticks += EngineTime::getDeltaTime();
	
	InputSystem::getInstance()->update();

	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();
	DeviceContext* deviceContext = graphEngine->getImmediateContext();
	ShaderNames shaderNames;
	deviceContext->clearRenderTargetColor(this->swapChain, 0, 0.5, 0.5, 1);

	RECT windowRect = this->getClientWindowRect();
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	deviceContext->setViewportSize(width, height);

	BaseComponentSystem::getInstance()->getPhysicsSystem()->updateAllComponents();
	GameObjectManager::getInstance()->updateAll();
	GameObjectManager::getInstance()->renderAll(width, height);
	SceneCameraHandler::getInstance()->update();
	UIManager::getInstance()->drawAllUI();

	graphEngine->getSwapChain()->present(true); //NOTE: Called once per screen refresh.
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	InputSystem::getInstance()->removeListener(this);
	InputSystem::getInstance()->destroy();
	this->vertexBuffer->release();
	this->indexBuffer->release();
	this->constantBuffer->release();
	this->swapChain->release();
	//this->vertexShader->release();
	//this->pixelShader->release();
	SceneCameraHandler::destroy();
	GraphicsEngine::destroy();
	ShaderLibrary::destroy();
	TextureManager::destroy();
	BaseComponentSystem::destroy();

	// IMGUI Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	std::cout << "On destroy \n";
}

void AppWindow::createGraphicsWindow()
{
	GraphicsEngine::initialize();
	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();
	ShaderLibrary::initialize();
	TextureManager::initialize();

	this->swapChain = GraphicsEngine::getInstance()->getSwapChain();
	RECT windowRect = this->getClientWindowRect();
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;
	std::cout << "Window rect width: " << width << "\n";
	std::cout << "Window rect height: " << height << "\n";

	this->swapChain->init(this->getWindowHandle(), width, height);

	GameObjectManager::initialize();
	SceneCameraHandler::initialize();
	BaseComponentSystem::initialize();
}

void AppWindow::createInterface()
{
	UIManager::initialize(this->windowHandle);
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'W') {
		std::cout << "Key W pressed down! \n";
	}
}

void AppWindow::onKeyUp(int key)
{
	if (key == 'W') {
		std::cout << "Key W pressed up! \n";
	}
}

void AppWindow::onMouseMove(const Point deltaPos)
{
}

void AppWindow::onLeftMouseDown(const Point deltaPos)
{
	//std::cout << "Left mouse down! \n";
}

void AppWindow::onLeftMouseUp(const Point deltaPos)
{
	//std::cout << "Left mouse up! \n";
}

void AppWindow::onRightMouseDown(const Point deltaPos)
{
	std::cout << "Right mouse down! \n";
	this->shouldRotate = true;
}

void AppWindow::onRightMouseUp(const Point deltaPos)
{
	std::cout << "Right mouse up! \n";
	this->shouldRotate = false;
}

void AppWindow::onFocus()
{
	InputSystem::getInstance()->addListener(this);
}

void AppWindow::onDefocus()
{
	InputSystem::getInstance()->removeListener(this);
}



AppWindow::AppWindow():Window()
{
	this->createWindow();
}

AppWindow::~AppWindow()
{
	Window::~Window();
}

