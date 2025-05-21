#include "Application.h"
#include <d3dcompiler.h>


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

bool Application::HandleKeyboard(MSG msg)
{
	XMFLOAT3 cameraPosition = _camera->GetPosition();

	switch (msg.wParam)
	{
	case VK_UP:
		_cameraOrbitRadius = max(_cameraOrbitRadiusMin, _cameraOrbitRadius - (_cameraSpeed * 0.2f));
		return true;
		break;

	case VK_DOWN:
		_cameraOrbitRadius = min(_cameraOrbitRadiusMax, _cameraOrbitRadius + (_cameraSpeed * 0.2f));
		return true;
		break;

	case VK_RIGHT:
		_cameraOrbitAngleXZ -= _cameraSpeed;
		return true;
		break;

	case VK_LEFT:
		_cameraOrbitAngleXZ += _cameraSpeed;
		return true;
		break;

	case WM_SETFOCUS: // Called on window focus
	{
		if (application != nullptr)
			application->_ButtonPressed = true;
		break;
	}

	case WM_KILLFOCUS: // Called on loss of window focus
	{
		if (application != nullptr)
			application->_ButtonPressed = false;
		break;
	}
}

	return false;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	CCWcullMode=nullptr;
	CWcullMode= nullptr;
	DSLessEqual = nullptr;
	RSCullNone = nullptr;
	 _WindowHeight = 0;
	 _WindowWidth = 0;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	//Get textures
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\stone.dds", nullptr, &_pTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\stone2.dds", nullptr, &_pTextureRV2);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\stone3.dds", nullptr, &_pTextureRV3);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\floor.dds", nullptr, &_pGroundTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\smoke.dds", nullptr, &_pTextureRV4);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\water.dds", nullptr, &_waterTexture);

    // Setup Camera
	XMFLOAT3 eye = XMFLOAT3(0.0f, 2.0f, -1.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 2.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_collisionHandler = new CollisionHandler();
	_camera = new Camera(eye, at, up, (float)_renderWidth, (float)_renderHeight, 0.01f, 200.0f);

	// Setup the scene's light
	basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	//geometry
	Geometry herculesGeometry;

	objMeshData = OBJLoader::Load((char*)"donut.obj", _pd3dDevice);
	herculesGeometry.indexBuffer = objMeshData.IndexBuffer;
	herculesGeometry.numberOfIndices = objMeshData.IndexCount;
	herculesGeometry.vertexBuffer = objMeshData.VertexBuffer;
	herculesGeometry.vertexBufferOffset = objMeshData.VBOffset;
	herculesGeometry.vertexBufferStride = objMeshData.VBStride;
	
	Geometry cubeGeometry;
	cubeGeometry.indexBuffer = _pIndexBuffer;
	cubeGeometry.vertexBuffer = _pVertexBuffer;
	cubeGeometry.numberOfIndices = 36;
	cubeGeometry.vertexBufferOffset = 0;
	cubeGeometry.vertexBufferStride = sizeof(SimpleVertex);
	
	Geometry sphereGeometry;
	MeshData objMeshData = OBJLoader::Load((char*)"sphere.obj", _pd3dDevice);
	sphereGeometry.indexBuffer = objMeshData.IndexBuffer;
	sphereGeometry.numberOfIndices = objMeshData.IndexCount;
	sphereGeometry.vertexBuffer = objMeshData.VertexBuffer;
	sphereGeometry.vertexBufferOffset = objMeshData.VBOffset;
	sphereGeometry.vertexBufferStride = objMeshData.VBStride;

	Geometry planeGeometry;
	planeGeometry.indexBuffer = _pPlaneIndexBuffer;
	planeGeometry.vertexBuffer = _pPlaneVertexBuffer;
	planeGeometry.numberOfIndices = 6;
	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.vertexBufferStride = sizeof(SimpleVertex);

	Material shinyMaterial;
	shinyMaterial.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial.specularPower = 10.0f;

	Material noSpecMaterial;
	noSpecMaterial.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial.specularPower = 0.0f;

//-----------------------------------------------------FLOOR--------------------------------------------------
	Appearance* floorAppearance = new Appearance(planeGeometry, noSpecMaterial);
	floorAppearance->SetTextureRV(_pGroundTextureRV);

	Transform* floorTransform = new Transform();
	floorTransform->SetPosition(0.0f, -0.3f, 10.0f);
	floorTransform->SetScale(15.0f, 15.0f, 15.0f);
	floorTransform->SetRotation(Vector3D(XMConvertToRadians(90.0f), 0.0f,0.0f));



	ParticleModel* particleModel = new ParticleModel(floorTransform, true, Vector3D(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.5f);

	floorGameObject = new GameObject("Floor", floorAppearance, floorTransform, particleModel);

	Vector3D floorVelocity = Vector3D(0.0f, 0.0f, 0.0f);
	floorGameObject->GetParticleModel()->SetVelocity(floorVelocity);


	_gameObjects.push_back(floorGameObject);
	
//----------------------------------------------------CUBES---------------------------------------------------

	Appearance* cubeAppearance = new Appearance(cubeGeometry, shinyMaterial);
	cubeAppearance->SetTextureRV(_pTextureRV);

	Appearance* sphereAppearance = new Appearance(sphereGeometry, shinyMaterial);
	sphereAppearance->SetTextureRV(_pTextureRV2);

	Appearance* sphereAppearanceBlue = new Appearance(sphereGeometry, shinyMaterial);
	sphereAppearanceBlue->SetTextureRV(_pTextureRV3);

	Appearance* cubeAppearanceBlue = new Appearance(cubeGeometry, shinyMaterial);
	cubeAppearanceBlue->SetTextureRV(_pTextureRV3);

	for (auto i = 0; i < 7; i++)
	{
		Transform* cubeTransform = new Transform();
		cubeTransform->SetScale(0.5f, 0.5f, 0.5f);
		cubeTransform->SetPosition(-4.0f + (i * 2.0f), 0.5f, 10.0f);

		BoundingSphere sphere = BoundingSphere(0.5f, cubeTransform->GetPosition());
		BoundingBox box = BoundingBox(cubeTransform->GetPosition(), Vector3D{ 0.27f,0.27f,0.27f });
		ParticleModel* particleModel = new RigidBody(cubeTransform, false, Vector3D{ 0.0f,0.0f,0.0f }, XMFLOAT3{ 0.0f,0.0f,0.0f });

		particleModel->SetBox(box);
		particleModel->SetSphere(sphere);
	

		if (i ==0)
		{
			GameObject* gameObject = new GameObject("Sphere " + i, sphereAppearanceBlue, cubeTransform, particleModel);
			_gameObjects.push_back(gameObject);
		}
		else if (i == 1)
		{
			GameObject* gameObject = new GameObject("Cube " + i, cubeAppearanceBlue, cubeTransform, particleModel);
			_gameObjects.push_back(gameObject);
		}
		else if (i == 2 ||i ==3)
		{
			GameObject* gameObject = new GameObject("Sphere " + i, sphereAppearance, cubeTransform, particleModel);
			_gameObjects.push_back(gameObject);
		}
		else if(i == 4 || i ==5)
		{
			GameObject* gameObject = new GameObject("Cube " + i, cubeAppearance, cubeTransform, particleModel);
			_gameObjects.push_back(gameObject);
		}
		else
		{
			Transform* waterTransform = new Transform();
			waterTransform->SetScale(2.0f, 1.0f, 0.6f);
			waterTransform->SetPosition(2.0f, 0.2f, 10.0f);

			GameObject* gameObject = new GameObject("Cube Water" + i, cubeAppearance, waterTransform, particleModel);
			gameObject->SetActive(false);
			_gameObjects.push_back(gameObject);
		}
	}	
	
//-------------------------------------------------OTHER------------------------------------------------------
	//Vector Testing
	Vector3D V1 = Vector3D(12, 50, 4);
	Vector3D V2 = Vector3D(3, 10, 2);
	Vector3D V3 = V1 + V2;

	application = this;
	_ButtonPressed = true;
//-------------------------------------------------Particle System------------------------------------------------------

	//water particles
	Appearance* smokeAppearance = new Appearance(cubeGeometry, noSpecMaterial);
	smokeAppearance->SetTextureRV(_waterTexture);

	Transform* ps2Transform = new Transform();
	_ps2 = new ParticleSystem(ps2Transform, { 5.0f, 0.0f, 10.0f }, smokeAppearance, 1);

	//Rain
	Transform* psTransform = new Transform();
	_ps = new ParticleSystem(psTransform, { 0.0f, 0.0f, 10.0f }, cubeAppearanceBlue,0);

//-----------------------------------------------Floating Object-----------------------------------------------------------

	Appearance* FloatingObjectAppearance = new Appearance(sphereGeometry, noSpecMaterial);
	FloatingObjectAppearance->SetTextureRV(_pGroundTextureRV);

	Transform* floatingObjectTransform = new Transform();
	floatingObjectTransform->SetPosition(0.0f, 0.40f, 1.0f);
	floatingObjectTransform->SetScale(1.0f, 1.0f, 1.0f);
	floatingObjectTransform->SetRotation(Vector3D(XMConvertToRadians(90.0f), 0.0f, 0.0f));

	ParticleModel* floatingObjectParticleModel = new ParticleModel(floatingObjectTransform, true, Vector3D(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.5f);

	floorGameObject = new GameObject("Floating Object", FloatingObjectAppearance, floatingObjectTransform, floatingObjectParticleModel);

	Vector3D floatingObjectVelocity = Vector3D(0.0f, 0.0f, 0.0f);
	floorGameObject->GetParticleModel()->SetVelocity(floatingObjectVelocity);

	_gameObjects.push_back(floorGameObject);

//------------------------------------------------Water-------------------------------------------------------------------

	Appearance* WaterAppearance = new Appearance(planeGeometry, noSpecMaterial);
	WaterAppearance->SetTextureRV(_waterTexture);

	Transform* waterTransform = new Transform();
	waterTransform->SetPosition(0.0f, 0.3f, 0.0f);
	waterTransform->SetScale(7.0f, 7.0f, 7.0f);
	waterTransform->SetRotation(Vector3D(XMConvertToRadians(90.0f), 0.0f, 0.0f));

	ParticleModel* waterParticleModel = new ParticleModel(waterTransform, true, Vector3D(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.5f);

	waterObject = new GameObject("water", WaterAppearance, waterTransform, waterParticleModel);

	Vector3D waterVelocity = Vector3D(0.0f, 0.0f, 0.0f);
	waterObject->GetParticleModel()->SetVelocity(waterVelocity);

	_gameObjects.push_back(waterObject);

	for (int i = 0; i < _gameObjects.size(); i++)
	{
		_gameObjects[i]->GetParticleModel()->SetGrav(false);
	}

	//disable water bouyancy
	_gameObjects[8]->GetParticleModel()->SetBuoyancy(false);
	_gameObjects[8]->GetTransform()->SetPosition(_gameObjects[8]->GetTransform()->GetPosition().x, 0.5f, _gameObjects[8]->GetTransform()->GetPosition().z);
	_gameObjects[9]->SetActive(false);
	_gameObjects[8]->SetActive(false);


	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile((WCHAR*)L"shader.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile((WCHAR*)L"shader.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;
	
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	// Create vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 5.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	// Create plane index buffer
	WORD planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 960, 540};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"Physics Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,



        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT sampleCount = 4;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _renderWidth;
    sd.BufferDesc.Height = _renderHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleCount;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_renderWidth;
    vp.Height = (FLOAT)_renderHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _renderWidth;
	depthStencilDesc.Height = _renderHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Rasterizer
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CWcullMode);

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
	if (_pSamplerLinear) _pSamplerLinear->Release();

	if (_pTextureRV) _pTextureRV->Release();

	if (_pGroundTextureRV) _pGroundTextureRV->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();

    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
	if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
	if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();

    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	if (DSLessEqual) DSLessEqual->Release();
	if (RSCullNone) RSCullNone->Release();

	if (CCWcullMode) CCWcullMode->Release();
	if (CWcullMode) CWcullMode->Release();

	if (_camera)
	{
		delete _camera;
		_camera = nullptr;
	}

	for (auto gameObject : _gameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
			gameObject = nullptr;
		}
	}
}

void Application::moveForward(int objectNumber, float deltaTime)
{
	_gameObjects[objectNumber]->GetParticleModel()->setVel(0.0001, 0, 0);
	_gameObjects[objectNumber]->GetParticleModel()->MoveConstantAcceleration(deltaTime);
}

void Application::MoveSphere(float deltaTime)
{
	if (_gameObjects[1]->GetParticleModel()->GetCollidingBool() == false)
	{
		_gameObjects[1]->GetParticleModel()->setVel(0.0001, 0, 0);
		_gameObjects[1]->GetParticleModel()->MoveConstantAcceleration(deltaTime);
	}
}

void Application::moveBackward(int objectNumber, float deltaTime)
{

	_gameObjects[objectNumber]->GetParticleModel()->setVel(-0.0001, 0, 0);
	_gameObjects[objectNumber]->GetParticleModel()->MoveConstVelocity(deltaTime);
	
}

void Application::Update()
{
    // Update our time
    static float deltaTime = 0.0f;
    static DWORD dwTimeStart = 0;

    DWORD dwTimeCur = GetTickCount64();

    if (dwTimeStart == 0)
        dwTimeStart = dwTimeCur;

	deltaTime = (dwTimeCur - dwTimeStart) / 1000.0f;


	// Update camera
	float angleAroundZ = XMConvertToRadians(_cameraOrbitAngleXZ);

	float x = _cameraOrbitRadius * cos(angleAroundZ);
	float z = _cameraOrbitRadius * sin(angleAroundZ);

	XMFLOAT3 cameraPos = _camera->GetPosition();
	cameraPos.x = x;
	cameraPos.z = z;

	_camera->SetPosition(cameraPos);
	_camera->Update();

	if (_ButtonPressed)
	{
		Keyboard(deltaTime);
	}
	

	for (int i = 0; i < _gameObjects.size(); i++)  //update gameobjects
	{
		_gameObjects[i]->Update(deltaTime);	
	}

	_ps->Update(deltaTime);
	_ps2->Update(deltaTime);

//-----------------------------------------------------------------------Sphere Cube collisions------------------------------------------------
	ParticleModel* particleModel = _gameObjects[1]->GetParticleModel();
	
	if (particleModel)
	{

			BoundingSphere sphere1 = _gameObjects[1]->GetParticleModel()->GetSphere();
			BoundingSphere sphere2 = _gameObjects[2]->GetParticleModel()->GetSphere();

			BoundingBox box1 = _gameObjects[1]->GetParticleModel()->GetBox();
			BoundingBox box2 = _gameObjects[2]->GetParticleModel()->GetBox();

			if (_collisionHandler->CheckCollision(sphere1, box2))
			{
				particleModel->SetCollidingBool(true);
				Vector3D vel = _gameObjects[1]->GetParticleModel()->GetVelocity();
				//_collisionHandler->CollisionResponseSphereCube(_gameObjects[1]->GetParticleModel()->GetVelocity(), _gameObjects[1], _gameObjects[2], deltaTime);
				//_collisionHandler->CollisionResponseInterPenetration(deltaTime, _gameObjects[1], _gameObjects[2]);
			}
			else
			{
				particleModel->SetCollidingBool(false);
			}

	}

//-----------------------------------------------------------------------sphere sphere collisions------------------------------------------------
	ParticleModel* sphereParticleModel = _gameObjects[3]->GetParticleModel();

	if (sphereParticleModel)
	{

		BoundingSphere sphere1 = _gameObjects[3]->GetParticleModel()->GetSphere();
		BoundingSphere sphere2 = _gameObjects[4]->GetParticleModel()->GetSphere();

		if (_collisionHandler->CheckCollision(sphere1, sphere2))
		{
			sphereParticleModel->SetCollidingBool(true);
			Vector3D vel = _gameObjects[3]->GetParticleModel()->GetVelocity();
			//_collisionHandler->CollisionResponseMoveVel(_gameObjects[3]->GetParticleModel()->GetVelocity(), _gameObjects[3], _gameObjects[4], deltaTime);		
			_collisionHandler->CollisionResponseInterPenetration(deltaTime, _gameObjects[3], _gameObjects[4]);
		}
		else
		{
			sphereParticleModel->SetCollidingBool(false);
		}

	}

//-----------------------------------------------------------------------Cube Cube collisions------------------------------------------------
	ParticleModel* cubeSphereParticleModel = _gameObjects[5]->GetParticleModel();

	if (cubeSphereParticleModel)
	{

		BoundingBox box1 = _gameObjects[5]->GetParticleModel()->GetBox();
		BoundingBox box2 = _gameObjects[6]->GetParticleModel()->GetBox();


		if (_collisionHandler->CheckCollision(box1, box2))
		{
			cubeSphereParticleModel->SetCollidingBool(true);
			Vector3D vel = _gameObjects[5]->GetParticleModel()->GetVelocity();
			_collisionHandler->CollisionResponseMoveVel(_gameObjects[CurrentSelectedObject]->GetParticleModel()->GetVelocity(), _gameObjects[CurrentSelectedObject], _gameObjects[SecondaryObject], deltaTime);
		}
		else
		{
			cubeSphereParticleModel->SetCollidingBool(false);
		}


	}
}

void Application::Keyboard(float deltaTime)
{
	// Move gameobject
	if (GetAsyncKeyState('2') < 0)
	{
		MoveSphere(deltaTime);
	}
	if (GetAsyncKeyState('1') < 0)
	{
		moveBackward(1, deltaTime);
	}

	if (GetAsyncKeyState('4') < 0)
	{
		moveForward(3, deltaTime);
	}
	if (GetAsyncKeyState('3') < 0)
	{
		moveBackward(3, deltaTime);
	}

	if (GetAsyncKeyState('6') < 0)
	{
		moveForward(5, deltaTime);
		CurrentSelectedObject = 5;
		SecondaryObject = 6;
	}
	if (GetAsyncKeyState('5') < 0)
	{
		moveBackward(5, deltaTime);
		CurrentSelectedObject = 5;
		SecondaryObject = 6;
	}

	if (GetAsyncKeyState('7') < 0)
	{
		moveBackward(6, deltaTime);
		CurrentSelectedObject = 6;
		SecondaryObject = 5;
	}

	if (GetAsyncKeyState('8') < 0)
	{
		moveForward(6, deltaTime);
		CurrentSelectedObject = 6;
		SecondaryObject = 5;
	}

	//thrust
	if (GetAsyncKeyState(VK_SPACE) < 0)
	{
		if (_gameObjects[3]->GetParticleModel()->GetGrav())
		{
			_gameObjects[3]->GetParticleModel()->SetThrust(0.0f, _gameObjects[3]->GetParticleModel()->GetThrust().y + 0.00010f, 0.0f);
		}
		else 
		{
			_gameObjects[3]->GetParticleModel()->SetThrust(0.0f, _gameObjects[3]->GetParticleModel()->GetThrust().y + 0.000001f, 0.0f);
		}
	}

	//rotations
	if (GetAsyncKeyState('W') < 0)
	{
		_gameObjects[2]->GetRigidBody()->AddRotationForce(Vector3D(0, 0.0005f, 0),Vector3D(0,0,0), deltaTime);
	}
	if (GetAsyncKeyState('A') < 0)
	{
		_gameObjects[2]->GetRigidBody()->AddRotationForce(Vector3D(-0.0005f,0 , 0), Vector3D(0, 0, 0), deltaTime);

	}

	if (GetAsyncKeyState('S') < 0)
	{
		_gameObjects[2]->GetRigidBody()->AddRotationForce(Vector3D(0, -0.0005f, 0), Vector3D(0, 0, 0), deltaTime);
	}
	if (GetAsyncKeyState('D') < 0)
	{
		_gameObjects[2]->GetRigidBody()->AddRotationForce(Vector3D(0.0005f, 0, 0), Vector3D(0, 0, 0), deltaTime);
	}

	//enable and disable particles
	if (GetAsyncKeyState('Z') < 0)
	{
		_ps->SetParticlesActive(true);
	}
	if (GetAsyncKeyState('X') < 0)
	{
		_ps->SetParticlesActive(false);
	}

	if (GetAsyncKeyState('C') < 0)
	{
		_ps2->SetParticlesActive(true);
	}
	if (GetAsyncKeyState('V') < 0)
	{
		_ps2->SetParticlesActive(false);
	}

	//bauyancy
	if (GetAsyncKeyState('B') < 0)
	{
		_gameObjects[8]->GetParticleModel()->SetMass(2.5f);
		_gameObjects[8]->GetParticleModel()->SetBuoyancy(true);
		_gameObjects[9]->SetActive(true);
		_gameObjects[8]->SetActive(true);
	}
	if (GetAsyncKeyState('N') < 0)
	{
		_gameObjects[8]->GetParticleModel()->SetBuoyancy(false);
		_gameObjects[8]->GetTransform()->SetPosition(_gameObjects[8]->GetTransform()->GetPosition().x, 0.5f, _gameObjects[8]->GetTransform()->GetPosition().z);
		_gameObjects[9]->SetActive(false);
		_gameObjects[8]->SetActive(false);
	}

	//enable and disable gravity
	if (GetAsyncKeyState('G') < 0) 
	{
		_gameObjects[3]->GetParticleModel()->SetGrav(true);
	}
	if (GetAsyncKeyState('H') < 0)
	{
		_gameObjects[3]->GetParticleModel()->SetGrav(false);
	}
}

void Application::Draw()
{
	//
	// Clear buffers
	//

	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//
	// Setup buffers and render scene
	//

	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

	ConstantBuffer cb;

	XMFLOAT4X4 viewAsFloats = _camera->GetView();
	XMFLOAT4X4 projectionAsFloats = _camera->GetProjection();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);

	cb.light = basicLight;
	cb.EyePosW = _camera->GetPosition();


	// Render all scene objects
	for (int i = 0; i <= NUMBER_OF_CUBES; i++)//auto gameObject : _gameObjects
	{
		ConstantBufferFunc(_gameObjects[i], cb);
	
		// Draw object
		_gameObjects[i]->Draw(_pImmediateContext);
	}


	for (auto particles : _ps->GetParticles()) // render rain
	{

		Material material = particles->GetAppearance()->GetMaterial();

		cb.surface.AmbientMtrl = material.ambient;
		cb.surface.DiffuseMtrl = material.diffuse;
		cb.surface.SpecularMtrl = material.specular;
		cb.World = XMMatrixTranspose(particles->GetTransform()->GetWorldMatrix());


		if (particles->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView* textureRV = particles->GetAppearance()->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}

		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		particles->Draw(_pImmediateContext);
	}


	for (auto particles : _ps2->GetParticles()) // render smoke
	{

		Material material = particles->GetAppearance()->GetMaterial();

		cb.surface.AmbientMtrl = material.ambient;
		cb.surface.DiffuseMtrl = material.diffuse;
		cb.surface.SpecularMtrl = material.specular;
		cb.World = XMMatrixTranspose(particles->GetTransform()->GetWorldMatrix());


		if (particles->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView* textureRV = particles->GetAppearance()->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}

		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		particles->Draw(_pImmediateContext);
	}


    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}

void Application::ConstantBufferFunc(GameObject* gameObject, ConstantBuffer& cb)
{

	// Get render material
	Material material = gameObject->GetAppearance()->GetMaterial();


	// Copy material to shader
	cb.surface.AmbientMtrl = material.ambient;
	cb.surface.DiffuseMtrl = material.diffuse;
	cb.surface.SpecularMtrl = material.specular;

	// Set world matrix
	cb.World = XMMatrixTranspose(gameObject->GetTransform()->GetWorldMatrix());

	// Set texture
	if (gameObject->GetAppearance()->HasTexture())
	{
		ID3D11ShaderResourceView* textureRV = gameObject->GetAppearance()->GetTextureRV();
		_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
		cb.HasTexture = 1.0f;
	}
	else
	{
		cb.HasTexture = 0.0f;
	}

	// Update constant buffer
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
}

