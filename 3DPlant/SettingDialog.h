#pragma once

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include "afxwin.h"

// SettingDialog 对话框

class SettingDialog : public CDialog
{
	DECLARE_DYNAMIC(SettingDialog)

public:
	SettingDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SettingDialog();

// 对话框数据
	enum { IDD = IDD_SETTINGDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT mIterations;
	afx_msg void OnBnClickedOk();

	void (*mpSettingOkClick)();
	CEdit mIterationsEdit;
	CEdit mStepMinEdit;
	float mStepMin;
	CEdit mStepMaxEdit;
	float mStepMax;
	CEdit mRotAngleMinEdit;
	float mRotAngleMin;
	CEdit mRotAngleMaxEdit;
	float mRotAngleMax;
	CEdit mTrunkSizeEdit;
	float mTrunkSize;
	CEdit mRule1Edit;
	CEdit mRule2Edit;
	CEdit mRule3Edit;
	CEdit mRule4Edit;
	CEdit mRule5Edit;

	std::map<char, std::vector<std::string> > mRules;
};
