#pragma once

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include "afxwin.h"
#include"LSystem.h"

// SettingDialog �Ի���

class SettingDialog : public CDialog
{
	DECLARE_DYNAMIC(SettingDialog)

public:
	SettingDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SettingDialog();

// �Ի�������
	enum { IDD = IDD_SETTINGDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	void AddRules(CString& text);

	void (*mpSettingOkClick)();
	void(*mDrawSavedPlant)();
	CEdit mIterationsEdit;
	CEdit mStepMinEdit;
	CEdit mStepMaxEdit;
	CEdit mStepAttEdit;
	CEdit mRotAngleMinEdit;
	CEdit mRotAngleMaxEdit;
	CEdit mTrunkSizeEdit;
	CEdit mRule1Edit;
	CEdit mRule2Edit;
	CEdit mRule3Edit;
	CEdit mRule4Edit;
	CEdit mRule5Edit;

	LSparameter mLSparamiter;
	CEdit mStartEdit;
	CEdit mRadiusRateEdit;
	CButton mIsTrunkCheck;
	CButton mIsLeaveCheck;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedSave();
	
	CEdit mTrunkSizeAttEdit;
	CEdit mToSunFactorEdit;
	CButton mIsToSunCheck;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
};
