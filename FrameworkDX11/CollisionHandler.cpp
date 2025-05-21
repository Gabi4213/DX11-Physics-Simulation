#include "CollisionHandler.h"

CollisionHandler::CollisionHandler()
{

}
CollisionHandler::~CollisionHandler() 
{

}



bool CollisionHandler::CheckCollision(BoundingSphere sphere1, BoundingSphere sphere2) // Sphere vs Sphere
{
	Vector3D distance;
	distance = sphere1._centre - sphere2._centre; // get distance

	float radii = sphere1._radius * sphere2._radius; // get radius

	float distanceDot = distance.dot_product(distance); // calcualte dot matrix

	if (distanceDot < radii) // chekc if collision true or not
	{
		return true;
	}
	return false;
}

bool CollisionHandler::CheckCollision(BoundingBox box1, BoundingBox box2) // AABB and AABB
{
	// get min and max values
	Vector3D box1Min = box1.GetMin();
	Vector3D box1Max = box1.GetMax();

	Vector3D box2Min = box2.GetMin();
	Vector3D box2Max = box2.GetMax();

	//Check for overlap
	return (box1Min.x <= box2Max.x && box1Max.x >= box2Min.x) &&
		(box1Min.y <= box2Max.y && box1Max.y >= box2Min.y) &&
		(box1Min.z <= box2Max.z && box1Max.z >= box2Min.z);
}

bool CollisionHandler::CheckCollision(BoundingSphere sphere1, BoundingBox box1) // sphere box
{
	Vector3D point = box1.ClosestPoint(box1, sphere1._centre); //find closest point

	float disatnceSqr = sphere1._centre.dot_product(sphere1._centre - point - Vector3D(0.5f,0,0)); //find distance square root

	float radiusSqr = sphere1._radius * sphere1._radius; //find radius square root

	//calcualate contact normals
	box1._contactNormal = point - sphere1._centre;
	box1._contactNormal.normalization();

	sphere1._contactNormal = point - box1._centre;
	sphere1._contactNormal.normalization();

	if (disatnceSqr < radiusSqr) // chekc if collision true or false
	{
		return true;
	}
	return false;

}


void CollisionHandler::CollisionResponseMoveVel(Vector3D vel, GameObject* obj1, GameObject* obj2, float deltaTime) // velocity mass based
{
	// apply collison repsonse in terms of velocity - moves the object at the same speed based on its speed and mass
	Vector3D A;

	float massA = obj1->GetParticleModel()->GetMass();
	float massB = obj2->GetParticleModel()->GetMass();

	A = ((A * massA) + (vel * massB) + ((vel - A) * (massB * 0.5)) / (massA + massB));

	obj2->GetParticleModel()->SetVelocity(A);
	obj2->GetParticleModel()->MoveConstVelocity(deltaTime);
}

void CollisionHandler::CollisionResponseSphereCube(Vector3D vel, GameObject* obj1, GameObject* obj2, float deltaTime)
{
	// Some force hiting it 
	Vector3D force;

	float massA = obj1->GetParticleModel()->GetMass();
	float massB = obj2->GetParticleModel()->GetMass();

	force = ((force * massA) + (vel * massB) + Vector3D(-1.0f,0.0f,0.0f));

	obj1->GetParticleModel()->ApplyImpulse(vel, force);
	obj1->GetParticleModel()->MoveConstVelocity(deltaTime);

}

void CollisionHandler::CollisionResponseInterPenetration(float time, GameObject* obj1, GameObject* obj2)
{
	// calucalte depth penetration and midline - distance between btoh objects positions
	Vector3D midLine = obj1->GetTransform()->GetPosition() - (obj2->GetTransform()->GetPosition());
	float depthPenetration = obj1->GetParticleModel()->GetSphere()._radius + obj2->GetParticleModel()->GetSphere()._radius - midLine.magnitude();

	if (depthPenetration <= 0) { return; } // if below zero dont continue

	//get closest point
	Vector3D point = obj2->GetParticleModel()->GetBox().ClosestPoint(obj2->GetParticleModel()->GetBox(), obj1->GetParticleModel()->GetBox()._centre);

	// obj 1
	Vector3D contact1 = point - obj1->GetParticleModel()->GetBox()._centre;
	contact1.normalization();

	//obj 2
	Vector3D contact2 = point - obj2->GetParticleModel()->GetBox()._centre;
	contact2.normalization();

	//total mass calculation
	float totalMass = obj1->GetParticleModel()->GetMass() + obj2->GetParticleModel()->GetMass();

	Vector3D movePerUnitMass = contact1 * (depthPenetration / totalMass); // movement per unit mass equation

	Vector3D bodyMovement[2];
	bodyMovement[0] = movePerUnitMass * obj1->GetParticleModel()->GetMass();
	bodyMovement[1] = movePerUnitMass * obj2->GetParticleModel()->GetMass();


	// apply separation
	obj1->GetTransform()->SetPosition(Vector3D(obj1->GetTransform()->GetPosition().x + -bodyMovement[0].x - 0.2f, obj1->GetTransform()->GetPosition().y, obj1->GetTransform()->GetPosition().z));
	obj2->GetTransform()->SetPosition(Vector3D(obj2->GetTransform()->GetPosition().x + bodyMovement[1].x + 0.2f, obj2->GetTransform()->GetPosition().y, obj2->GetTransform()->GetPosition().z));

} //inter penetration separation

void CollisionHandler::UpdateCollisionBoundries(GameObject* obj1, GameObject* obj2)
{
	//get collision boundries
	Vector3D pos1 = obj1->GetTransform()->GetPosition();
	Vector3D pos2 = obj2->GetTransform()->GetPosition();


	obj1->GetParticleModel()->SetSphereVal(pos1);
	obj1->GetParticleModel()->SetBoxVal(pos1);

	obj1->GetParticleModel()->GetBox()._centre;
	obj2->GetParticleModel()->GetBox()._centre;


	obj2->GetParticleModel()->SetSphereVal(pos2);
	obj2->GetParticleModel()->SetBoxVal(pos2);
}
