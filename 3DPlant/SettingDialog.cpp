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
{

}

SettingDialog::~SettingDialog()
{
	
}

void SettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	std::vector<std::string> rules(6,"");
	int index = 0;
	for (std::map<char, std::vector<std::string> >::iterator it = mLSparamiter.mRules.begin(); it != mLSparamiter.mRules.end(); ++it)
	{
		char k = it->first;
		for (int i = 0; i < it->second.size(); i++)
		{
			rules[index] = k;
			rules[index] += "=";
			rules[index] += (it->second)[i];
			index++;
		}
	}

	DDX_Control(pDX, IDC_ITERATIONS, mIterationsEdit);
	DDX_Text(pDX, IDC_ITERATIONS, mLSparamiter.mIterations);

	DDX_Control(pDX, IDC_EDIT1, mStepMinEdit);
	DDX_Text(pDX, IDC_EDIT1, mLSparamiter.mStepMin);

	DDX_Control(pDX, IDC_EDIT7, mStepMaxEdit);
	DDX_Text(pDX, IDC_EDIT7, mLSparamiter.mStepMax);

	DDX_Control(pDX, IDC_EDIT8, mRotAngleMinEdit);
	DDX_Text(pDX, IDC_EDIT8, mLSparamiter.mRotAngleMin);

	DDX_Control(pDX, IDC_EDIT9, mRotAngleMaxEdit);
	DDX_Text(pDX, IDC_EDIT9, mLSparamiter.mRotAngleMax);

	DDX_Control(pDX, IDC_EDIT10, mTrunkSizeEdit);
	DDX_Text(pDX, IDC_EDIT10, mLSparamiter.mTrunkSize);

	CString Start(mLSparamiter.mStart);
	DDX_Control(pDX, IDC_EDIT11, mStartEdit);
	DDX_Text(pDX, IDC_EDIT11, Start);

	std::wstring ws;

	DDX_Control(pDX, IDC_EDIT2, mRule1Edit);
	ws.resize(rules[0].size());
	std::copy(rules[0].begin(), rules[0].end(), ws.begin());
	CString cs1(ws.c_str());
	DDX_Text(pDX, IDC_EDIT2, cs1);

	DDX_Control(pDX, IDC_EDIT3, mRule2Edit);
	ws.resize(rules[1].size());
	std::copy(rules[1].begin(), rules[1].end(), ws.begin());
	CString cs2(ws.c_str());
	DDX_Text(pDX, IDC_EDIT3, cs2);

	DDX_Control(pDX, IDC_EDIT4, mRule3Edit);
	ws.resize(rules[2].size());
	std::copy(rules[2].begin(), rules[2].end(), ws.begin());
	CString cs3(ws.c_str());
	DDX_Text(pDX, IDC_EDIT4, cs3);

	DDX_Control(pDX, IDC_EDIT5, mRule4Edit);
	ws.resize(rules[3].size());
	std::copy(rules[3].begin(), rules[3].end(), ws.begin());
	CString cs4(ws.c_str());
	DDX_Text(pDX, IDC_EDIT5, cs4);

	DDX_Control(pDX, IDC_EDIT6, mRule5Edit);
	ws.resize(rules[4].size());
	std::copy(rules[4].begin(), rules[4].end(), ws.begin());
	CString cs5(ws.c_str());
	DDX_Text(pDX, IDC_EDIT6, cs5);

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
	else if (s.length() != 0)
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
	if (mLSparamiter.mStart != ' ' && mLSparamiter.mRules.size() > 0)
	{
		mpSettingOkClick();
	}
	else
	{
		MessageBox(0, L" 不符合文法规则！", 0);
	}
}
