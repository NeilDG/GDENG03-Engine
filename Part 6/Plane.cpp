#include "Plane.h"
#include "GraphicsEngine.h"
#include "ShaderLibrary.h"

Plane::Plane(String name): Cube(name)
{
	ShaderNames shaderNames;
	void* shaderByteCode = NULL;
	size_t sizeShader = 0;
	ShaderLibrary::getInstance()->requestVertexShaderData(shaderNames.BASE_VERTEX_SHADER_NAME, &shaderByteCode, &sizeShader);

	//override cube constructor with different renderer
	//create buffers for drawing. vertex data that needs to be drawn are temporarily placed here.
	Vertex quadList[] = {
		//X, Y, Z
		//FRONT FACE
		{Vector3D(-0.5f,-0.5f,-0.5f),    Vector3D(1,1,1),  Vector3D(0.2f,0,0) },
		{Vector3D(-0.5f,0.5f,-0.5f),    Vector3D(1,1,1), Vector3D(0.2f,0.2f,0) },
		{Vector3D(0.5f,0.5f,-0.5f),   Vector3D(1,1,1),  Vector3D(0.2f,0.2f,0) },
		{Vector3D(0.5f,-0.5f,-0.5f),     Vector3D(1,1,1), Vector3D(0.2f,0,0) },

		//BACK FACE
		{Vector3D(0.5f,-0.5f,0.5f),    Vector3D(1,1,1), Vector3D(0,0.2f,0) },
		{Vector3D(0.5f,0.5f,0.5f),    Vector3D(1,1,1), Vector3D(0,0.2f,0.2f) },
		{Vector3D(-0.5f,0.5f,0.5f),   Vector3D(1,1,1),  Vector3D(0,0.2f,0.2f) },
		{Vector3D(-0.5f,-0.5f,0.5f),     Vector3D(1,1,1), Vector3D(0,0.2f,0) },
	};

	//this->vertexBuffer->release();
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

	this->indexBuffer->release();
	this->indexBuffer = GraphicsEngine::getInstance()->createIndexBuffer();
	this->indexBuffer->load(indexList, ARRAYSIZE(indexList));

	this->constantBuffer->release();
	//create constant buffer
	CBData cbData = {};
	cbData.time = 0;
	this->constantBuffer = GraphicsEngine::getInstance()->createConstantBuffer();
	this->constantBuffer->load(&cbData, sizeof(CBData));

	this->setScale(8.0f, 8.0f, 0.1f);
	this->setRotation(190.0f, 0.0f, 0.0f);
}

Plane::~Plane()
{
}

void Plane::update(float deltaTime)
{
	Cube::update(deltaTime);
}

void Plane::draw(int width, int height)
{
	Cube::draw(width, height);
}
