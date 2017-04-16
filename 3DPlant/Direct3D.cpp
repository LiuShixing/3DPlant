#include "stdafx.h"
#include "Direct3D.h"


Direct3D::Direct3D() :mhMainWnd(0),
mAppPaused(false),
mMinimized(false),
mMaximized(false),
mResizing(false),
mEnable4xMsaa(true),
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

/// Use 4X MSAA? 
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}


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

	//无背面消隐渲染状态
	D3D11_RASTERIZER_DESC    NoBackDesc;
	ZeroMemory(&NoBackDesc, sizeof(D3D11_RASTERIZER_DESC));
	NoBackDesc.CullMode = D3D11_CULL_NONE;
	NoBackDesc.FillMode = D3D11_FILL_SOLID;
	NoBackDesc.DepthClipEnable = true;
	if (FAILED(md3dDevice->CreateRasterizerState(&NoBackDesc, &mNoBackRS)))
		return 0;

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
	
	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	

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
void Direct3D::Draw(std::vector<Trunk>& trunks, std::vector<Leave>& leaves)
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

	//--------------------------------draw------------------------------------------------------------------
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

	// draw trunk---------------------------------------------------------------------------------------

	//update trunk
	mDiffuseMap->SetResource(mShaderSView);
	md3dImmediateContext->IASetInputLayout(mPosTexLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	stride = sizeof(Vertex::PosTex);
	offset = 0;
	this->md3dImmediateContext->IASetVertexBuffers(0, 1, &mpTrunkVB, &stride, &offset);
	this->md3dImmediateContext->IASetIndexBuffer(mpTrunkIB, DXGI_FORMAT_R32_UINT, 0);
	for (int i = 0; i < trunks.size(); i++)
	{
		XMMATRIX sizeScal = XMMatrixScaling(trunks[i].sizeScal, trunks[i].scalY, trunks[i].sizeScal);
		world = XMMatrixTranslation(trunks[i].pos.x, trunks[i].pos.y, trunks[i].pos.z);
		XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&trunks[i].rotAxis), trunks[i].angle);
		worldViewProj = sizeScal*rot*world*View*Proj;
		mPosTexWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

		techDesc;
		mPosTexTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			mPosTexTech->GetPassByIndex(p)->Apply(0, this->md3dImmediateContext);
			this->md3dImmediateContext->DrawIndexed(mTrunkIndexCount, 0, 0);
		}
	}

	//draw leaves
	mDiffuseMap->SetResource(mLeaveShaderSView);
	md3dImmediateContext->IASetInputLayout(mPosTexLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->RSSetState(mNoBackRS);

	stride = sizeof(Vertex::PosTex);
	offset = 0;
	this->md3dImmediateContext->IASetVertexBuffers(0, 1, &mpLeaveVB, &stride, &offset);
	this->md3dImmediateContext->IASetIndexBuffer(mpLeaveIB, DXGI_FORMAT_R32_UINT, 0);
	float rotY[4] = { 0.0f, XM_PI / 2.0f, XM_PI, XM_PI*1.5f };
	for (int i = 0; i < leaves.size(); i++)
	{
		XMMATRIX RotYM = XMMatrixRotationY(rotY[i%4]);
		XMMATRIX sizeScal = XMMatrixScaling(leaves[i].scal, leaves[i].scal, leaves[i].scal);
		world = XMMatrixTranslation(leaves[i].pos.x, leaves[i].pos.y, leaves[i].pos.z);
		XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&leaves[i].rotAxis), leaves[i].angle);
		worldViewProj = sizeScal*RotYM*rot*world*View*Proj;
		mPosTexWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

		techDesc;
		mPosTexTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			mPosTexTech->GetPassByIndex(p)->Apply(0, this->md3dImmediateContext);
			this->md3dImmediateContext->DrawIndexed(6, 0, 0);
		}
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
		MessageBoxA(mhMainWnd, (char*)errMSG->GetBufferPointer(), "ShaderCompileError", MB_OK);
		errMSG->Release();
		return 0;
	}
	if (FAILED(hr))
	{
		MessageBox(mhMainWnd, L"CompileShader错误!", L"错误", MB_OK);
		return 0;
	}
	if (FAILED(::D3DX11CreateEffectFromMemory(shader->GetBufferPointer(), shader->GetBufferSize(), 0, md3dDevice, &mpEffect)))
	{
		MessageBox(mhMainWnd, L"D3DX11CreateEffectFromMemory错误!", L"错误", MB_OK);
		return 0;
	}
	ReleaseCOM(shader);

	mTech = mpEffect->GetTechniqueByName("PosColTech");
	mWorldViewProj = mpEffect->GetVariableByName("gWorldViewProj")->AsMatrix();

	//build 2
	ID3D10Blob* shader2(NULL);
	ID3D10Blob* errMSG2(NULL);
	const std::wstring filename2 = L"HLSL/pos_tex.hlsl";
	hr = D3DX11CompileFromFile(filename2.c_str(), 0, 0, 0, "fx_5_0", flag, 0, 0, &shader2, &errMSG2, 0);
	if (errMSG2)
	{
		MessageBoxA(mhMainWnd, (char*)errMSG2->GetBufferPointer(), "ShaderCompileError    2", MB_OK);
		errMSG2->Release();
		return 0;
	}
	if (FAILED(hr))
	{
		MessageBox(mhMainWnd, L"CompileShader错误!   2", L"错误", MB_OK);
		return 0;
	}
	if (FAILED(::D3DX11CreateEffectFromMemory(shader2->GetBufferPointer(), shader2->GetBufferSize(), 0, md3dDevice, &mpEffect)))
	{
		MessageBox(mhMainWnd, L"D3DX11CreateEffectFromMemory错误!   2", L"错误", MB_OK);
		return 0;
	}
	ReleaseCOM(shader2);
	
	mPosTexTech = mpEffect->GetTechniqueByName("PosTexTech");
	mPosTexWorldViewProj = mpEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	mDiffuseMap = mpEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

	std::cout << "BuildHLSL  success" << std::endl;
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

	//layout 2
	std::cout << " in initlayout " << std::endl;
	if (!mPosTexTech)
	{
		MessageBox(0, L"mPosTexTech=NULL!! failed", 0, 0);
	}
	const int count2 = 2;
	D3DX11_PASS_DESC PassDesc2;
	D3D11_INPUT_ELEMENT_DESC PosTexDesc[count2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	mPosTexTech->GetPassByIndex(0)->GetDesc(&PassDesc2);
	if (FAILED(md3dDevice->CreateInputLayout(PosTexDesc, count2, PassDesc2.pIAInputSignature,
		PassDesc2.IAInputSignatureSize, &mPosTexLayout)))
	{
		std::cout << "init mPosTexLayout fail" << std::endl;
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

bool Direct3D::CreateTrunkVIBuffer(std::vector<Vertex::PosTex>& vertexs, std::vector<UINT>& indices)
{
	D3D11_BUFFER_DESC vDesc = { 0 };
	vDesc.ByteWidth = vertexs.size() * sizeof(Vertex::PosTex);
	vDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;


	D3D11_SUBRESOURCE_DATA vbdata = { 0 };
	vbdata.pSysMem = &vertexs[0];

	if (FAILED(md3dDevice->CreateBuffer(&vDesc, &vbdata, &mpTrunkVB)))
		return 0;

	//index
	mIndexCount = indices.size();
	D3D11_BUFFER_DESC iDesc = { 0 };
	iDesc.ByteWidth = mIndexCount * sizeof(UINT);
	iDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA ibdata = { 0 };
	ibdata.pSysMem = &indices[0];
	if (FAILED(md3dDevice->CreateBuffer(&iDesc, &ibdata, &mpTrunkIB)))
		return 0;

	return 1;
}
bool Direct3D::CreateLeaveVIBuffer()
{
	std::vector<Vertex::PosTex> vertexs(4);
	vertexs[0].pos = XMFLOAT3(-1.0f,0.0f,0.0f);
	vertexs[0].tex = XMFLOAT2(0.0f,1.0f);
	vertexs[1].pos = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertexs[1].tex = XMFLOAT2(0.0f, 0.0f);
	vertexs[2].pos = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexs[2].tex = XMFLOAT2(1.0f, 0.0f);
	vertexs[3].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertexs[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vDesc = { 0 };
	vDesc.ByteWidth = vertexs.size() * sizeof(Vertex::PosTex);
	vDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;


	D3D11_SUBRESOURCE_DATA vbdata = { 0 };
	vbdata.pSysMem = &vertexs[0];

	if (FAILED(md3dDevice->CreateBuffer(&vDesc, &vbdata, &mpLeaveVB)))
		return 0;

	//index
	UINT indices[6] = {
		0,1,2,
		2,3,0
	};
	
	mIndexCount = 6;
	D3D11_BUFFER_DESC iDesc = { 0 };
	iDesc.ByteWidth = mIndexCount * sizeof(UINT);
	iDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA ibdata = { 0 };
	ibdata.pSysMem = indices;
	if (FAILED(md3dDevice->CreateBuffer(&iDesc, &ibdata, &mpLeaveIB)))
		return 0;

	return 1;
}
void Direct3D::ReleaseVIBuffer()
{
	ReleaseCOM(mpVB);
	ReleaseCOM(mpIB);
	ReleaseCOM(mpTrunkVB);
	ReleaseCOM(mpTrunkIB);
}


void Direct3D::CreateCylinder(float bottomRadius, float topRadius, float height, 
	UINT sliceCount, UINT stackCount, std::vector<Vertex::PosTex>& vertexs, std::vector<UINT>& indices)
{
	vertexs.clear();
	indices.clear();

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ringCount; ++i)
	{
	//	float y = -0.5f*height + i*stackHeight;
		float y = i*stackHeight;             //模型中心在原点
		float r = bottomRadius + i*radiusStep;

		// vertices of ring
		float dTheta = 2.0f*XM_PI / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			Vertex::PosTex vertex;

			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			vertex.pos = XMFLOAT3(r*c, y, r*s);

			vertex.tex.x = (float)j / sliceCount;
			vertex.tex.y = 1.0f - (float)i / stackCount;

			vertexs.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			indices.push_back(i*ringVertexCount + j);
			indices.push_back((i + 1)*ringVertexCount + j);
			indices.push_back((i + 1)*ringVertexCount + j + 1);

			indices.push_back(i*ringVertexCount + j);
			indices.push_back((i + 1)*ringVertexCount + j + 1);
			indices.push_back(i*ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, vertexs, indices);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, vertexs, indices);
	mTrunkIndexCount = indices.size();
}

void Direct3D::BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
	UINT sliceCount, UINT stackCount, std::vector<Vertex::PosTex>& vertexs, std::vector<UINT>& indices)
{
	UINT baseIndex = (UINT)vertexs.size();

//	float y = 0.5f*height;
	float y = height;  //模型中心在原点
	float dTheta = 2.0f*XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		Vertex::PosTex ver;

		ver.pos.x = topRadius*cosf(i*dTheta);
		ver.pos.y = y;
		ver.pos.z = topRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		ver.tex.x = ver.pos.x / height + 0.5f;
		ver.tex.y = ver.pos.z / height + 0.5f;

		vertexs.push_back(ver);
	}

	// Cap center vertex.
	Vertex::PosTex ver;
	ver.pos.x = 0.0f;
	ver.pos.y = y;
	ver.pos.z = 0.0f;
	ver.tex.x = 0.5f;
	ver.tex.y = 0.5f;
	vertexs.push_back(ver);

	// Index of center vertex.
	UINT centerIndex = (UINT)vertexs.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}
}

