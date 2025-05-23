#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include "Camera.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "Vector3D.h"

#include <vector>

#include "GameObject.h"
#include "ParticleModel.h"
#include "Appearance.h"
#include "Transform.h"
#include "RigidBody.h"
#include <directxmath.h>
#include "CollisionHandler.h"

#include "ParticleSystem.h"

using namespace DirectX;

#define NUMBER_OF_CUBES 9
#define FPS60 0.016f
#define FPS30 0.033f


struct SurfaceInfo
{
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 SpecularMtrl;
};

struct Light
{
	XMFLOAT4 AmbientLight;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 SpecularLight;

	float SpecularPower;
	XMFLOAT3 LightVecW;
};

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
	
	SurfaceInfo surface;

	Light light;

	XMFLOAT3 EyePosW;
	float HasTexture;
};

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;

	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;

	ID3D11Buffer*           _pPlaneVertexBuffer;
	ID3D11Buffer*           _pPlaneIndexBuffer;

	ID3D11Buffer*           _pConstantBuffer;

	ID3D11DepthStencilView* _depthStencilView = nullptr;
	ID3D11Texture2D* _depthStencilBuffer = nullptr;

	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11ShaderResourceView* _pTextureRV2 = nullptr;
	ID3D11ShaderResourceView* _pTextureRV3 = nullptr;
	ID3D11ShaderResourceView* _pTextureRV4 = nullptr;
	ID3D11ShaderResourceView * _pTextureRV5 = nullptr;
	ID3D11ShaderResourceView* _waterTexture = nullptr;



	ID3D11ShaderResourceView * _pGroundTextureRV = nullptr;

	ID3D11ShaderResourceView* _pHerculesTextureRV = nullptr;

	ID3D11SamplerState * _pSamplerLinear = nullptr;

	MeshData objMeshData, sphereMeshData;

	Light basicLight;

	vector<GameObject*> _gameObjects;
	vector<GameObject *> _sphereObjects;
	GameObject* floatingObject;
	GameObject* waterObject;



	GameObject* currentObjectIndicator;
	int currentObjectIndex;

	GameObject* floorGameObject;


	Camera * _camera = nullptr;
	float _cameraOrbitRadius = 7.0f;
	float _cameraOrbitRadiusMin = 2.0f;
	float _cameraOrbitRadiusMax = 50.0f;
	float _cameraOrbitAngleXZ = -90.0f;
	float _cameraSpeed = 2.0f;

	UINT _WindowHeight;
	UINT _WindowWidth;

	// Render dimensions - Change here to alter screen resolution
	UINT _renderHeight = 1080;
	UINT _renderWidth = 1920;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

	Application* application;

	//Buttons and their states
	bool _ButtonPressed;
	bool _colliding = false;
	

	CollisionHandler* _collisionHandler;
	ParticleSystem* _ps;
	ParticleSystem* _ps2;

	int CurrentSelectedObject, SecondaryObject;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	void moveForward(int objectNumber, float deltaTime);
	void moveBackward(int objectNumber, float deltaTime);
	void MoveSphere(float deltaTime);
	void Keyboard(float deltaTime);

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	bool HandleKeyboard(MSG msg);

	void Update();
	void Draw();
	void ConstantBufferFunc(GameObject* gameObject, ConstantBuffer& cb);
};

