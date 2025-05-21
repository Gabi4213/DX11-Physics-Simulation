#pragma once
#include<iostream>
#include<math.h>
#include<assert.h>
using namespace std;

class Vector3D
{
public:
	float x, y, z;

//------Constructors & Destructor------
	Vector3D();
	Vector3D(float xl, float yl, float zl);
	Vector3D(const Vector3D&vec);

//------Arithmetic Operations------

	//Addition & Subtraction
	Vector3D operator+(const Vector3D& vec);
	Vector3D&operator+=(const Vector3D& vec);
	Vector3D operator-(const Vector3D& vec);
	Vector3D&operator-=(const Vector3D& vec);

	//Multiplication & Division
	Vector3D operator*(float value);
	Vector3D&operator*=(float value);
	Vector3D operator/(float value);
	Vector3D&operator/=(float value);

	//Other
	Vector3D&operator=(const Vector3D& vec);

//------Vector Operations------
	float dot_product(const Vector3D& vec);

	Vector3D cross_product(const Vector3D&vec);
	Vector3D normalization();


//------Scalar Operations------
	float square();
	float distance(const Vector3D& vec);
	float magnitude();

//------Display operations------
	float show_X();
	float show_Y();
	float show_Z();
	void disp();
};

