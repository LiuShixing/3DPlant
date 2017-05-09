// SettingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "3DPlant.h"
#include "SettingDialog.h"
#include "afxdialogex.h"
#include"Direct3D.h"
#include<fstream>

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

	std::vector<std::string> rules(6, "");
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

	DDX_Control(pDX, IDC_EDIT13, mStepAttEdit);
	DDX_Text(pDX, IDC_EDIT13, mLSparamiter.mStepAtt);

	DDX_Control(pDX, IDC_EDIT8, mRotAngleMinEdit);
	DDX_Text(pDX, IDC_EDIT8, mLSparamiter.mRotAngleMin);

	DDX_Control(pDX, IDC_EDIT9, mRotAngleMaxEdit);
	DDX_Text(pDX, IDC_EDIT9, mLSparamiter.mRotAngleMax);

	DDX_Control(pDX, IDC_EDIT10, mTrunkSizeEdit);
	DDX_Text(pDX, IDC_EDIT10, mLSparamiter.mTrunkSize);

	DDX_Control(pDX, IDC_EDIT14, mTrunkSizeAttEdit);
	DDX_Text(pDX, IDC_EDIT14, mLSparamiter.mTrunkSizeAtt);

	DDX_Control(pDX, IDC_EDIT12, mRadiusRateEdit);
	DDX_Text(pDX, IDC_EDIT12, mLSparamiter.mRadiusRate);


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


	DDX_Control(pDX, IDC_CHECK1, mIsTrunkCheck);
	DDX_Control(pDX, IDC_CHECK2, mIsLeaveCheck);
	mIsTrunkCheck.SetCheck(mLSparamiter.mIsTrunk);
	mIsLeaveCheck.SetCheck(mLSparamiter.mIsLeave);


	DDX_Control(pDX, IDC_EDIT15, mToSunFactorEdit);
	DDX_Text(pDX, IDC_EDIT15, mLSparamiter.mSunFactor);

	DDX_Control(pDX, IDC_CHECK3, mIsToSunCheck);
	mIsToSunCheck.SetCheck(mLSparamiter.mIsToSun);
}


BEGIN_MESSAGE_MAP(SettingDialog, CDialog)
	ON_BN_CLICKED(IDC_OK, &SettingDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_OPEN, &SettingDialog::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_SAVE, &SettingDialog::OnBnClickedSave)
	ON_BN_CLICKED(IDC_CHECK1, &SettingDialog::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &SettingDialog::OnBnClickedCheck2)
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
	mStepAttEdit.GetWindowText(text);
	mLSparamiter.mStepAtt = _ttof(text);
	mRotAngleMinEdit.GetWindowText(text);
	mLSparamiter.mRotAngleMin = _ttof(text);
	mRotAngleMaxEdit.GetWindowText(text);
	mLSparamiter.mRotAngleMax = _ttof(text);
	mTrunkSizeEdit.GetWindowText(text);
	mLSparamiter.mTrunkSize = _ttof(text);
	mTrunkSizeAttEdit.GetWindowText(text);
	mLSparamiter.mTrunkSizeAtt = _ttof(text);
	mRadiusRateEdit.GetWindowText(text);
	mLSparamiter.mRadiusRate = _ttof(text);

	mToSunFactorEdit.GetWindowText(text);
	mLSparamiter.mSunFactor = _ttof(text);

	mLSparamiter.mIsToSun = mIsToSunCheck.GetCheck();

	mLSparamiter.mIsTrunk = mIsTrunkCheck.GetCheck();
	mLSparamiter.mIsLeave = mIsLeaveCheck.GetCheck();
	

	mStartEdit.GetWindowText(text);
	mLSparamiter.mStart = text[0];

	mLSparamiter.mRules.clear();
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

extern LSystem  gLS;
extern Direct3D gD3d;
extern SettingDialog gSettingDia;

