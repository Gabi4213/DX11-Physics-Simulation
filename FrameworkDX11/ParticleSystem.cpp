#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(Transform* transform, Vector3D pos, Appearance* appearance, int particleType) : _transform(transform), _pos(pos), _appearance(appearance), _particleType(particleType)
{
	_transform->SetPosition(_transform->GetPosition().x + pos.x,_transform->GetPosition().y + pos.y,_transform->GetPosition().z + pos.z); // set initial pos
	_particlesActive = false; // set particles to false
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(float dt)
{
	if (_particlesActive)  // if true
	{
		_transform->Update(dt); // udpate

		if (particles.size() != 100) // if size is not at max (100) creat emore
		{
			CreateParticles(dt);
		}

		for (int i = 0; i < particles.size(); i++) // loop thoguh particles
		{
			particles.at(i)->Update(dt); // update

			if (particles.at(i)->GetLifetime() == 0) // if the lifetime is at 0 delete
			{
				particles.erase(particles.begin() + i); //start removing
			}
		}
	}
	else
	{
		if (particles.size() > 0) { DeleteParticles(); }
	}
}

void ParticleSystem::CreateParticles(float deltaTime)
{
	if (_particleType == 0) // if rain
	{
		Transform* transform = new Transform();
		Vector3D randomPos = { -4.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (4.0 - -4.0)),5.0f,-4.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (4.0 - -4.0)) };
		transform->SetPosition(randomPos);
		transform->SetScale(0.01f, 0.2f, 0.01f);
		transform->SetRotation(Vector3D(XMConvertToRadians(0.0f), 0.0f, 0.0f));
		ParticleModel* pm = new ParticleModel(transform, true, Vector3D(0, 0, 0), XMFLOAT3(0, 0, 0), 2.5f);
		particles.push_back(new Particle(transform, pm, _appearance, _particleType));
	}
	else //if water droplets
	{
		Transform* transform = new Transform();
		Vector3D randomPos = { -1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0 - -0.5)),0.0f,-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0 - -0.5)) };
		transform->SetPosition(randomPos);
		transform->SetScale(0.1f, 0.1f, 0.01f);
		transform->SetRotation(Vector3D(XMConvertToRadians(0.0f), 0.0f, 0.0f));
		ParticleModel* pm = new ParticleModel(transform, false, Vector3D{ 0.0f,0.0f,0.0f }, XMFLOAT3{ 0.0f,0.0f,0.0f },2.5f);
		particles.push_back(new Particle(transform, pm, _appearance, _particleType));
	}
}

void ParticleSystem::DeleteParticles() //delete
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles.erase(particles.begin() + i);	
	}
}


void ParticleSystem::Draw(ID3D11DeviceContext* pImmediateContext)
{
	for (auto particle : particles)
	{
		particle->Draw(pImmediateContext);
	}
}