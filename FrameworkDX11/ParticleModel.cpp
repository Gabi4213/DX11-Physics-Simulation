#include "ParticleModel.h"


ParticleModel::ParticleModel(Transform* transform, bool useConstVel, Vector3D initialVelocty, XMFLOAT3 initialAcceleration, float mass) : _transform(transform), _useConstVel(useConstVel), _velocity(initialVelocty), _acceleration(initialAcceleration), _mass(mass)
{
	_transform = transform;
	_velocity = initialVelocty;
	_acceleration = initialAcceleration;

	_mass = mass;
	_netForce = Vector3D(0.0f, 0.0f, 0.0f);

	_useConstVel = useConstVel;

	_weight.x = 0.0f;
	_weight.y = 0.0f;
	_weight.z = 0.0f;

	_thrustForce = { 0.0f, 0.0f, 0.0f };
	_gravity.y = -9.7f;

	_thrustEnabled = false;

	_laminar = true;
	_dragFactor = 1.0f;
	_startingPosition = _transform->GetPosition();

	_drag = { 0.0f, 0.0f, 0.0f };

	_upforce = 0.0f;

	_colRadius = 0.1f;

	_useGrav = true;
	_useBioyancy = false;

	_friction = Vector3D(1.0f, 1.0f, 1.0f);
}

ParticleModel::~ParticleModel()
{

}

void ParticleModel::Update(float deltaTime)
{
	if (_useBioyancy)
	{
		CalculateBuoyancy(_mass, 1000.0, _colRadius, deltaTime);
	}
	else
	{
		Vector3D pos = _transform->GetPosition();

		GetSphere()._centre = pos;
		SetSphereVal(pos);
		SetBoxVal(pos);

		UpdateState();

		//Makes the object drop when above its starting position -- bascially simulates gravity since I haven't made it 
		if (_transform->GetPosition().y > _startingPosition.y && _useGrav == true)
		{
			_thrustForce.y -= 0.0001f;
		}

		if (_transform->GetPosition().y < _startingPosition.y)
		{
			SetThrust(0.0f, 0.0f, 0);
			_transform->SetPosition(Vector3D(_transform->GetPosition().x,_startingPosition.y,_transform->GetPosition().z));
		}

		UpdateVertThrust();
		MotionInFluid(deltaTime);
	}
}

void ParticleModel::UpdateVertThrust()
{
	_transform->SetPosition(_transform->GetPosition().x, _transform->GetPosition().y + _netForce.y, _transform->GetPosition().z);
}

void ParticleModel::MoveConstVelocity(float deltaTime)
{
	Vector3D pos = _transform->GetPosition();

	pos.x += _velocity.x * deltaTime;
	pos.y += _velocity.y * deltaTime;
	pos.z += _velocity.z * deltaTime;


	_transform->SetPosition(pos);		
}

void ParticleModel::MoveConstantAcceleration(float deltaTime)
{
	Vector3D pos = _transform->GetPosition();

	pos.x += _velocity.x * deltaTime + 0.5f * _acceleration.x * deltaTime * deltaTime;
	pos.y += _velocity.y * deltaTime + 0.5f * _acceleration.y * deltaTime * deltaTime;
	pos.z += _velocity.z * deltaTime + 0.5f * _acceleration.z * deltaTime * deltaTime;

	_velocity.x += _acceleration.x * deltaTime;
	_velocity.y += _acceleration.y * deltaTime;
	_velocity.z += _acceleration.z * deltaTime;

	_transform->SetPosition(pos);
}

void ParticleModel::UpdateNetForce()
{
	// set thrust to netforce
	_netForce.x = _thrustForce.x;
	_netForce.y = _thrustForce.y;
	_netForce.z = _thrustForce.z;

	// Add gravity to net force if its avaliable and in this case above the starting position
	// becasue I dont want objects to fall beyond this point
	if (_useGrav && _transform->GetPosition().y > _startingPosition.y)
	{
		_netForce.x += _gravity.x;
		_netForce.y += _gravity.y;
		_netForce.z += _gravity.z;
	}

	//apply frction force to the netforce
	_netForce.x *= _friction.x;
	_netForce.y *= _friction.y;
	_netForce.z *= _friction.z;
}

