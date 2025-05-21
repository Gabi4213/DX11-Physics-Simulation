#pragma once
#include <directxmath.h>
#include <d3d11.h>
#include <vector>
#include "Transform.h"
#include "Structures.h"
#include <DirectXMath.h>
#include <algorithm>
#include <string>
#include "Vector3D.h"
#include "Collision.h"

using namespace std;

class ParticleModel
{
public:
	ParticleModel(Transform* transform, bool useConstVel, Vector3D initialVelocty, XMFLOAT3 initialAcceleration, float mass);

	
	virtual ~ParticleModel();
	void Update(float deltaTime);

	void MoveConstVelocity(float deltaTime);
	Vector3D GetVelocity() { return _velocity; }
	void SetVelocity(Vector3D velocityNew) { _velocity = velocityNew; }
	void setVel(float x, float y, float z) { _velocity = { x, y, z }; }

	void AddForce(Vector3D force);

	void MoveConstantAcceleration(float deltaTime);
	XMFLOAT3 GetAcceleration() const { return _acceleration; }
	void SetAcceleration(XMFLOAT3 acceleration) { _acceleration = acceleration; }

	void SetMoveBool(bool moveObject) { _useConstVel = moveObject; }
	bool GetMoveBool() { return _useConstVel; }

	Vector3D GetNetForce() { return _netForce; }
	void SetNetForce(Vector3D _newNetForce) { _netForce = _newNetForce; }

	float GetMass() const { return _mass; }
	void SetMass(float mass) { _mass = mass; }

	void UpdateNetForce();
	void UpdateState();
	void UpdateAccel();
	void Move(float deltaTime);


	bool GetThrustEnabled() { return _thrustEnabled; }
	void SetThrustEnabled(bool isEnabled) { _thrustEnabled = isEnabled; }
	void SetThrust(float x, float y, float z) { _thrustForce = { x, y, z }; }
	XMFLOAT3 GetThrust() { return _thrustForce; }
	void UpdateVertThrust();


	void MotionInFluid(float deltaTime);
	void DragForce();
	void DragLamFlow();
	void DragTurbFlow();

	void UpdateParticle(float deltaTime, Vector3D vel);

	Vector3D GetVector3DPosition()const { return _vector3DPosition; }

	bool GetCollidingBool() { return _colliding; }
	void SetCollidingBool(bool colliding) { _colliding = colliding; }

	void SetGrav(bool grav) { _useGrav = grav; }
	bool GetGrav() { return _useGrav; }

	BoundingSphere GetSphere() { return _sphere; }
	void SetSphere(BoundingSphere sphere) { _sphere = sphere; }
	void SetSphereVal(Vector3D value);
	Vector3D GetSphereContact() { return _sphere._contactNormal; }



	BoundingBox GetBox() { return _box; }
	void SetBox(BoundingBox box) { _box = box; }
	void SetBoxVal(Vector3D value);
	Vector3D GetBoxContact() { return _box._contactNormal; }

	void ApplyImpulse(Vector3D point, Vector3D force);


	void CalculateBuoyancy(float Mass, float GravOfLiquid, float Radius, float time);
	void SetBuoyancy(bool useBioyancy) { _useBioyancy = useBioyancy; }
	bool GetBuoyancy() { return _useBioyancy;}
	void FloatingSimulation();


protected:
	Transform* _transform;
	
	Vector3D _velocity;
	XMFLOAT3 _acceleration;

	bool _useConstVel;

	Vector3D _friction;

	Vector3D _netForce;
	float _mass;
	Vector3D _gravity;
	vector<XMFLOAT3> forces;

	bool _thrustEnabled;
	XMFLOAT3 _thrustForce;

	Vector3D _weight;
	float _upforce;

	bool _laminar;

	XMFLOAT3 _drag;
	float _dragFactor;

	Vector3D _startingPosition;

	Vector3D _vector3DPosition;

	//Colisions
	float _colRadius;
	bool _colliding;

	BoundingSphere _sphere;
	BoundingBox _box;

	bool _useGrav;
	bool _useBioyancy;
	bool GoUp = true;

};


