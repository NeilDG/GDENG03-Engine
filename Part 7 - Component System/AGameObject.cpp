#include "AGameObject.h"


AGameObject::AGameObject(String name)
{
	this->name = name;
	//this->localRotation = Vector3D::zeros();
	this->localPosition = Vector3D::zeros();
	this->localScale = Vector3D::ones();
	this->localMatrix.setIdentity();
}

AGameObject::~AGameObject()
{
	for (int i = 0; i < this->componentList.size(); i++) {
		this->componentList[i]->detachOwner();
	}
	this->componentList.clear();
}

void AGameObject::update(float deltaTime)
{

}

void AGameObject::draw(int width, int height)
{
	//do nothing
}

void AGameObject::setPosition(float x, float y, float z)
{
	this->localPosition = Vector3D(x, y, z);
	this->overrideMatrix = false;
}

void AGameObject::setPosition(Vector3D pos)
{
	this->localPosition = pos;
	this->overrideMatrix = false;
}

Vector3D AGameObject::getLocalPosition()
{
	return this->localPosition;
}

void AGameObject::setScale(float x, float y, float z)
{
	this->localScale = Vector3D(x, y, z);
	this->overrideMatrix = false;
}

void AGameObject::setScale(Vector3D scale)
{
	this->localScale = scale;
	this->overrideMatrix = false;
}

void AGameObject::setRotation(float x, float y, float z)
{
	//this->localRotation = Vector3D(x, y, z);
	this->orientation = {};
	this->orientation.x = x;
	this->orientation.y = y;
	this->orientation.z = z;
	this->overrideMatrix = false;
}

void AGameObject::setRotation(Vector3D rot)
{
	//this->localRotation = rot;
	this->orientation = {};
	this->orientation.x = rot.getX();
	this->orientation.y = rot.getY();
	this->orientation.z = rot.getZ();
	this->overrideMatrix = false;
}

void AGameObject::setRotation(float x, float y, float z, float w)
{
	this->orientation = {};
	this->orientation.x = x;
	this->orientation.y = y;
	this->orientation.z = z;
	this->orientation.w = w;
	this->overrideMatrix = false;
}

Vector3D AGameObject::getLocalRotation()
{
	return Vector3D(this->orientation.x, this->orientation.y, this->orientation.z);
	//return this->localRotation;
}

/*void AGameObject::setEulerAnglesRotation(float x, float y, float z)
{
	this->eulerAngles = Vector3D(x, y, z);
	Quaternion quaternion = Quaternion::fromEulerAngles(x, y, z);
	this->localRotation = Vector3D(quaternion.getVectorV().x, quaternion.getVectorV().y, quaternion.getVectorV().z);
}

Vector3D AGameObject::getEulerAngles()
{
	return this->eulerAngles;
}

void AGameObject::storeRotMatrix(Matrix3x3 matrix3x3)
{
	float matrix4x4[4][4];
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			matrix4x4[i][j] = matrix3x3[i][j];
		}
	}
	
	this->rotationMatrix.setMatrix(matrix4x4);
}*/

bool AGameObject::isEnabled()
{
	return this->enabled;
}

void AGameObject::setEnabled(bool flag)
{
	this->enabled = flag;
}

Vector3D AGameObject::getLocalScale()
{
	return this->localScale;
}

AGameObject::String AGameObject::getName()
{
	return this->name;
}

void AGameObject::attachComponent(AComponent* component)
{
	this->componentList.push_back(component);
	component->attachOwner(this);
}

void AGameObject::detachComponent(AComponent* component)
{
	int index = -1;
	for (int i = 0; i < this->componentList.size(); i++) {
		if (this->componentList[i] == component) {
			index = i;
			break;
		}
	}
	if (index != -1) {
		this->componentList.erase(this->componentList.begin() + index);
	}
}

AComponent* AGameObject::findComponentByName(String name)
{
	for (int i = 0; i < this->componentList.size(); i++) {
		if (this->componentList[i]->getName() == name) {
			return this->componentList[i];
		}
	}

	return NULL;
}

AComponent* AGameObject::findComponentOfType(AComponent::ComponentType type, String name)
{
	for (int i = 0; i < this->componentList.size(); i++) {
		if (this->componentList[i]->getName() == name && this->componentList[i]->getType() == type) {
			return this->componentList[i];
		}
	}

	return NULL;
}

