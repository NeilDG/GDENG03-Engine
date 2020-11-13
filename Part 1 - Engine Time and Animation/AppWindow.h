#pragma once
#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"

class AppWindow : public Window
{
public:
	static AppWindow* getInstance();
	static void initialize();
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;

	void initializeEngine();

private:
	AppWindow();
	~AppWindow();
	AppWindow(AppWindow const&) {};             // copy constructor is private
	AppWindow& operator=(AppWindow const&) {};  // assignment operator is private*/
	static AppWindow* sharedInstance;

	SwapChain* swapChain;
	VertexBuffer* vertexBuffer;
	VertexBuffer* secondBuffer;
	ConstantBuffer* constantBuffer;

	VertexShader* vertexShader;
	PixelShader* pixelShader;

	float ticks = 0.0f;
	float speedFactor = 0.1f;
	bool increasing = false;

	float CHANGE_DELAY = 1.0f;
	bool displayAlt = false;

};