void ParticleModel::UpdateState()
{
	UpdateNetForce();
	UpdateAccel();
}

void ParticleModel::UpdateAccel()
{
	_acceleration.x = _netForce.x / _mass;
	_acceleration.y = _netForce.y / _mass;
	_acceleration.z = _netForce.z / _mass;
}

void ParticleModel::Move(float deltaTime)
{
	Vector3D position = _transform->GetPosition();

	position.x = position.x + _velocity.x * deltaTime + 0.5f * _acceleration.x * deltaTime * deltaTime;
	position.y = position.y + _velocity.y * deltaTime + 0.5f * _acceleration.y * deltaTime * deltaTime;
	position.z = position.z + _velocity.z * deltaTime + 0.5f * _acceleration.z * deltaTime * deltaTime;

	_velocity.x = _velocity.x + _acceleration.x * deltaTime;
	_velocity.y = _velocity.y + _acceleration.y * deltaTime;

	_transform->SetPosition(position);
}

void ParticleModel::MotionInFluid(float deltaTime)
{
	DragForce();
}

void ParticleModel::DragForce()
{
	if (_laminar) { DragLamFlow(); }
	else { DragTurbFlow(); }
}

void ParticleModel::DragLamFlow()
{
	_drag.x = -_dragFactor * _velocity.x;
	_drag.y = -_dragFactor * _velocity.y;
	_drag.z = -_dragFactor * _velocity.z;
}

void ParticleModel::DragTurbFlow()
{
	float velMag = sqrt((_velocity.x * _velocity.x) + (_velocity.y * _velocity.y) + (_velocity.y * _velocity.y));

	XMFLOAT3 unitVel;
	unitVel.x = (_velocity.x / velMag);
	unitVel.y = (_velocity.y / velMag);
	unitVel.z = (_velocity.z / velMag);

	// Calculate magnitude of drag force
	float dragMag;
	dragMag = _dragFactor * velMag * velMag;

	// Calculate of x- and y-components of drag force
	_drag.x = -dragMag * unitVel.x;
	_drag.y = -dragMag * unitVel.y;

}

void ParticleModel::UpdateParticle(float deltaTime, Vector3D vel)
{
	_velocity = vel;
	UpdateState();	
	MoveConstVelocity(deltaTime);
}

void ParticleModel::AddForce(Vector3D force)
{
	_netForce += force;
}

void ParticleModel::SetSphereVal(Vector3D value) 
{
	_sphere._centre = value;
}

void ParticleModel::SetBoxVal(Vector3D value)
{
	_box._centre = value;
}

void ParticleModel::ApplyImpulse(Vector3D point, Vector3D force)
{
	_velocity = force;
}

void ParticleModel::CalculateBuoyancy(float Mass, float GravOfLiquid, float Radius, float time)
{
	float ObjectVolume = (4 * 3.14 * Radius * Radius * Radius) / 3;
	float buoyancyForce = ObjectVolume * GravOfLiquid * -_gravity.y;
	
	if (buoyancyForce >= Mass) //Float - make mass
	{
		FloatingSimulation();
	}
	else //Sink
	{
		_transform->SetPosition(_transform->GetPosition().x, _transform->GetPosition().y - 0.0001, _transform->GetPosition().z);
	}
}

void ParticleModel::FloatingSimulation() 
{
	float y = _transform->GetPosition().y;

	if (y > 0.0f && y < 0.4f && GoUp == true)
	{
		GoUp = true;
	}

	if (y >= 0.4f && GoUp == true)
	{
		GoUp = false;
	}

	if (y <= 0.0f && GoUp == false)
	{
		GoUp = true;
	}

	if (GoUp)
	{
		_transform->SetPosition(_transform->GetPosition().x, y + 0.0001, _transform->GetPosition().z);
	}
	else
	{
		_transform->SetPosition(_transform->GetPosition().x, y - 0.0001, _transform->GetPosition().z);
	}
}