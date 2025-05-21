#pragma once
#include <string>
#include "Appearance.h"
#include "Transform.h"
#include "ParticleModel.h"
#include "RigidBody.h"

using namespace DirectX;
using namespace std;
class GameObject
{
public:
	GameObject(string type, Appearance* appearance, Transform* transform, ParticleModel* particleModel);
	~GameObject();

	string GetType() const { return _type; }

	Appearance* GetAppearance() const { return _appearance; }
	Transform* GetTransform() const { return _transform; }
	ParticleModel* GetParticleModel() const { return _particleModel; }

	RigidBody* GetRigidBody() { return _rigidBody; }

	void SetCentreOfMass(Vector3D point) { _centre = point; }
	Vector3D GetCentre() { return _centre; }

	void Update(float t);
	void Draw(ID3D11DeviceContext* pImmediateContext);
	XMMATRIX mRotation;

	void SetActive(bool active) { _active = active; }
	bool GetActive() { return _active; }

private:

	Vector3D _centre;
	string _type;
	Appearance* _appearance;
	Transform* _transform;
	ParticleModel* _particleModel;
	RigidBody* _rigidBody;
	bool _active;
};

