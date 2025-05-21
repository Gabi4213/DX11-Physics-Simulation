#include "Vector3D.h"

//------Constructors------
Vector3D::Vector3D()
{
	x = 0, y = 0, z = 0;
}

Vector3D::Vector3D(float xl, float yl, float zl): x(xl), y (yl), z(zl)
{
	
}

Vector3D::Vector3D(const Vector3D&vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

//------Addition------
Vector3D Vector3D::operator+(const Vector3D&vec)
{
	return Vector3D(x + vec.x, y + vec.y, z + vec.z);
}

Vector3D& Vector3D::operator+=(const Vector3D&vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

//------Subtraction------
Vector3D Vector3D::operator-(const Vector3D&vec)
{
	return Vector3D(x - vec.x, y - vec.y, z - vec.z);
}

Vector3D& Vector3D::operator-=(const Vector3D& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

//------Multiplication------
Vector3D Vector3D::operator*(float value)
{
	return Vector3D(x * value, y * value, z * value);
}

Vector3D& Vector3D::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

//------Division------
Vector3D Vector3D::operator/(float value)
{
	return Vector3D(x / value, y / value, z / value);
}

Vector3D& Vector3D::operator/=(float value)
{
	x /= value;
	y /= value;
	z /= value;
	return *this;
}

//------Other------
Vector3D& Vector3D::operator=(const Vector3D&vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	return *this;
}

//------Dot Product------
float Vector3D::dot_product(const Vector3D&vec)
{
	return (x * vec.x + y * vec.y + z * vec.z);
}



//------Cross Product------
Vector3D Vector3D::cross_product(const Vector3D& vec)
{
	float ni = x * vec.y - y * vec.x;
	float nj = y * vec.z - z * vec.y;
	float nk = z * vec.x - x * vec.z;
	return Vector3D(x, y, z);
}

//------Other------
float Vector3D::magnitude()
{
	return sqrt(x * x + y * y + z * z);
}

float Vector3D::square()
{
	return x * x + y * y + z * z;
}

Vector3D Vector3D::normalization()
{
	Vector3D _returnVector;

	_returnVector.x = x / magnitude();
	_returnVector.y = y / magnitude();
	_returnVector.z = z / magnitude();

	if (_returnVector.x == 0) { _returnVector.x = 0; }
	if (_returnVector.y == 0) { _returnVector.y = 0; }
	if (_returnVector.z == 0) { _returnVector.z = 0; }

	return _returnVector;
}

float Vector3D::distance(const Vector3D&vec)
{
	float X = x - vec.x;
	float Y = y - vec.y;
	float Z = z - vec.z;
	return sqrt((X * X) + (Y * Y) + (Z * Z));
}

//------Show Vector Values------
float Vector3D::show_X()
{
	return x;
}

float Vector3D::show_Y()
{
	return y;
}

float Vector3D::show_Z()
{
	return z;
}

void Vector3D::disp()
{
	cout << x << " " << y << " " << z << endl;
}

