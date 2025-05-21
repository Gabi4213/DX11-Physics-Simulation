#pragma once
#include "ParticleModel.h"
#include "Quaternion.h"
#include "Vector3D.h"
#include "Collision.h"


class RigidBody: public ParticleModel
{
public:
    RigidBody(Transform* transform, bool useConstVel, Vector3D initialVelocty, XMFLOAT3 initialAcceleration);
    ~RigidBody();

    void CalulateAngularAcceleration();
    void CalulateAngularVelocity(float deltaTime);
    void CalculateTorque(Vector3D Force, Vector3D point);
    float AddDampAngularVelocity(float deltaTime);

    void Update(float deltaTime);

    Vector3D GetTorque() const { return _torque; }

    void AddRotationForce(Vector3D force, Vector3D point, float deltaTime);

    XMFLOAT3X3 CalculateInertiaTensor(float w, float h, float d);

    void SetRotForce(Vector3D rotForce) { _RotForce = rotForce; }
    void SetPointHit(Vector3D point) { _PointHit = point; }

public:

    Vector3D _angularVel;
private:

    BoundingSphere _sphere;
    BoundingBox _box;

    Vector3D _torque;
    XMFLOAT3X3 _interiaTensor;

    Vector3D _angularAcc;
    
    
    Quaternion _orientationQuaternion;
    XMMATRIX _finalOrientation;

    Vector3D _RotForce;
    Vector3D _PointHit;

    float _angularDamping = 0.8f;
};

