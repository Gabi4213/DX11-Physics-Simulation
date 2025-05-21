#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "ParticleModel.h"
#include "Appearance.h"
#include <string>

using namespace DirectX;

class Particle
{
public:
	Particle(Transform* transform, ParticleModel* particleModel, Appearance* appearance, int particleType);
	Particle(Transform* transform, ParticleModel* particleModel);
	~Particle();

	Transform* GetTransform() const { return _transform; }
	ParticleModel* GetParticleModel() const { return _particleModel; }
	Appearance* GetAppearance() const { return _appearance; }

	void SetLifetime(int lifetime) { _lifetime = lifetime; }
	int GetLifetime() { return _lifetime; }

	void Update(float t);
	void Draw(ID3D11DeviceContext* pImmediateContext);

private:

	Transform* _transform;
	ParticleModel* _particleModel;
	Appearance* _appearance;

	int _particleType;
	bool _drop;

	int _lifetime;
};