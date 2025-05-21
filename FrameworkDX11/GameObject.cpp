#include "GameObject.h"

GameObject::GameObject(string type, Appearance* appearance, Transform* transform, ParticleModel* particleModel) : _appearance(appearance), _type(type), _transform(transform), _particleModel(particleModel)
{
	_type = type;
	_appearance = appearance;
	_transform = transform;
	_particleModel = particleModel;
	_rigidBody = new RigidBody(_transform, _particleModel->GetMoveBool(), Vector3D(0, 0, 0), XMFLOAT3(0,0,0));
	_centre = Vector3D(0,0,0);
	_active = true;
}

GameObject::~GameObject()
{
}

void GameObject::Update(float t)
{
	//Update
	_transform->Update(t);
	_particleModel->Update(t);
	_rigidBody->Update(t);

	//set colliders
	_particleModel->SetSphereVal(_transform->GetPosition());
	_particleModel->SetBoxVal(_transform->GetPosition());

}


void GameObject::Draw(ID3D11DeviceContext* pImmediateContext)
{
	if (_active) 
	{
		_appearance->Draw(pImmediateContext);
	}
}
