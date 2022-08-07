#include "AGameObject.h"
#include "EditorAction.h"

static const float ZPI = 3.14159265358979323846f;
static const float RAD2DEG = (180.f / ZPI);
static const float DEG2RAD = (ZPI / 180.f);

AGameObject::AGameObject(String name, PrimitiveType type)
{
	this->name = name;
	this->objectType = type;
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

AGameObject::PrimitiveType AGameObject::getObjectType()
{
	return this->objectType;
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
}

void AGameObject::setPosition(Vector3D pos)
{
	this->localPosition = pos;
}

Vector3D AGameObject::getLocalPosition()
{
	return this->localPosition;
}

void AGameObject::setScale(float x, float y, float z)
{
	this->localScale = Vector3D(x, y, z);
}

void AGameObject::setScale(Vector3D scale)
{
	this->localScale = scale;
}

void AGameObject::setRotationDegrees(float x, float y, float z)
{
	
	this->localRotation = Vector3D(x * DEG2RAD, y * DEG2RAD, z * DEG2RAD);
}

Vector3D AGameObject::getLocalRotationDegrees()
{
	Vector3D rotDeg(this->localRotation.getX() * RAD2DEG, this->localRotation.getY() * RAD2DEG, this->localRotation.getZ() * RAD2DEG);
	return rotDeg;
}

Vector3D AGameObject::getLocalRotationRaw()
{
	return this->localRotation;
}

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
	Vector3D rotation = this->getLocalRotationRaw();
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

void AGameObject::setPhysicsMatrix(const float matrix[16])
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
}

void AGameObject::setNewMatrix(const float matrix[16])
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
	
	this->localMatrix.setMatrix(matrix4x4);
}

bool AGameObject::testObjectSelect(Vector3D point)
{
	std::cout << "No implementation of testObjectSelect()" << std::endl;
	return false;
}

float* AGameObject::getRawMatrix()
{
	float* matrix4x4 = this->localMatrix.getMatrix();
	return matrix4x4;
}

Matrix4x4 AGameObject::getLocalMatrix()
{
	return this->localMatrix;
}

float* AGameObject::getPhysicsLocalMatrix()
{
	Matrix4x4 allMatrix; allMatrix.setIdentity();
	Matrix4x4 translationMatrix; translationMatrix.setIdentity();  
	translationMatrix.setTranslation(this->getLocalPosition());
	Matrix4x4 scaleMatrix; scaleMatrix.setScale(Vector3D::ones()); //physics 3D only accepts uniform scale for rigidbody
	Vector3D rotation = this->getLocalRotationRaw();
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

void AGameObject::saveEditState()
{
	if (this->lastEditState == NULL) {
		this->lastEditState = new EditorAction(this);
	}
}

void AGameObject::restoreEditState()
{
	if (this->lastEditState != NULL) {
		this->localPosition = this->lastEditState->getStorePos();
		this->localScale = this->lastEditState->getStoredScale();
		this->localRotation = this->lastEditState->getStoredOrientation();
		this->localMatrix = this->lastEditState->getStoredMatrix();

		this->lastEditState = NULL;
	}
	else {
		std::cout << "Edit state is null. Cannot restore. \n";
	}
}

void AGameObject::awake()
{
}
