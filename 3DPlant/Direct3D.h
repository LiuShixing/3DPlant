#pragma once

#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <iostream>
#include<vector>
#include "d3dx11Effect.h"

#include"Vertex.h"
#include"LSystem.h"

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
	void Draw(std::vector<Trunk>& trunks);
	bool BuildHLSL(const std::wstring& filename);
	bool InitLayout();
	bool CreateVIBuffer(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices);
	bool CreateTrunkVIBuffer(std::vector<Vertex::PosTex>& vertexs, std::vector<UINT>& indices);
	void ReleaseVIBuffer();

	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, 
		UINT stackCount, std::vector<Vertex::PosTex>& vertexs, std::vector<UINT>& indices);

	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,UINT sliceCount,
		UINT stackCount, std::vector<Vertex::PosTex>& vertexs, std::vector<UINT>& indices);

	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount,
		UINT stackCount, std::vector<Vertex::PosTex>& vertexs, std::vector<UINT>& indices);

	bool CreateShaderRV(LPCWSTR fileName);
private:
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;
	UINT      m4xMsaaQuality;
	bool      mEnable4xMsaa;


	ID3D11Device*          md3dDevice;
	ID3D11DeviceContext*    md3dImmediateContext;
	IDXGISwapChain*        mSwapChain;
	ID3D11Texture2D*       mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT         mScreenViewport;

	ID3DX11Effect*          mpEffect;
	ID3DX11EffectTechnique*  mTech;
	ID3DX11EffectTechnique*  mPosTexTech;

	ID3D11Buffer*           mpVB;
	ID3D11Buffer*           mpIB;

	ID3D11Buffer*           mpTrunkVB;
	ID3D11Buffer*           mpTrunkIB;

	ID3DX11EffectMatrixVariable* mWorldViewProj;
	ID3DX11EffectMatrixVariable* mPosTexWorldViewProj;

	ID3D11InputLayout*       mPosColorLayout;
	ID3D11InputLayout*       mPosTexLayout;

	ID3D11ShaderResourceView*   mShaderSView;
	ID3DX11EffectShaderResourceVariable*   mDiffuseMap;
	//
	XMFLOAT4X4  mWorld;
	XMFLOAT4X4  mView;
	XMFLOAT4X4  mProj;

	UINT       mIndexCount;
	UINT       mTrunkIndexCount;
public:
	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

