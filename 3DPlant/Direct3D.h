#pragma once

#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <iostream>
#include<vector>
#include "d3dx11Effect.h"

#include"Vertex.h"

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"d3dx11.lib")
#pragma comment (lib,"dxguid.lib")

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
	HRESULT hr = (x);                                      \
if (FAILED(hr))                                         \
		{                                                      \
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
#define ReleasePointer(x) { if(x){ x=NULL; } }

class Direct3D
{
public:
	Direct3D();
	~Direct3D();

	bool Init(HWND mainWnd, int clientWidth, int clientHeight, const std::wstring& filename);
	void OnResize(int clientWidth, int clientHeight);
	void Draw();
	bool BuildHLSL(const std::wstring& filename);
	bool InitLayout();
	bool CreateVIBuffer(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices);
	void ReleaseVIBuffer();
private:
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;
	UINT      m4xMsaaQuality;


	ID3D11Device*          md3dDevice;
	ID3D11DeviceContext*    md3dImmediateContext;
	IDXGISwapChain*        mSwapChain;
	ID3D11Texture2D*       mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT         mScreenViewport;

	ID3DX11Effect*          mpEffect;
	ID3DX11EffectTechnique*  mTech;

	ID3D11Buffer*           mpVB;
	ID3D11Buffer*           mpIB;

	ID3DX11EffectMatrixVariable* mWorldViewProj;

	ID3D11InputLayout*       mPosColorLayout;

	//
	XMFLOAT4X4  mWorld;
	XMFLOAT4X4  mView;
	XMFLOAT4X4  mProj;

	UINT       mIndexCount;
public:
	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

