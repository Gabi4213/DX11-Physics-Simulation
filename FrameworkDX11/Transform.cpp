#include "Transform.h"
Transform::Transform()
{
	//Set Variables
	_parent = nullptr;
	_position = Vector3D(0.0f, 0.0f, 0.0f);
	_rotation = Quaternion();
	_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

Transform::~Transform()
{
}

void Transform::Rotate(Vector3D rot, float scale)
{
	_rotation.normalise(); //normalise
	_rotation.addScaledVector(rot, scale); // ad scalar vector
}

void Transform::SetRotation(Vector3D rotation)
{
	//set rotation
	XMMATRIX rotMatrix = XMMatrixRotationX(rotation.x) * XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z);
	XMVECTOR swap = XMQuaternionRotationMatrix(rotMatrix);
	XMFLOAT4 placeHolderVector;
	XMStoreFloat4(&placeHolderVector, swap);
	_rotation = Quaternion(placeHolderVector.w, placeHolderVector.x, placeHolderVector.y, placeHolderVector.z);
}



void Transform::Update(float t)
{
	// Calculate world matrix
	XMMATRIX scale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);

	_rotation.normalise();
	XMMATRIX orientationMatrix;
	CalculateTransformMatrixRowMajor(orientationMatrix, _position, _rotation);
	XMMATRIX rotation = orientationMatrix;


	//XMMATRIX rotation = MatrixToQuarternion(_rotation);
	XMMATRIX translation = XMMatrixTranslation(_position.x, _position.y, _position.z);

	XMStoreFloat4x4(&_world, scale * rotation * translation);

	if (_parent != nullptr)
	{
		XMStoreFloat4x4(&_world, this->GetWorldMatrix() * _parent->GetWorldMatrix());
	}
}