void SettingDialog::OnBnClickedOpen()
{
	CString  strPathName;
	GetModuleFileName(NULL, strPathName.GetBuffer(256), 256);
	strPathName.ReleaseBuffer(256);
	int nPos = strPathName.ReverseFind('\\');
	strPathName = strPathName.Left(nPos + 1);

	BOOL isOpen = TRUE;     //是否打开(否则为保存)  
	CString defaultDir = strPathName;   //默认打开的文件路径  
	CString fileName = L"";         //默认打开的文件名  
	CString filter = L"文件 (*.plant)|*.plant||";   //文件过虑的类型  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\test.doc";
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir + "\\test.doc";
    if (result == IDOK) {
		filePath = openFileDlg.GetPathName();

		std::ifstream is(filePath);

		std::string ignore;
		is >> ignore >> mLSparamiter.mIterations
			>> ignore >> mLSparamiter.mStepMin
			>> ignore >> mLSparamiter.mStepMax
			>> ignore >> mLSparamiter.mStepAtt
			>> ignore >> mLSparamiter.mRotAngleMin
			>> ignore >> mLSparamiter.mRotAngleMax
			>> ignore >> mLSparamiter.mTrunkSize
			>> ignore >> mLSparamiter.mTrunkSizeAtt
			>> ignore >> mLSparamiter.mRadiusRate
			>> ignore >> mLSparamiter.mLeaveSize
			>> ignore >> mLSparamiter.mStart
			>> ignore >> mLSparamiter.mIsTrunk
			>> ignore >> mLSparamiter.mIsLeave
			>> ignore >> mLSparamiter.mIsToSun
			>> ignore >> mLSparamiter.mSunFactor;

		int rulesCount;
		is >> ignore >> rulesCount;
		mLSparamiter.mRules.clear();
		for (int i = 0; i < rulesCount; i++)
		{
			char k;
			is >> ignore >> k;
			int count;
			is >> count;
			std::vector<std::string> vs;
			for (int j = 0; j < count; j++)
			{
				std::string s;
				is >> s;
				vs.push_back(s);
			}
			mLSparamiter.mRules.insert(std::make_pair(k, vs));
		}

		int vertexsCount;
		is >> ignore >> vertexsCount;
		gLS.mVertexs.clear();
		gLS.mVertexs.resize(vertexsCount);
		for (int i = 0; i < vertexsCount; i++)
		{
			XMFLOAT3 p;
			XMFLOAT4 c;
			is >> p.x >> p.y >> p.z;
			is >> c.x >> c.y >> c.z >> c.w;
			gLS.mVertexs[i].pos = p;
			gLS.mVertexs[i].color = c;
		}

		int indexCount;
		is >> ignore >> indexCount;
		gLS.mIndices.resize(indexCount);
		for (int i = 0; i < indexCount; i++)
		{
			is >> gLS.mIndices[i];
		}
	
		int trunkCount;
		is >> ignore >> trunkCount;
		gLS.mTrunks.resize(trunkCount);
		for (int i = 0; i < trunkCount; i++)
		{
			XMFLOAT3 p;
			XMFLOAT3 r;
			is >> p.x >> p.y >> p.z;
			is >> r.x >> r.y >> r.z;
			gLS.mTrunks[i].pos = p;
			gLS.mTrunks[i].rotAxis = r;
			is >> gLS.mTrunks[i].sizeScal
				>> gLS.mTrunks[i].scalY
				>> gLS.mTrunks[i].angle;
		}
		
		int leafCount;
		is >> ignore >> leafCount;
		gLS.mLeaves.resize(leafCount);
		for (int i = 0; i < leafCount; i++)
		{
			XMFLOAT3 p;
			XMFLOAT3 r;
			is >> p.x >> p.y >> p.z;
			is >> r.x >> r.y >> r.z;
			gLS.mLeaves[i].pos = p;
			gLS.mLeaves[i].rotAxis = r;
			is >> gLS.mLeaves[i].scal
				>> gLS.mLeaves[i].angle;

		}

		is.close();
		UpdateData(FALSE);
		mDrawSavedPlant();
	}
}


