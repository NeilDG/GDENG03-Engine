#pragma once
#include "AGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <reactphysics3d/reactphysics3d.h>

class BasicRenderer;
class Cube :    public AGameObject
{
public:
	Cube(String name, bool skipInit = false);
	~Cube() override;

	void update(float deltaTime) override;
	void draw(int width, int height) override;
	bool testObjectSelect(Vector3D point) override;

	void attachRenderer(BasicRenderer* renderer);
	BasicRenderer* getRenderer() const;

protected:

	struct Vertex {
		Vector3D position;
		Vector3D color;
		Vector3D color2;
	};

	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;
	ConstantBuffer* constantBuffer = NULL;
	float ticks = 0.0f;

private:
	BasicRenderer* renderer;
	CollisionBody* clickableBody; //for object picking
};

