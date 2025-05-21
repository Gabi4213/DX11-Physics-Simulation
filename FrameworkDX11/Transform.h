#pragma once
#include <directxmath.h>
#include "Vector3D.h"
#include "Quaternion.h"

using namespace DirectX;

class Transform
{
public:
	Transform();
	~Transform();

	Vector3D GetPosition() const { return _position; }
	void SetPosition(Vector3D position) { _position = position; }
	void SetPosition(float x, float y, float z) { SetPosition(Vector3D(x, y, z)); }
	XMFLOAT3 GetScale() const { return _scale; }

	void SetScale(XMFLOAT3 scale) { _scale = scale; }
	void SetScale(float x, float y, float z) { _scale.x = x; _scale.y = y; _scale.z = z; }

	Quaternion GetRotation() const { return _rotation; }
	void SetRotation(Vector3D rotation);

	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4(&_world); }

	void SetParent(Transform* parent) { _parent = parent; }

	void Rotate(Vector3D rot, float scale);

	void Update(float t);

private:

	Vector3D _position;
	Quaternion _rotation;
	XMFLOAT3 _scale;

	XMFLOAT4X4 _world;
	Transform* _parent;
};

