#pragma once
#include "Vector3D.h"

struct BoundingSphere
{	
	float _radius;
	Vector3D _centre;
	Vector3D _contactNormal;
	BoundingSphere(float radius, Vector3D centre) { _radius = radius;  _centre = centre; }
	BoundingSphere() = default;

	void SetRadius(float radius) { _radius = radius; }
	float GetRadius() { return _radius; }
};

struct BoundingBox
{
	Vector3D _centre;
	Vector3D _size;
	Vector3D _contactNormal;
	
	BoundingBox(Vector3D centre, Vector3D size) { _centre = centre;  _size = size; }
	BoundingBox() = default;
	Vector3D ClosestPoint(BoundingBox box, Vector3D point) 
	{  
		Vector3D boxMin = box.GetMin();
		Vector3D boxMax = box.GetMax();


		point.x = (point.x < boxMin.x) ? boxMin.x: point.x; // 1st if true, 2nd if false - tenerary operator
		point.y = (point.y < boxMin.y) ? boxMin.y: point.y;
		point.z = (point.z < boxMin.z) ? boxMin.z: point.z;

		point.x = (point.x < boxMax.x) ? boxMax.x : point.x; // 1st if true, 2nd if false - tenerary operator
		point.y = (point.y < boxMax.y) ? boxMax.y : point.y;
		point.z = (point.z < boxMax.z) ? boxMax.z : point.z;

		return point;
	};

	Vector3D GetMax() 
	{
		Vector3D point1 = _centre + _size; // right side cube - for defining maximum points
		Vector3D point2 = _centre - _size; // left side cube - for defining maximum points

		return Vector3D(fmaxf(point1.x, point2.x), fmaxf(point1.y, point2.y), fmaxf(point1.z, point2.z));
	};

	Vector3D GetMin()
	{
		Vector3D point1 = _centre + _size; // right side cube - for defining minimum points
		Vector3D point2 = _centre - _size; // left side cube - for defining minimum points

		return Vector3D(fminf(point1.x, point2.x), fminf(point1.y, point2.y), fminf(point1.z, point2.z)); // return smallest point out of two given values
	};

};





