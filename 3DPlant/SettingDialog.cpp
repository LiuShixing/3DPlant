// SettingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "3DPlant.h"
#include "SettingDialog.h"
#include "afxdialogex.h"

// SettingDialog 对话框

IMPLEMENT_DYNAMIC(SettingDialog, CDialog)

SettingDialog::SettingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SettingDialog::IDD, pParent)
	, mIterations(1)
	, mStepMin(0)
	, mStepMax(0)
	, mRotAngleMin(0)
	, mRotAngleMax(0)
	, mTrunkSize(0)
{

}

SettingDialog::~SettingDialog()
{
	
}

void SettingDialog::DoDataExchange(CDataExchange* pDX)
{
	std::cout << "change" << std::endl;
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ITERATIONS, mIterations);
	DDV_MinMaxUInt(pDX, mIterations, 1, 1000);
	DDX_Control(pDX, IDC_ITERATIONS, mIterationsEdit);
	DDX_Control(pDX, IDC_EDIT1, mStepMinEdit);
	DDX_Control(pDX, IDC_EDIT7, mStepMaxEdit);
	DDX_Control(pDX, IDC_EDIT8, mRotAngleMinEdit);
	DDX_Control(pDX, IDC_EDIT9, mRotAngleMaxEdit);
	DDX_Control(pDX, IDC_EDIT10, mTrunkSizeEdit);
	DDX_Control(pDX, IDC_EDIT2, mRule1Edit);
	DDX_Control(pDX, IDC_EDIT3, mRule2Edit);
	DDX_Control(pDX, IDC_EDIT4, mRule3Edit);
	DDX_Control(pDX, IDC_EDIT5, mRule4Edit);
	DDX_Control(pDX, IDC_EDIT6, mRule5Edit);
}


BEGIN_MESSAGE_MAP(SettingDialog, CDialog)
	ON_BN_CLICKED(IDC_OK, &SettingDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// SettingDialog 消息处理程序


void SettingDialog::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

	CString text;
	mIterationsEdit.GetWindowText(text);
	mIterations = _ttoi(text);
	
	mStepMinEdit.GetWindowText(text);
	mStepMin = _ttof(text);
	mStepMaxEdit.GetWindowText(text);
	mStepMax = _ttof(text);
	mRotAngleMinEdit.GetWindowText(text);
	mRotAngleMin = _ttof(text);
	mRotAngleMaxEdit.GetWindowText(text);
	mRotAngleMax = _ttof(text);
	mTrunkSizeEdit.GetWindowText(text);
	mTrunkSize = _ttof(text);

	mRule1Edit.GetWindowText(text);
	text.Remove(' ');
	if (text.GetLength() >= 2)
	{
		char k = text[0];
		if (text[1] == '=')
		{
			
		}
	}
	
	mRule2Edit.GetWindowText(text);
	mRule3Edit.GetWindowText(text);
	mRule4Edit.GetWindowText(text);
	mRule5Edit.GetWindowText(text);

	//--
	mpSettingOkClick();
}
