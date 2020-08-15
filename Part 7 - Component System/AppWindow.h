#pragma once
#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "Cube.h"
#include <vector>
#include "InputSystem.h"

typedef std::vector<Cube*> CubeList;

class AppWindow : public Window, public InputListener
{
public:
	static AppWindow* getInstance();
	static void initialize();
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;

	void createGraphicsWindow();
	void createInterface();

	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;
	virtual void onMouseMove(const Point deltaPos) override;
	virtual void onLeftMouseDown(const Point deltaPos) override;
	virtual void onLeftMouseUp(const Point deltaPos) override;
	virtual void onRightMouseDown(const Point deltaPos) override;
	virtual void onRightMouseUp(const Point deltaPos) override;

	virtual void onFocus();
	virtual void onDefocus();

private:
	AppWindow();
	~AppWindow();
	AppWindow(AppWindow const&) {};             // copy constructor is private
	AppWindow& operator=(AppWindow const&) {};  // assignment operator is private*/
	static AppWindow* sharedInstance;

	SwapChain* swapChain;
	VertexBuffer* vertexBuffer;
	VertexBuffer* secondBuffer;
	IndexBuffer* indexBuffer;
	ConstantBuffer* constantBuffer;

	//VertexShader* vertexShader;
	//PixelShader* pixelShader;

	float ticks = 0.0f;
	float CHANGE_DELAY = 1.0f;

	float deltaPos = 0.0f;
	bool shouldRotate = false;

};

