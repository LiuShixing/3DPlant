#include "stdafx.h"
#include "Direct3D.h"


Direct3D::Direct3D() :mhMainWnd(0),
mAppPaused(false),
mMinimized(false),
mMaximized(false),
mResizing(false),
m4xMsaaQuality(0),

md3dDevice(0),
md3dImmediateContext(0),
mSwapChain(0),
mDepthStencilBuffer(0),
mRenderTargetView(0),
mDepthStencilView(0),
mTech(NULL), mRadius(30.0f),
//mTheta(0.0f), mPhi(0.0f)
mTheta(1.5f*XM_PI), mPhi(0.25f*XM_PI) 
{
	XMMATRIX a(XMMatrixIdentity());
	::XMStoreFloat4x4(&mWorld, a);
	::XMStoreFloat4x4(&mView, a);
	::XMStoreFloat4x4(&mProj, a);

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
}


Direct3D::~Direct3D()
{
	ReleaseCOM(md3dDevice);
	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mpEffect);
	ReleaseCOM(mpVB);
	ReleaseCOM(mpIB); 
	ReleaseCOM(mPosColorLayout);
	ReleasePointer(mWorldViewProj); 
	ReleasePointer(mTech);
}

bool Direct3D::Init(HWND mainWnd, int clientWidth, int clientHeight, const std::wstring& filename)
{
	std::cout << "d3d init" << std::endl;

	mhMainWnd = mainWnd;
	HRESULT	hr;

	D3D_FEATURE_LEVEL featureLevels[6] = { D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL	featureLevel;
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, featureLevels, 6,
		D3D11_SDK_VERSION, &md3dDevice, &featureLevel, &md3dImmediateContext);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Craete device failed!", L"ERROR", MB_OK);
		return false;
	}
	
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		//		return false;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	HR(md3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = clientWidth;
	sd.BufferDesc.Height = clientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;


	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.

	OnResize(clientWidth,clientHeight);

	BuildHLSL(filename);

	InitLayout();

	return true;
}

void Direct3D::OnResize(int clientWidth, int clientHeight)
{
	std::cout << "d3d resize" << std::endl;
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(mSwapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = clientWidth;
	depthStencilDesc.Height = clientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));


	// Bind the render target view and depth/stencil view to the pipeline.

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);


	// Set the viewport transform.

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(clientWidth);
	mScreenViewport.Height = static_cast<float>(clientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	//proj matrix
	float aspectRatio = static_cast<float>(clientWidth) / clientHeight;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}
void Direct3D::Draw()
{
	//--------------------------------update--------------------------
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
//	XMVECTOR pos = XMVectorSet(0.0f, 20.0f, mRadius, 1.0f);
//	XMVECTOR target = XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	XMMATRIX RX = XMMatrixRotationX(mPhi);
	XMMATRIX RY = XMMatrixRotationY(-mTheta);
	
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
//	XMMATRIX world = XMMatrixTranslation(0.0f, -10.0f, 0.0f);
//	world = RX*RY*world;
	XMMATRIX View = XMLoadFloat4x4(&mView);
	XMMATRIX Proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*View*Proj;
	mWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	//--------------------------------draw----------------------------
	//std::cout << "d3d draw" << std::endl;
	this->md3dImmediateContext->ClearRenderTargetView(this->mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	this->md3dImmediateContext->ClearDepthStencilView(this->mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mPosColorLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	UINT stride = sizeof(Vertex::PosColor);
	UINT offset = 0;
	this->md3dImmediateContext->IASetVertexBuffers(0, 1, &mpVB, &stride, &offset);
	this->md3dImmediateContext->IASetIndexBuffer(mpIB, DXGI_FORMAT_R32_UINT, 0);
	
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p<techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, this->md3dImmediateContext);
		this->md3dImmediateContext->DrawIndexed(mIndexCount, 0, 0);
	}
	
	//-----------------------------------
	this->mSwapChain->Present(0, 0);
}
bool Direct3D::BuildHLSL(const std::wstring& filename)
{
	UINT flag(0);
#if defined(DEBUG) || defined(_DEBUG)  
	flag |= D3D10_SHADER_DEBUG;
	flag |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif  
	ID3D10Blob* shader(NULL);
	ID3D10Blob* errMSG(NULL);
	HRESULT hr;
	//---------------------------------------------------------------------------------------

	hr = D3DX11CompileFromFile(filename.c_str(), 0, 0, 0, "fx_5_0", flag, 0, 0, &shader, &errMSG, 0);
	if (errMSG)
	{
		MessageBoxA(NULL, (char*)errMSG->GetBufferPointer(), "ShaderCompileError", MB_OK);
		errMSG->Release();
		return 0;
	}
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CompileShader´íÎó!", L"´íÎó", MB_OK);
		return 0;
	}
	if (FAILED(::D3DX11CreateEffectFromMemory(shader->GetBufferPointer(), shader->GetBufferSize(), 0, md3dDevice, &mpEffect)))
	{
		MessageBox(NULL, L"D3DX11CreateEffectFromMemory´íÎó!", L"´íÎó", MB_OK);
		return 0;
	}
	ReleaseCOM(shader);

	mTech = mpEffect->GetTechniqueByName("PosColTech");
	mWorldViewProj = mpEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	return true;
}

bool Direct3D::InitLayout()
{
	if (!mTech)
	{
		MessageBox(0, L"Tech=NULL!! failed", 0, 0);
	}
	const int count = 2;
	D3DX11_PASS_DESC PassDesc;
	D3D11_INPUT_ELEMENT_DESC PosColorDesc[count] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	if (FAILED(md3dDevice->CreateInputLayout(PosColorDesc, count, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mPosColorLayout)))
	{
		std::cout << "init layout fail" << std::endl;
		return 0;
	}

	return true;
}

bool Direct3D::CreateVIBuffer(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices)
{
	D3D11_BUFFER_DESC vDesc = { 0 };
	vDesc.ByteWidth = vertexs.size() * sizeof(Vertex::PosColor);
	vDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	
	D3D11_SUBRESOURCE_DATA vbdata = { 0 };
	vbdata.pSysMem = &vertexs[0];

	if (FAILED(md3dDevice->CreateBuffer(&vDesc, &vbdata, &mpVB)))
		return 0;
	
	//index
	mIndexCount = indices.size();
	D3D11_BUFFER_DESC iDesc = { 0 };
	iDesc.ByteWidth = mIndexCount * sizeof(UINT);
	iDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA ibdata = { 0 };
	ibdata.pSysMem = &indices[0];
	if (FAILED(md3dDevice->CreateBuffer(&iDesc, &ibdata, &mpIB)))
		return 0;
	
	return 1;
}

void Direct3D::ReleaseVIBuffer()
{
	ReleaseCOM(mpVB);
	ReleaseCOM(mpIB);
}