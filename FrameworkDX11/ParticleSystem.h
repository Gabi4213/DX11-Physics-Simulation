#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "Particle.h"
#include <vector>
#include "GameObject.h"

using namespace DirectX;
using namespace std;

class ParticleSystem
{
public:
	ParticleSystem(Transform* transform, Vector3D pos, Appearance* appearance, int particleType);
	~ParticleSystem();

	Transform* GetTransform() { return _transform; }
	Appearance* GetAppearance() { return _appearance; }
	vector<Particle*> GetParticles() { return particles; }

	void CreateParticles(float deltaTime);
	void DeleteParticles();


	bool GetParticlesActive() { return _particlesActive; };
	void SetParticlesActive(bool particlesActive) {_particlesActive = particlesActive; }

	void Update(float t);
	void Draw(ID3D11DeviceContext* pImmediateContext);

private:

	vector<Particle*> particles;

	Transform* _transform;
	Vector3D _pos;
	Appearance* _appearance;

	int _particleType;
	bool _particlesActive;

};
