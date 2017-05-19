#pragma once
#include "afxwin.h"


// CHelpDial 对话框

class CHelpDial : public CDialog
{
	DECLARE_DYNAMIC(CHelpDial)

public:
	CHelpDial(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHelpDial();

// 对话框数据
	enum { IDD = IDD_HELPDIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit mText;
};
