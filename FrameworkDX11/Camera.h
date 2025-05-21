#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

using namespace DirectX;

class Camera
{
private:

    XMFLOAT3 _eye;
    XMFLOAT3 _at;
    XMFLOAT3 _up;

    float _windowWidth;
    float _windowHeight;
    float _nearDepth;
    float _farDepth;
    float _speed;
    float _rotationSpeed;

    XMFLOAT3 _forwardVector;

    XMFLOAT4X4* _world;
    XMFLOAT4X4 _view;
    XMFLOAT4X4 _projection;

public:

    Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, int windowWidth, int windowHeight, int nearDepth, int farDepth);
    ~Camera();

    void Update(const float deltaTime);

    inline const XMFLOAT4X4& GetView() const { return _view; }
    inline const XMFLOAT4X4& GetProjection() const { return _projection; }

    inline XMFLOAT3 GetPosition() const { return _eye; }
    inline XMFLOAT3 GetLookAt() const { return _at; }
    inline XMFLOAT3 GetUp() const { return _up; }

    XMMATRIX& GetCameraMatrix();

    inline void SetPosition(XMFLOAT3 position) { _eye = position; }
    inline void SetLookAt(XMFLOAT3 lookAt) { _at = lookAt; }
    inline void SetUp(XMFLOAT3 up) { _up = up; }

    void SetCameraPosition(float x, float y, float z);

    void Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth);
};