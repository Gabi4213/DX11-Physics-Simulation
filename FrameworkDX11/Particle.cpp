#include "Particle.h"

Particle::Particle(Transform* transform, ParticleModel* particleModel, Appearance* appearance, int particleType) : _transform(transform), _particleModel(particleModel), _appearance(appearance), _particleType(particleType)
{
	_lifetime = rand() % 7000; // lifetime random
	_drop = false; // dont drop
}

Particle::~Particle()
{
}

void Particle::Update(float t)
{
	_transform->Update(t); // update

	_particleModel->UpdateParticle(t, Vector3D(0, 0, 0)); // update particle model

	switch(_particleType)
	{
		case 0: //rain			
			_particleModel->setVel(0.0f, -0.001, 0.0f);
			_particleModel->MoveConstVelocity(t);
		break;
		case 1: //water particle
			
			if (_transform->GetPosition().y >= 2.0f)  // set drop to false and reset acceleration
			{
				_drop = true;
				_particleModel->SetAcceleration(XMFLOAT3(0.0f, 0.0, 0.0f));
			}

			if (_drop) // go down
			{
				_drop = true;
				_particleModel->SetAcceleration(XMFLOAT3(0.0f, -0.00005, 0.0f));
				_particleModel->MoveConstantAcceleration(t);
			}
			else if (!_drop) // go up
			{
				_particleModel->SetAcceleration(XMFLOAT3(0.0f, 0.00005, 0.0f));
				_particleModel->MoveConstantAcceleration(t);
			}

			_transform->Rotate(Vector3D(0.0f, 0.0f, XMConvertToRadians(0.001f)),t); // apply rotation
			break;
	}
	_lifetime--; //r educe lifetime
}

void Particle::Draw(ID3D11DeviceContext* pImmediateContext)
{
	_appearance->Draw(pImmediateContext);
}