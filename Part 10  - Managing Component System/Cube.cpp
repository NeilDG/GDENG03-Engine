#include "Cube.h"
#include "GraphicsEngine.h"
#include "InputSystem.h"
#include "SwapChain.h"
#include "SceneCameraHandler.h"
#include "ShaderLibrary.h"
#include "BasicRenderer.h"

Cube::Cube(String name, bool skipInit):AGameObject(name, AGameObject::PrimitiveType::CUBE)
{
	if (skipInit) {
		return;
	}

	ShaderNames shaderNames;
	void* shaderByteCode = NULL;
	size_t sizeShader = 0;
	ShaderLibrary::getInstance()->requestVertexShaderData(shaderNames.BASE_VERTEX_SHADER_NAME, &shaderByteCode, &sizeShader);

	//create buffers for drawing. vertex data that needs to be drawn are temporarily placed here.
	Vertex quadList[] = {
		//X, Y, Z
		//FRONT FACE
		{Vector3D(-0.5f,-0.5f,-0.5f),    Vector3D(1,1,1),  Vector3D(0.2f,0,0) },
		{Vector3D(-0.5f,0.5f,-0.5f),    Vector3D(1,1,1), Vector3D(0.2f,0.2f,0) },
		{Vector3D(0.5f,0.5f,-0.5f),   Vector3D(0,0,0),  Vector3D(0.2f,0.2f,0) },
		{Vector3D(0.5f,-0.5f,-0.5f),     Vector3D(0,0,0), Vector3D(0.2f,0,0) },

		//BACK FACE
		{Vector3D(0.5f,-0.5f,0.5f),    Vector3D(1,0,0), Vector3D(0,0.2f,0) },
		{Vector3D(0.5f,0.5f,0.5f),    Vector3D(1,1,0), Vector3D(0,0.2f,0.2f) },
		{Vector3D(-0.5f,0.5f,0.5f),   Vector3D(0,0,0),  Vector3D(0,0.2f,0.2f) },
		{Vector3D(-0.5f,-0.5f,0.5f),     Vector3D(0,0,0), Vector3D(0,0.2f,0) },
	};

	this->vertexBuffer = GraphicsEngine::getInstance()->createVertexBuffer();
	this->vertexBuffer->load(quadList, sizeof(Vertex), ARRAYSIZE(quadList), shaderByteCode, sizeShader);

	unsigned int indexList[] =
	{
		//FRONT SIDE
		0,1,2,  //FIRST TRIANGLE
		2,3,0,  //SECOND TRIANGLE
		//BACK SIDE
		4,5,6,
		6,7,4,
		//TOP SIDE
		1,6,5,
		5,2,1,
		//BOTTOM SIDE
		7,0,3,
		3,4,7,
		//RIGHT SIDE
		3,2,5,
		5,4,3,
		//LEFT SIDE
		7,6,1,
		1,0,7
	};
	this->indexBuffer = GraphicsEngine::getInstance()->createIndexBuffer();
	this->indexBuffer->load(indexList, ARRAYSIZE(indexList));

	//create constant buffer
	CBData cbData = {};
	cbData.time = 0;
	this->constantBuffer = GraphicsEngine::getInstance()->createConstantBuffer();
	this->constantBuffer->load(&cbData, sizeof(CBData));

	BasicRenderer* basicRenderer = new BasicRenderer();
	this->setRenderer(basicRenderer);
}

Cube::~Cube()
{
	this->vertexBuffer->release();
	this->indexBuffer->release();
	delete this->renderer;
	AGameObject::~AGameObject();
}

void Cube::update(float deltaTime)
{ 

}

void Cube::draw(int width, int height)
{
	//set vertex shader and pixel shader for the object
	//ShaderNames shaderNames;
	//DeviceContext* deviceContext = GraphicsEngine::getInstance()->getImmediateContext();
	//deviceContext->setRenderConfig(ShaderLibrary::getInstance()->getVertexShader(shaderNames.BASE_VERTEX_SHADER_NAME), ShaderLibrary::getInstance()->getPixelShader(shaderNames.BASE_PIXEL_SHADER_NAME));

	this->renderer->configureDeviceContext();
	
	CBData cbData = {};

	if (this->overrideMatrix) {
		cbData.worldMatrix = this->localMatrix;
	}
	else {
		this->updateLocalMatrix();
		cbData.worldMatrix = this->localMatrix;
	}
	
	Matrix4x4 cameraMatrix = SceneCameraHandler::getInstance()->getSceneCameraViewMatrix();
	cbData.viewMatrix = cameraMatrix;

	float aspectRatio = (float)width / (float)height;
	//cbData.projMatrix.setOrthoLH(width / 400.0f, height / 400.0f, -4.0f, 4.0f);
	cbData.projMatrix.setPerspectiveFovLH(2, aspectRatio, 0.1f, 1000.0f);

	DeviceContext* deviceContext = GraphicsEngine::getInstance()->getImmediateContext();
	this->constantBuffer->update(deviceContext, &cbData);
	deviceContext->setConstantBuffer(this->constantBuffer);
	deviceContext->setIndexBuffer(this->indexBuffer);
	deviceContext->setVertexBuffer(this->vertexBuffer);

	deviceContext->drawTriangle(this->indexBuffer->getIndexSize(), 0, 0);
}

void Cube::setRenderer(ABaseRenderer* renderer)
{
	//delete old renderer
	if(this->renderer != nullptr)
	{
		delete this->renderer;
	}
	
	this->renderer = renderer;
}

ABaseRenderer* Cube::getRenderer() const
{
	return static_cast<ABaseRenderer*>(this->renderer);
}
