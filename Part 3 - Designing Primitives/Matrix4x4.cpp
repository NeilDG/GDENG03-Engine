#include "Matrix4x4.h"


Matrix4x4::Matrix4x4()
{
}

Matrix4x4::~Matrix4x4()
{
}

void Matrix4x4::setIdentity()
{
	//set array as zeros
	this->matrixInitialize();
	this->matrix[0][0] = 1;
	this->matrix[1][1] = 1;
	this->matrix[2][2] = 1;
	this->matrix[3][3] = 1;
}

void Matrix4x4::setTranslation(const Vector3D translation)
{
	this->setIdentity();
	Vector3D::Vect vect = translation.getValues();
	this->matrix[3][0] = vect.x;
	this->matrix[3][1] = vect.y;
	this->matrix[3][2] = vect.z;
}

void Matrix4x4::setScale(const Vector3D scale)
{
	this->setIdentity();
	Vector3D::Vect vect = scale.getValues();
	this->matrix[0][0] = vect.x;
	this->matrix[1][1] = vect.y;
	this->matrix[2][2] = vect.z;
}

void Matrix4x4::setRotationX(float x)
{
	this->setIdentity();
	this->matrix[1][1] = cos(x);
	this->matrix[1][2] = sin(x);
	this->matrix[2][1] = -sin(x);
	this->matrix[2][2] = cos(x);
}

void Matrix4x4::setRotationY(float y)
{
	this->setIdentity();
	this->matrix[0][0] = cos(y);
	this->matrix[0][2] = -sin(y);
	this->matrix[2][0] = sin(y);
	this->matrix[2][2] = cos(y);
}


void Matrix4x4::setRotationZ(float z)
{
	this->setIdentity();
	this->matrix[0][0] = cos(z);
	this->matrix[0][1] = sin(z);
	this->matrix[1][0] = -sin(z);
	this->matrix[1][1] = cos(z);
}

void Matrix4x4::setOrthoLH(float width, float height, float near_plane, float far_plane)
{
	this->setIdentity();
	this->matrix[0][0] = 2.0f / width;
	this->matrix[1][1] = 2.0f / height;
	this->matrix[2][2] = 1.0f / (far_plane - near_plane);
	this->matrix[3][2] = -(near_plane / (far_plane - near_plane));
}

Matrix4x4 Matrix4x4::multiplyTo(Matrix4x4 matrix)
{
	Matrix4x4 out;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.matrix[i][j] =
				this->matrix[i][0] * matrix.matrix[0][j] + this->matrix[i][1] * matrix.matrix[1][j] +
				this->matrix[i][2] * matrix.matrix[2][j] + this->matrix[i][3] * matrix.matrix[3][j];
		}
	}

	return out;
}

void Matrix4x4::debugPrint()
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << this->matrix[i][j] << " ";
		}
		std::cout << "\n";
	}
}

void Matrix4x4::matrixInitialize()
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			this->matrix[i][j] = 0.0f;
		}
	}
}
