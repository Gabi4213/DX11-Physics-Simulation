#include "Camera.h"

Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, int windowWidth, int windowHeight, int nearDepth, int farDepth)
	: _eye(position), _at(at), _up(up), _windowWidth(windowWidth), _windowHeight(windowHeight), _nearDepth(nearDepth), _farDepth(farDepth)
{
	_world = new XMFLOAT4X4();

	float _multiplier = 10000.0f;
	_speed = 1.0f / _multiplier;
	_rotationSpeed = 1.0f / _multiplier;


	_forwardVector = XMFLOAT3(0.0f, 0.0f, 1.0f);

	// Set View Matrix
	XMStoreFloat4x4(&_view, XMMatrixLookToLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_forwardVector), XMLoadFloat3(&_up)));

	// Set Projection Matrix
	XMMATRIX TempMat;
	float t_Aspect = _windowWidth / _windowHeight;
	TempMat = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), t_Aspect, 0.01, 100.0f);
	XMStoreFloat4x4(&_projection, TempMat);
}

Camera::~Camera()
{
	if (_world != nullptr)
	{
		delete _world;
		_world = nullptr;
	}
}

void Camera::Update(const float deltaTime)
{
	XMMATRIX t_TempWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_view));

	//wasd
	if (GetAsyncKeyState(VK_UP)) { t_TempWorld.r[3] += t_TempWorld.r[2] * _speed * deltaTime; }
	if (GetAsyncKeyState(VK_DOWN)) { t_TempWorld.r[3] -= t_TempWorld.r[2] * _speed * deltaTime; }
	if (GetAsyncKeyState(VK_LEFT)) { t_TempWorld.r[3] -= t_TempWorld.r[0] * _speed * deltaTime; }
	if (GetAsyncKeyState(VK_RIGHT)) { t_TempWorld.r[3] += t_TempWorld.r[0] * _speed * deltaTime; }
																		  
	//up adn down									  
	if (GetAsyncKeyState(VK_SPACE)) { t_TempWorld.r[3] += t_TempWorld.r[1] * _speed * deltaTime; }
	if (GetAsyncKeyState(VK_SHIFT)) { t_TempWorld.r[3] -= t_TempWorld.r[1] * _speed * deltaTime; }

	////rotations
	if (GetAsyncKeyState(0x51)) // Rotate Left
	{
		XMMATRIX CamRotation = XMMatrixRotationAxis(t_TempWorld.r[1], XMConvertToRadians(-1 * (_rotationSpeed * 10) * deltaTime));
		t_TempWorld.r[0] = XMVector3TransformNormal(t_TempWorld.r[0], CamRotation);
		t_TempWorld.r[2] = XMVector3TransformNormal(t_TempWorld.r[2], CamRotation);
	}
	if (GetAsyncKeyState(0x45)) // Rotate Right
	{
		XMMATRIX CamRotation = XMMatrixRotationAxis(t_TempWorld.r[1], XMConvertToRadians(1 * (_rotationSpeed * 10) * deltaTime));
		t_TempWorld.r[0] = XMVector3TransformNormal(t_TempWorld.r[0], CamRotation);
		t_TempWorld.r[2] = XMVector3TransformNormal(t_TempWorld.r[2], CamRotation);
	}

	XMStoreFloat4x4(_world, t_TempWorld);
	XMMATRIX t_TempView = XMMatrixInverse(nullptr, XMLoadFloat4x4(_world));
	XMStoreFloat4x4(&_view, t_TempView);
}

void Camera::SetCameraPosition(float x, float y, float z)
{
	XMMATRIX TempWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_view));

	TempWorld.r[3] += TempWorld.r[0] * x;
	TempWorld.r[3] += TempWorld.r[1] * y;
	TempWorld.r[3] += TempWorld.r[2] * z;

	XMStoreFloat4x4(_world, TempWorld);
	XMMATRIX TempView = XMMatrixInverse(nullptr, XMLoadFloat4x4(_world));
	XMStoreFloat4x4(&_view, TempView);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_at), XMLoadFloat3(&_up))); // Set AT needs this for the view matrix
}

void Camera::Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}

XMMATRIX& Camera::GetCameraMatrix()
{
	XMMATRIX t_TempMat = XMLoadFloat4x4(_world);
	return t_TempMat;
}