void Direct3D::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
	UINT sliceCount, UINT stackCount, std::vector<Vertex::PosTex>& vertexs, std::vector<UINT>& indices)
{
	// 
	// Build bottom cap.
	//

	UINT baseIndex = (UINT)vertexs.size();
//	float y = -0.5f*height;
	float y = 0.0f;//模型中心在原点

	// vertices of ring
	float dTheta = 2.0f*XM_PI / sliceCount;
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		Vertex::PosTex ver;

		ver.pos.x = bottomRadius*cosf(i*dTheta);
		ver.pos.y = y;
		ver.pos.z = bottomRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		ver.tex.x = ver.pos.x / height + 0.5f;
		ver.tex.y = ver.pos.z / height + 0.5f;

		vertexs.push_back(ver);
	}

	// Cap center vertex.
	Vertex::PosTex ver;
	ver.pos.x = 0.0f;
	ver.pos.y = y;
	ver.pos.z = 0.0f;
	ver.tex.x = 0.5f;
	ver.tex.y = 0.5f;
	vertexs.push_back(ver);

	// Cache the index of center vertex.
	UINT centerIndex = (UINT)vertexs.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
}

bool Direct3D::CreateShaderRV(LPCWSTR fileName, LPCWSTR leave_fileName)
{
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(md3dDevice, fileName, 0, 0, &mShaderSView, 0)))
	{
		return false;
	}
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(md3dDevice, leave_fileName, 0, 0, &mLeaveShaderSView, 0)))
	{
		return false;
	}
	return true;
}