AGameObject::ComponentList AGameObject::getComponentsOfType(AComponent::ComponentType type)
{
	ComponentList foundList;
	for (int i = 0; i < this->componentList.size(); i++) {
		if (this->componentList[i]->getType() == type) {
			foundList.push_back(this->componentList[i]);
		}
	}

	return foundList;
}

AGameObject::ComponentList AGameObject::getComponentsOfTypeRecursive(AComponent::ComponentType type)
{
	ComponentList foundList;
	for (int i = 0; i < this->componentList.size(); i++) {
		if (this->componentList[i]->getType() == type) {
			foundList.push_back(this->componentList[i]);
		}
	}

	return foundList;
}

void AGameObject::updateLocalMatrix()
{
	//setup transformation matrix for drawing.
	Matrix4x4 allMatrix; allMatrix.setIdentity();
	Matrix4x4 translationMatrix; translationMatrix.setIdentity();  translationMatrix.setTranslation(this->getLocalPosition());
	Matrix4x4 scaleMatrix; scaleMatrix.setScale(this->getLocalScale());
	Vector3D rotation = this->getLocalRotation();
	Matrix4x4 xMatrix; xMatrix.setRotationX(rotation.getX());
	Matrix4x4 yMatrix; yMatrix.setRotationY(rotation.getY());
	Matrix4x4 zMatrix; zMatrix.setRotationZ(rotation.getZ());

	//Scale --> Rotate --> Transform as recommended order.
	Matrix4x4 rotMatrix; rotMatrix.setIdentity();
	rotMatrix = rotMatrix.multiplyTo(xMatrix.multiplyTo(yMatrix.multiplyTo(zMatrix)));

	allMatrix = allMatrix.multiplyTo(scaleMatrix.multiplyTo(rotMatrix));
	allMatrix = allMatrix.multiplyTo(translationMatrix);
	this->localMatrix = allMatrix;
}

void AGameObject::setLocalMatrix(float matrix[16])
{
	float matrix4x4[4][4];
	matrix4x4[0][0] = matrix[0];
	matrix4x4[0][1] = matrix[1];
	matrix4x4[0][2] = matrix[2];
	matrix4x4[0][3] = matrix[3];

	matrix4x4[1][0] = matrix[4];
	matrix4x4[1][1] = matrix[5];
	matrix4x4[1][2] = matrix[6];
	matrix4x4[1][3] = matrix[7];

	matrix4x4[2][0] = matrix[8];
	matrix4x4[2][1] = matrix[9];
	matrix4x4[2][2] = matrix[10];
	matrix4x4[2][3] = matrix[11];

	matrix4x4[3][0] = matrix[12];
	matrix4x4[3][1] = matrix[13];
	matrix4x4[3][2] = matrix[14];
	matrix4x4[3][3] = matrix[15];

	Matrix4x4 newMatrix; newMatrix.setMatrix(matrix4x4);
	Matrix4x4 scaleMatrix; scaleMatrix.setScale(this->localScale);
	Matrix4x4 transMatrix; transMatrix.setTranslation(this->localPosition);
	this->localMatrix = scaleMatrix.multiplyTo(transMatrix.multiplyTo(newMatrix));
	this->overrideMatrix = true;
}

float* AGameObject::getLocalMatrix()
{
	float* matrix4x4 = this->localMatrix.getMatrix();
	return matrix4x4;
}

float* AGameObject::getPhysicsLocalMatrix()
{
	Matrix4x4 allMatrix; allMatrix.setIdentity();
	Matrix4x4 translationMatrix; translationMatrix.setIdentity();  
	translationMatrix.setTranslation(this->getLocalPosition());
	Matrix4x4 scaleMatrix; scaleMatrix.setScale(Vector3D::ones()); //physics 3D only accepts uniform scale for rigidbody
	Vector3D rotation = this->getLocalRotation();
	Matrix4x4 xMatrix; xMatrix.setRotationX(rotation.getX());
	Matrix4x4 yMatrix; yMatrix.setRotationY(rotation.getY());
	Matrix4x4 zMatrix; zMatrix.setRotationZ(rotation.getZ());

	//Scale --> Rotate --> Transform as recommended order.
	Matrix4x4 rotMatrix; rotMatrix.setIdentity();
	rotMatrix = rotMatrix.multiplyTo(xMatrix.multiplyTo(yMatrix.multiplyTo(zMatrix)));

	allMatrix = allMatrix.multiplyTo(scaleMatrix.multiplyTo(rotMatrix));
	allMatrix = allMatrix.multiplyTo(translationMatrix);
	
	return allMatrix.getMatrix();
}
