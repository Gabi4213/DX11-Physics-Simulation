#pragma once
#include "Collision.h"
#include "ParticleModel.h"
#include "GameObject.h"
using namespace std;


class CollisionHandler 
{
public:
	CollisionHandler();
	~CollisionHandler();

	bool CheckCollision(BoundingSphere sphere1, BoundingSphere sphere2);
	bool CheckCollision(BoundingBox box1, BoundingBox box2);
	bool CheckCollision(BoundingSphere sphere1, BoundingBox box1);

	void CollisionResponseMoveVel(Vector3D vel, GameObject* obj1, GameObject* obj2, float deltaTime);
	void CollisionResponseSphereCube(Vector3D vel, GameObject* obj1, GameObject* obj2, float deltaTime);

	void CollisionResponseInterPenetration(float time, GameObject* obj1, GameObject* obj2);
	void UpdateCollisionBoundries(GameObject* obj1, GameObject* obj2);
};