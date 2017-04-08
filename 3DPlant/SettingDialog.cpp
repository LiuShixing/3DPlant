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
	DDX_Control(pDX, IDC_EDIT11, mStartEdit);
}


BEGIN_MESSAGE_MAP(SettingDialog, CDialog)
	ON_BN_CLICKED(IDC_OK, &SettingDialog::OnBnClickedOk)
END_MESSAGE_MAP()


void SettingDialog::AddRules(CString& text)
{
	text.Remove(' ');
	std::wstring ws(text.GetBuffer(text.GetLength()));
	std::string s(ws.length(), ' ');
	std::copy(ws.begin(), ws.end(), s.begin());

	if (s.length() >= 2 && s[1] == '=')
	{
		char k = s[0];
		std::string v(s.begin() + 2, s.end());
		std::map<char, std::vector<std::string> >::iterator it = mLSparamiter.mRules.find(k);
		if (it != mLSparamiter.mRules.end())
		{
			(it->second).push_back(v);
		}
		else
		{
			std::vector<std::string> vs;
			vs.push_back(v);
			mLSparamiter.mRules.insert(std::make_pair(k, vs));
		}
	}
	else
	{
		MessageBox(0, text+L" 不符合文法规则！", 0);
	}
}
// SettingDialog 消息处理程序

void SettingDialog::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

	CString text;
	mIterationsEdit.GetWindowText(text);
	mLSparamiter.mIterations = _ttoi(text);
	
	mStepMinEdit.GetWindowText(text);
	mLSparamiter.mStepMin = _ttof(text);
	mStepMaxEdit.GetWindowText(text);
	mLSparamiter.mStepMax = _ttof(text);
	mRotAngleMinEdit.GetWindowText(text);
	mLSparamiter.mRotAngleMin = _ttof(text);
	mRotAngleMaxEdit.GetWindowText(text);
	mLSparamiter.mRotAngleMax = _ttof(text);
	mTrunkSizeEdit.GetWindowText(text);
	mLSparamiter.mTrunkSize = _ttof(text);

	mStartEdit.GetWindowText(text);
	mLSparamiter.mStart = text[0];
	mRule1Edit.GetWindowText(text);
	AddRules(text);
	mRule2Edit.GetWindowText(text);
	AddRules(text);
	mRule3Edit.GetWindowText(text);
	AddRules(text);
	mRule4Edit.GetWindowText(text);
	AddRules(text);
	mRule5Edit.GetWindowText(text);
	AddRules(text);

	//--
	mpSettingOkClick();
}
