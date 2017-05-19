
// 3DPlantView.h : CMy3DPlantView ��Ľӿ�
//

#pragma once
#include"Direct3D.h"
#include"LSystem.h"
#include"SettingDialog.h"
#include "HelpDial.h"

class CMy3DPlantView : public CView
{
protected: // �������л�����
	CMy3DPlantView();
	DECLARE_DYNCREATE(CMy3DPlantView)

// ����
public:
	CMy3DPlantDoc* GetDocument() const;

// ����
public:
//	LSystem  mLS;
//	Direct3D mD3d;
//	SettingDialog mSettingDia;
// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CMy3DPlantView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//	void UpdatePlant();
protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetting();
protected:
//	afx_msg LRESULT OnSettingOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSettingOk(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void On32773();
};

#ifndef _DEBUG  // 3DPlantView.cpp �еĵ��԰汾
inline CMy3DPlantDoc* CMy3DPlantView::GetDocument() const
   { return reinterpret_cast<CMy3DPlantDoc*>(m_pDocument); }
#endif

