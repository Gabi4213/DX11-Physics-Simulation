#include "RigidBody.h"


RigidBody::RigidBody(Transform* transform, bool useConstVel, Vector3D initialVelocty, XMFLOAT3 initialAcceleration) :ParticleModel(transform, useConstVel, initialVelocty, initialAcceleration, 10)
{
	//Set Variables
	_interiaTensor = CalculateInertiaTensor(1,1,1);
	_angularVel = Vector3D{ 0,0,0 };
	_RotForce = Vector3D{ 0,0,0 };
	_PointHit = Vector3D{ 0,0,0 };
}

XMFLOAT3X3 RigidBody::CalculateInertiaTensor(float w, float h, float d)
{
	//calculate the inreria tensor
	XMFLOAT3X3 matrix = XMFLOAT3X3();

	matrix._11 = 0.0833f * GetMass() * (h * h + d * d);
	matrix._12 = 0.0833f * GetMass() * (w * w + d * d);
	matrix._13 = 0.0833f * GetMass() * (w * w + h * h);

	return matrix;
}

void RigidBody::Update(float deltaTime)
{
	CalulateAngularAcceleration();
	CalulateAngularVelocity(deltaTime);
	_torque = Vector3D(0,0,0);
}

RigidBody::~RigidBody()
{


}

void RigidBody::AddRotationForce(Vector3D force, Vector3D point, float deltaTime)
{
	_angularVel += force * deltaTime; // calculate angualar velocity
}

void RigidBody::CalculateTorque(Vector3D Force, Vector3D point)
{
	_torque = (Vector3D(0,0,0) + point).cross_product(Force); // calculate torque
}

void RigidBody::CalulateAngularAcceleration()
{
	// Calcualte angual acc
	XMFLOAT3 acceleration;
	XMVECTOR torqueVector = XMVectorSet(_torque.x, _torque.y, _torque.z, 0.0f);
	XMMATRIX intertiaTensor = XMLoadFloat3x3(&_interiaTensor);
	XMVECTOR inverse;
	XMMatrixInverse(&inverse, intertiaTensor); // inverse

	XMVECTOR result = torqueVector * inverse;

	XMStoreFloat3(&acceleration, result);
	_angularAcc = Vector3D(acceleration.x, acceleration.y, acceleration.z);
}

void RigidBody::CalulateAngularVelocity(float deltaTime)
{
	_angularVel += _angularAcc * deltaTime; //calcualte
	float damping = AddDampAngularVelocity(deltaTime);
	_angularVel *= damping; //dampen
	_transform->Rotate(_angularVel,deltaTime); // apply rotation
}

float RigidBody::AddDampAngularVelocity(float deltaTime)
{
	return pow(_angularDamping, deltaTime);
}
