#pragma once
#include "afxwin.h"


// CHelpDial �Ի���

class CHelpDial : public CDialog
{
	DECLARE_DYNAMIC(CHelpDial)

public:
	CHelpDial(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHelpDial();

// �Ի�������
	enum { IDD = IDD_HELPDIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit mText;
};
