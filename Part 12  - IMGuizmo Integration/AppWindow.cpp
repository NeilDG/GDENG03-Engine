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
#include "EngineBackend.h"
#include "ActionHistory.h"
#include "Plane.h"
#include "Debug.h"
#include <sstream>

#include "GizmoLayer.h"

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
	deviceContext->clearRenderTargetColor(this->swapChain, 0.25, 0.25, 0.25, 1);

	RECT windowRect = this->getClientWindowRect();
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	deviceContext->setViewportSize(width, height);

	//update proper
	EngineBackend* backend = EngineBackend::getInstance();
	if (backend->getMode() == EngineBackend::EditorMode::PLAY) {
		BaseComponentSystem::getInstance()->getPhysicsSystem()->updateAllComponents();
		GameObjectManager::getInstance()->updateAll();
	}
	else if (backend->getMode() == EngineBackend::EditorMode::EDITOR) {
		GameObjectManager::getInstance()->updateAll();
		
	}
	else if (backend->getMode() == EngineBackend::EditorMode::PAUSED) {
		if (backend->insideFrameStep()) {
			BaseComponentSystem::getInstance()->getPhysicsSystem()->updateAllComponents();
			GameObjectManager::getInstance()->updateAll();
			backend->endFrameStep();
		}
	}

	
	//default updates on systems
	GameObjectManager::getInstance()->renderAll(width, height);
	SceneCameraHandler::getInstance()->update();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	GizmoLayer::getInstance()->draw();
	UIManager::getInstance()->drawAllUI();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	

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

	EngineBackend::destroy();
	SceneCameraHandler::destroy();
	GraphicsEngine::destroy();
	ShaderLibrary::destroy();
	TextureManager::destroy();
	BaseComponentSystem::destroy();
	ActionHistory::destroy();
	Debug::destroy();

	// IMGUI Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	std::cout << "On destroy \n";
}

void AppWindow::initializeEngine()
{
	Debug::initialize();
	EngineBackend::initialize();
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
	GizmoLayer::initialize();
	BaseComponentSystem::initialize();
	ActionHistory::initialize();
}

void AppWindow::createInterface() const
{
	UIManager::initialize(this->windowHandle);
}

void AppWindow::onKeyDown(int key)
{
	int CTRL_Y = 89;
	int CTRL_Z = 90;

	std::stringstream buffer;
	buffer << "Key pressed: " << key << '\n';
	Debug::Log(buffer.str());

	if (key == CTRL_Z) {
		
	}
	else if (key == CTRL_Y) {
		
	}
}

void AppWindow::onKeyUp(int key)
{
	int CTRL_Y = 89;
	int CTRL_Z = 90;
	int KEY_DELETE = 46;

	if (key == CTRL_Z) {
		if (ActionHistory::getInstance()->hasRemainingUndoActions()) {
			GameObjectManager::getInstance()->applyEditorAction(ActionHistory::getInstance()->undoAction());
		}
	}
	else if (key == CTRL_Y) {
		if (ActionHistory::getInstance()->hasRemainingRedoActions()) {
			GameObjectManager::getInstance()->applyEditorAction(ActionHistory::getInstance()->redoAction());
		}
	}
	else if (key == KEY_DELETE) {
		AGameObject* selectedObject = GameObjectManager::getInstance()->getSelectedObject();
		if (selectedObject != NULL) {
			GameObjectManager::getInstance()->deleteObject(selectedObject);
			GameObjectManager::getInstance()->setSelectedObject(NULL);
		}
	}
}

void AppWindow::onMouseMove(const Point deltaPos)
{
}

void AppWindow::onLeftMouseDown(const Point deltaPos, const Point currentPt)
{
	//std::cout << "Left mouse down! \n";
}

void AppWindow::onLeftMouseUp(const Point deltaPos)
{
	//std::cout << "Left mouse up! \n";
}

void AppWindow::onRightMouseDown(const Point deltaPos)
{
	//std::cout << "Right mouse down! \n";
	this->shouldRotate = true;
}

void AppWindow::onRightMouseUp(const Point deltaPos)
{
	//std::cout << "Right mouse up! \n";
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