void SettingDialog::OnBnClickedSave()
{
	CString  strPathName;
	GetModuleFileName(NULL, strPathName.GetBuffer(256), 256);
	strPathName.ReleaseBuffer(256);
	int nPos = strPathName.ReverseFind('\\');
	strPathName = strPathName.Left(nPos + 1);

	BOOL isOpen = FALSE;        //是否打开(否则为保存)  
	CString defaultDir = strPathName;   //默认打开的文件路径  
	CString fileName = L"file.plant";         //默认打开的文件名  
	CString filter = L"文件 (*.plant)|*.plant||";   //文件过虑的类型  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\test.doc";
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir + "\\" + fileName;
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();

		std::ofstream os(filePath);

		using namespace std;
		os << "Iterations: " << mLSparamiter.mIterations << endl
			<< "StepMin: " << mLSparamiter.mStepMin << endl
			<< "StepMax: " << mLSparamiter.mStepMax << endl
			<< "StepAtt: " << mLSparamiter.mStepAtt << endl
			<< "RotAngleMin: " << mLSparamiter.mRotAngleMin << endl
			<< "RotAngleMax: " << mLSparamiter.mRotAngleMax << endl
			<< "TrunkSize: " << mLSparamiter.mTrunkSize << endl
			<< "TrunkSizeAtt: " << mLSparamiter.mTrunkSizeAtt << endl
			<< "RadiusRate: " << mLSparamiter.mRadiusRate << endl
			<< "LeaveSize: " << mLSparamiter.mLeaveSize << endl
			<< "Start: " << mLSparamiter.mStart << endl
			<< "IsTrunk: " << mLSparamiter.mIsTrunk << endl
			<< "IsLeave: " << mLSparamiter.mIsLeave << endl
			<< "IsToSun: " << mLSparamiter.mIsToSun << endl
			<< "SunFactor: " << mLSparamiter.mSunFactor << endl;
		os << endl;
		os << "RulesCount: "<<mLSparamiter.mRules.size() << endl;
		for (std::map<char, std::vector<std::string> >::iterator it = mLSparamiter.mRules.begin(); it != mLSparamiter.mRules.end(); ++it)
		{
			char k = it->first;
			vector<string> vs = it->second;

			os << "key: " << k << endl;
			os << vs.size() << endl;
			for (int i = 0; i < vs.size(); i++)
				os << vs[i] << endl;
			os << endl;
		}


		os << "vertexsCount: " << gLS.mVertexs.size() << endl;
		for (int i = 0; i < gLS.mVertexs.size(); i++)
		{
			XMFLOAT3 p = gLS.mVertexs[i].pos;
			XMFLOAT4 c = gLS.mVertexs[i].color;
			os << p.x << " " << p.y << " " << p.z << endl;
			os << c.x << " " << c.y << " " << c.z << " " << c.w << endl;
		}

		os << "indexCount: " << gLS.mIndices.size() << endl;
		for (int i = 0; i < gLS.mIndices.size(); i++)
			os << gLS.mIndices[i] << " ";
		os << endl;

		os << "trunkCount: " << gLS.mTrunks.size() << endl;
		for (int i = 0; i < gLS.mTrunks.size(); i++)
		{
			XMFLOAT3 p = gLS.mTrunks[i].pos;
			XMFLOAT3 r = gLS.mTrunks[i].rotAxis;
			os << p.x << " " << p.y << " " << p.z << endl;
			os << r.x << " " << r.y << " " << r.z << endl;
			os << gLS.mTrunks[i].sizeScal << endl
				<< gLS.mTrunks[i].scalY << endl
				<< gLS.mTrunks[i].angle << endl;
		}
		os << endl;

		os << "leafCount: " << gLS.mLeaves.size() << endl;
		for (int i = 0; i < gLS.mLeaves.size(); i++)
		{
			XMFLOAT3 p = gLS.mLeaves[i].pos;
			XMFLOAT3 r = gLS.mLeaves[i].rotAxis;
			os << p.x << " " << p.y << " " << p.z << endl;
			os << r.x << " " << r.y << " " << r.z << endl;
			os << gLS.mLeaves[i].scal << endl
				<< gLS.mLeaves[i].angle << endl;
		}
		os << endl;

	
		os.close();
	}
}
/*



*/

void SettingDialog::OnBnClickedCheck1()
{
	mLSparamiter.mIsTrunk = mIsTrunkCheck.GetCheck();
	gD3d.Draw(gLS.mTrunks, gLS.mLeaves, gSettingDia.mLSparamiter.mIsTrunk, gSettingDia.mLSparamiter.mIsLeave);
}


void SettingDialog::OnBnClickedCheck2()
{
	mLSparamiter.mIsLeave = mIsLeaveCheck.GetCheck();
	gD3d.Draw(gLS.mTrunks, gLS.mLeaves, gSettingDia.mLSparamiter.mIsTrunk, gSettingDia.mLSparamiter.mIsLeave);
}
