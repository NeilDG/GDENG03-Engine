#include "UIManager.h"
#include "GraphicsEngine.h"
#include "ProfilerScreen.h"
#include "MenuScreen.h"
#include "InspectorScreen.h"
#include "HierarchyScreen.h"
#include "PlaybackScreen.h"
#include "ActionScreen.h"
#include "ConsoleScreen.h"
#include "Debug.h"
#include "MaterialScreen.h"
#include "SceneCameraHandler.h"
#include "GizmoLayer.h"
#include "Viewport.h"


UIManager* UIManager::sharedInstance = NULL;

UIManager* UIManager::getInstance()
{
    return sharedInstance;
}

void UIManager::initialize(HWND windowHandle)
{
    sharedInstance = new UIManager(windowHandle);
}

void UIManager::destroy()
{
    delete sharedInstance;
}

void UIManager::drawAllUI()
{
	for (int i = 0; i < this->uiList.size(); i++) {
		this->uiList[i]->drawUI();
	}
}

void UIManager::setEnabled(String uiName, bool flag)
{
	if(this->uiTable[uiName] != nullptr)
	{
		this->uiTable[uiName]->SetEnabled(flag);
	}
}

AUIScreen* UIManager::findUIByName(String uiName)
{
	if(this->uiTable[uiName] != nullptr)
	{
		return this->uiTable[uiName];
	}
	else
	{
		return nullptr;
	}
}

UIManager::UIManager(HWND windowHandle)
{
    //initialize IMGUI interface
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(windowHandle);
	ImGui_ImplDX11_Init(GraphicsEngine::getInstance()->getDirectXDevice(), GraphicsEngine::getInstance()->getImmediateContext()->getContext());

    //populate UI table
	//UIs that will show during runtime
	UINames uiNames;
	ProfilerScreen* profilerScreen = new ProfilerScreen();
	this->uiTable[uiNames.PROFILER_SCREEN] = profilerScreen;
	this->uiList.push_back(profilerScreen);

	MenuScreen* menuScreen = new MenuScreen();
	this->uiTable[uiNames.MENU_SCREEN] = menuScreen;
	this->uiList.push_back(menuScreen);

	InspectorScreen* inspectorScreen = new InspectorScreen();
	this->uiTable[uiNames.INSPECTOR_SCREEN] = inspectorScreen;
	this->uiList.push_back(inspectorScreen);

	HierarchyScreen* hierarchyScreen = new HierarchyScreen();
	this->uiTable[uiNames.HIERARCHY_SCREEN] = hierarchyScreen;
	this->uiList.push_back(hierarchyScreen);

	PlaybackScreen* playbackScreen = new PlaybackScreen();
	this->uiTable[uiNames.PLAYBACK_SCREEN] = playbackScreen;
	this->uiList.push_back(playbackScreen);

	ActionScreen* actionScreen = new ActionScreen();
	this->uiTable[uiNames.ACTION_SCREEN] = actionScreen;
	this->uiList.push_back(actionScreen);

	ConsoleScreen* consoleScreen = new ConsoleScreen();
	this->uiTable[uiNames.CONSOLE_SCREEN] = consoleScreen;
	this->uiList.push_back(consoleScreen);
	Debug::assignConsole(consoleScreen);
	
	MaterialScreen* materialScreen = new MaterialScreen();
	this->uiTable[uiNames.MATERIAL_SCREEN] = materialScreen;
	this->uiList.push_back(materialScreen);
	materialScreen->SetEnabled(false);

	//gizmo layer - similar to Unity/Unreal Engine gizmos
	// GizmoLayer* gizmoLayer = new GizmoLayer();
	// this->uiTable[uiNames.GIZMO_LAYER] = gizmoLayer;
	// this->uiList.push_back(gizmoLayer);

	//viewport overlay for listening to raycasting
	this->viewport = new Viewport();
	
}

UIManager::~UIManager()
{
}

Viewport* UIManager::getViewportRef() const
{
	return this->viewport;
}
