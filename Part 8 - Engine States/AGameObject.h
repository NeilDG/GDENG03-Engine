#pragma once
#include <string>
#include "Vector3D.h"
#include "Matrix4x4.h"
#include <vector>
#include "AComponent.h"
#include "reactphysics3d/reactphysics3d.h"


using namespace reactphysics3d;

class GameObjectManager;
class VertexShader;
class PixelShader;
class EditorAction;
class AGameObject
{
public:
	struct AQuaternion {
		float w = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	typedef std::string String;
	typedef std::vector<AComponent*> ComponentList;

	_declspec(align(16)) //make CBData a size of 16-bytes.
		struct CBData {
		Matrix4x4 worldMatrix;
		Matrix4x4 viewMatrix;
		Matrix4x4 projMatrix;
		//unsigned int time; //size of only 4 bytes. this won't align in GPU device because device requires 16 bytes.
		float time;
	};

	AGameObject(String name);
	~AGameObject();

	virtual void update(float deltaTime);
	virtual void draw(int width, int height);

	void setPosition(float x, float y, float z);
	void setPosition(Vector3D pos);
	Vector3D getLocalPosition();

	void setScale(float x, float y, float z);
	void setScale(Vector3D scale);
	Vector3D getLocalScale();

	void setRotation(float x, float y, float z);
	void setRotation(Vector3D rot);
	void setRotation(float x, float y, float z, float w);
	Vector3D getLocalRotation();

	//void setEulerAnglesRotation(float x, float y, float z);
	//Vector3D getEulerAngles();
	
	bool isEnabled();
	void setEnabled(bool flag);
	String getName();
	friend class GameObjectManager;

	void attachComponent(AComponent* component);
	void detachComponent(AComponent* component);

	AComponent* findComponentByName(String name);
	AComponent* findComponentOfType(AComponent::ComponentType type, String name);
	ComponentList getComponentsOfType(AComponent::ComponentType type);
	ComponentList getComponentsOfTypeRecursive(AComponent::ComponentType type);

	void updateLocalMatrix(); //updates local matrix based from latest position, rotation, and scale.
	void setLocalMatrix(float matrix[16]);
	float* getRawMatrix();
	Matrix4x4 getLocalMatrix();
	float* getPhysicsLocalMatrix(); //scale is set to 1.0

	virtual void saveEditState();
	virtual void restoreEditState();

protected:
	String name;
	Vector3D localPosition;
	Vector3D localScale;
	AQuaternion orientation;
	Matrix4x4 localMatrix;

	ComponentList componentList;

	bool overrideMatrix = false;

	virtual void awake();

private:
	bool enabled = true;

	EditorAction* lastEditState = NULL; //used for storing the state of this object prior to play mode.

};

