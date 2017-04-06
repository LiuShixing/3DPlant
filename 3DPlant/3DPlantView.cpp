
// 3DPlantView.cpp : CMy3DPlantView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "3DPlant.h"
#endif

#include "3DPlantDoc.h"
#include "3DPlantView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy3DPlantView

IMPLEMENT_DYNCREATE(CMy3DPlantView, CView)

BEGIN_MESSAGE_MAP(CMy3DPlantView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMy3DPlantView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYUP()
	ON_COMMAND(ID_SETTING, &CMy3DPlantView::OnSetting)
	ON_MESSAGE(WM_SETTING_OK, &CMy3DPlantView::OnSettingOk)
END_MESSAGE_MAP()

// CMy3DPlantView 构造/析构

CMy3DPlantView::CMy3DPlantView()
{
	// TODO:  在此处添加构造代码

}

CMy3DPlantView::~CMy3DPlantView()
{
}

void InitConsoleWindow()//printf调试窗口  
{
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}

BOOL CMy3DPlantView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	InitConsoleWindow(); //初始化CMD控制台调试窗口 

	return CView::PreCreateWindow(cs);
}

LSystem  gLS;
Direct3D gD3d;
SettingDialog gSettingDia;
void UpdatePlant()
{
	
	std::vector<Vertex::PosColor> vertexs;
	std::vector<UINT> indices;

	gLS.CreatePlant(vertexs, indices, gSettingDia.mIterations);
	gD3d.ReleaseVIBuffer();
	gD3d.CreateVIBuffer(vertexs, indices);
	gD3d.Draw();
	
}

// CMy3DPlantView 绘制
bool isInit = true;

void CMy3DPlantView::OnDraw(CDC* /*pDC*/)
{
	CMy3DPlantDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	if (isInit)
	{
		isInit = false;
		RECT clientR;
		GetClientRect(&clientR);
		gD3d.Init(m_hWnd, clientR.right - clientR.left, clientR.bottom - clientR.top, L"HLSL/pos_color.hlsl");
		std::vector<Vertex::PosColor> vertexs;
		std::vector<UINT> indices;
		int iterations = 7;
		gLS.CreatePlant(vertexs, indices, iterations);
		gD3d.CreateVIBuffer(vertexs, indices);
		gSettingDia.Create(IDD_SETTINGDIALOG);
		gSettingDia.mpSettingOkClick = UpdatePlant;
	}
	gD3d.Draw();
}


// CMy3DPlantView 打印


void CMy3DPlantView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMy3DPlantView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMy3DPlantView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
	
}

void CMy3DPlantView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void CMy3DPlantView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMy3DPlantView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMy3DPlantView 诊断

#ifdef _DEBUG
void CMy3DPlantView::AssertValid() const
{
	CView::AssertValid();
}

void CMy3DPlantView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy3DPlantDoc* CMy3DPlantView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3DPlantDoc)));
	return (CMy3DPlantDoc*)m_pDocument;
}
#endif //_DEBUG







// CMy3DPlantView 消息处理程序


void CMy3DPlantView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (!isInit)
	{
		RECT clientR;
		GetClientRect(&clientR);
		gD3d.OnResize(clientR.right - clientR.left, clientR.bottom - clientR.top);
	}
}

float Clamp(const float& x, const float& low, const float& high)
{
	return x < low ? low : (x > high ? high : x);
}
void CMy3DPlantView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if ((nFlags & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(point.x - gD3d.mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(point.y - gD3d.mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		gD3d.mTheta += dx;
		gD3d.mPhi += dy;

		// Restrict the angle mPhi.
		gD3d.mPhi = Clamp(gD3d.mPhi, 0.1f, XM_PI - 0.1f);
	}
	else if ((nFlags & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(point.x - gD3d.mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(point.y - gD3d.mLastMousePos.y);

		// Update the camera radius based on input.
		gD3d.mRadius += dx - dy;

		// Restrict the radius.
		gD3d.mRadius = Clamp(gD3d.mRadius, 3.0f, 15.0f);
	}
	
	gD3d.mLastMousePos.x = point.x;
	gD3d.mLastMousePos.y = point.y;

	gD3d.Draw();

	CView::OnMouseMove(nFlags, point);
}


void CMy3DPlantView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gD3d.mLastMousePos.x = point.x;
	gD3d.mLastMousePos.y = point.y;

	SetCapture();

	CView::OnMButtonDown(nFlags, point);
}


void CMy3DPlantView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
}


BOOL CMy3DPlantView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	
	gD3d.mRadius += zDelta*0.005f;
	gD3d.Draw();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CMy3DPlantView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nChar >= '1' && nChar <= '9')
	{
	
	}
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CMy3DPlantView::OnSetting()
{
	// TODO:  在此添加命令处理程序代码
	
	gSettingDia.ShowWindow(SW_SHOWNORMAL);
}



afx_msg LRESULT CMy3DPlantView::OnSettingOk(WPARAM wParam, LPARAM lParam)
{
	std::cout << "on setting click ok4" << std::endl;
	return 0;
}
