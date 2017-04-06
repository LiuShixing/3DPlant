
// 3DPlantView.cpp : CMy3DPlantView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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
	// ��׼��ӡ����
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

// CMy3DPlantView ����/����

CMy3DPlantView::CMy3DPlantView()
{
	// TODO:  �ڴ˴���ӹ������

}

CMy3DPlantView::~CMy3DPlantView()
{
}

void InitConsoleWindow()//printf���Դ���  
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
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	InitConsoleWindow(); //��ʼ��CMD����̨���Դ��� 

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

// CMy3DPlantView ����
bool isInit = true;

void CMy3DPlantView::OnDraw(CDC* /*pDC*/)
{
	CMy3DPlantDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
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


// CMy3DPlantView ��ӡ


void CMy3DPlantView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMy3DPlantView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CMy3DPlantView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
	
}

void CMy3DPlantView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
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


// CMy3DPlantView ���

#ifdef _DEBUG
void CMy3DPlantView::AssertValid() const
{
	CView::AssertValid();
}

void CMy3DPlantView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy3DPlantDoc* CMy3DPlantView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3DPlantDoc)));
	return (CMy3DPlantDoc*)m_pDocument;
}
#endif //_DEBUG







// CMy3DPlantView ��Ϣ�������


void CMy3DPlantView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gD3d.mLastMousePos.x = point.x;
	gD3d.mLastMousePos.y = point.y;

	SetCapture();

	CView::OnMButtonDown(nFlags, point);
}


void CMy3DPlantView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
}


BOOL CMy3DPlantView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	gD3d.mRadius += zDelta*0.005f;
	gD3d.Draw();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CMy3DPlantView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nChar >= '1' && nChar <= '9')
	{
	
	}
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CMy3DPlantView::OnSetting()
{
	// TODO:  �ڴ���������������
	
	gSettingDia.ShowWindow(SW_SHOWNORMAL);
}



afx_msg LRESULT CMy3DPlantView::OnSettingOk(WPARAM wParam, LPARAM lParam)
{
	std::cout << "on setting click ok4" << std::endl;
	return 0;
}
