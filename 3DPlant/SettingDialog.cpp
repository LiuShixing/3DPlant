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
	, mLeafOrder(0)
	, mLeafOrder0Ctrl(FALSE)
	, mLeafOrder1Ctrl(0)
{

}

SettingDialog::~SettingDialog()
{
	
}
extern LSystem  gLS;
extern Direct3D gD3d;
extern SettingDialog gSettingDia;
extern PlantData    gPlantData;
void SettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	std::vector<std::string> rules(6, "");
	float probs[6] = {0,0,0,0,0,0};
	int index = 0;
	for (std::map<char, std::vector<RuleRight> >::iterator it = gLS.mParam.mRules.begin(); it != gLS.mParam.mRules.end(); ++it)
	{
		char k = it->first;
		for (int i = 0; i < it->second.size(); i++)
		{
			rules[index] = k;
			rules[index] += "=";
			rules[index] += (it->second)[i].production;
			probs[index] = (it->second)[i].probability;
			index++;
		}
	}

	DDX_Control(pDX, IDC_ITERATIONS, mIterationsEdit);
	DDX_Text(pDX, IDC_ITERATIONS, gLS.mParam.mIterations);

	DDX_Control(pDX, IDC_EDIT1, mStepMinEdit);
	DDX_Text(pDX, IDC_EDIT1, gLS.mParam.mStepMin);

	DDX_Control(pDX, IDC_EDIT7, mStepMaxEdit);
	DDX_Text(pDX, IDC_EDIT7, gLS.mParam.mStepMax);

	DDX_Control(pDX, IDC_EDIT13, mStepAttEdit);
	DDX_Text(pDX, IDC_EDIT13, gLS.mParam.mStepAtt);

	DDX_Control(pDX, IDC_EDIT8, mRotAngleMinEdit);
	DDX_Text(pDX, IDC_EDIT8, gLS.mParam.mRotAngleMin);

	DDX_Control(pDX, IDC_EDIT9, mRotAngleMaxEdit);
	DDX_Text(pDX, IDC_EDIT9, gLS.mParam.mRotAngleMax);

	DDX_Control(pDX, IDC_EDIT10, mTrunkSizeEdit);
	DDX_Text(pDX, IDC_EDIT10, gLS.mParam.mTrunkSize);

	DDX_Control(pDX, IDC_EDIT14, mTrunkSizeAttEdit);
	DDX_Text(pDX, IDC_EDIT14, gLS.mParam.mTrunkSizeAtt);

	DDX_Control(pDX, IDC_EDIT12, mRadiusRateEdit);
	DDX_Text(pDX, IDC_EDIT12, gLS.mParam.mRadiusRate);

	DDX_Control(pDX, IDC_EDIT16, mLeafSizeEdit);
	DDX_Text(pDX, IDC_EDIT16, gLS.mParam.mLeaveSize);

	DDX_Control(pDX, IDC_EDIT17, mProb1Edit);
	DDX_Text(pDX, IDC_EDIT17, probs[0]);
	DDX_Control(pDX, IDC_EDIT18, mProb2Edit);
	DDX_Text(pDX, IDC_EDIT18, probs[1]);
	DDX_Control(pDX, IDC_EDIT19, mProb3Edit);
	DDX_Text(pDX, IDC_EDIT19, probs[2]);
	DDX_Control(pDX, IDC_EDIT20, mProb4Edit);
	DDX_Text(pDX, IDC_EDIT20, probs[3]);
	DDX_Control(pDX, IDC_EDIT21, mProb5Edit);
	DDX_Text(pDX, IDC_EDIT21, probs[4]);

	CString Start(gLS.mParam.mStart);
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
	mIsTrunkCheck.SetCheck(gLS.mParam.mIsTrunk);
	mIsLeaveCheck.SetCheck(gLS.mParam.mIsLeave);


	DDX_Control(pDX, IDC_EDIT15, mToSunFactorEdit);
	DDX_Text(pDX, IDC_EDIT15, gLS.mParam.mSunFactor);

	DDX_Control(pDX, IDC_CHECK3, mIsToSunCheck);
	mIsToSunCheck.SetCheck(gLS.mParam.mIsToSun);

	mLeafOrder = gLS.mParam.mLeafOrder;

	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);//没选上
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);//没选上
	((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);//没选上
	if (mLeafOrder == 0)
	{
		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);//选上
	}
	else if (mLeafOrder == 1)
	{
		((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);//选上
	}
	else if (mLeafOrder == 2)
	{
		((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);//选上
	}
	
}


BEGIN_MESSAGE_MAP(SettingDialog, CDialog)
	ON_BN_CLICKED(IDC_OK, &SettingDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_OPEN, &SettingDialog::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_SAVE, &SettingDialog::OnBnClickedSave)
	ON_BN_CLICKED(IDC_CHECK1, &SettingDialog::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &SettingDialog::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_RADIO1, &SettingDialog::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &SettingDialog::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &SettingDialog::OnBnClickedRadio3)
END_MESSAGE_MAP()


void SettingDialog::AddRules(CString& text,float prob)
{
	text.Remove(' ');
	std::wstring ws(text.GetBuffer(text.GetLength()));
	std::string s(ws.length(), ' ');
	std::copy(ws.begin(), ws.end(), s.begin());

	if (s.length() >= 2 && s[1] == '=')
	{
		char k = s[0];
		RuleRight R;
		std::string v(s.begin() + 2, s.end());
		R.production = v;
		R.probability = prob;
		std::map<char, std::vector<RuleRight> >::iterator it = gLS.mParam.mRules.find(k);
		if (it != gLS.mParam.mRules.end())
		{
			(it->second).push_back(R);
		}
		else
		{
			std::vector<RuleRight> vs;
			vs.push_back(R);
			gLS.mParam.mRules.insert(std::make_pair(k, vs));
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
	gLS.mParam.mIterations = _ttoi(text);
	
	mStepMinEdit.GetWindowText(text); 
	gLS.mParam.mStepMin = _ttof(text);
	mStepMaxEdit.GetWindowText(text);
	gLS.mParam.mStepMax = _ttof(text);
	mStepAttEdit.GetWindowText(text);
	gLS.mParam.mStepAtt = _ttof(text);
	mRotAngleMinEdit.GetWindowText(text);
	gLS.mParam.mRotAngleMin = _ttof(text);
	mRotAngleMaxEdit.GetWindowText(text);
	gLS.mParam.mRotAngleMax = _ttof(text);
	mTrunkSizeEdit.GetWindowText(text);
	gLS.mParam.mTrunkSize = _ttof(text);
	mTrunkSizeAttEdit.GetWindowText(text);
	gLS.mParam.mTrunkSizeAtt = _ttof(text);
	mRadiusRateEdit.GetWindowText(text);
	gLS.mParam.mRadiusRate = _ttof(text);

	mToSunFactorEdit.GetWindowText(text);
	gLS.mParam.mSunFactor = _ttof(text);

	mLeafSizeEdit.GetWindowText(text);
	gLS.mParam.mLeaveSize = _ttof(text);

	mProb1Edit.GetWindowText(text);
	float prob1 = _ttof(text);
	mProb2Edit.GetWindowText(text);
	float prob2 = _ttof(text);
	mProb3Edit.GetWindowText(text);
	float prob3 = _ttof(text);
	mProb4Edit.GetWindowText(text);
	float prob4 = _ttof(text);
	mProb5Edit.GetWindowText(text);
	float prob5 = _ttof(text);

	gLS.mParam.mIsToSun = mIsToSunCheck.GetCheck();

	gLS.mParam.mIsTrunk = mIsTrunkCheck.GetCheck();
	gLS.mParam.mIsLeave = mIsLeaveCheck.GetCheck();

	gLS.mParam.mLeafOrder = mLeafOrder;
	
//	std::cout << "mLSparamiter.mLeafOrder " << gLS.mParam.mLeafOrder << std::endl;
	mStartEdit.GetWindowText(text);
	gLS.mParam.mStart = text[0];

	gLS.mParam.mRules.clear();
	mRule1Edit.GetWindowText(text);
	AddRules(text, prob1);
	mRule2Edit.GetWindowText(text);
	AddRules(text, prob2);
	mRule3Edit.GetWindowText(text);
	AddRules(text, prob3);
	mRule4Edit.GetWindowText(text);
	AddRules(text, prob4);
	mRule5Edit.GetWindowText(text);
	AddRules(text, prob5);

	//--
	if (gLS.mParam.mStart != ' ' && gLS.mParam.mRules.size() > 0)
	{
		if (gLS.mParam.CheckParam())
		{
			mpSettingOkClick();
		}
		else
		{
			MessageBox(0, L" 参数不合法！", 0);
		}
	}
	else
	{
		MessageBox(0, L" 不符合文法规则！", 0);
	}
}



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
		is >> ignore >> gLS.mParam.mIterations
			>> ignore >> gLS.mParam.mStepMin
			>> ignore >> gLS.mParam.mStepMax
			>> ignore >> gLS.mParam.mStepAtt
			>> ignore >> gLS.mParam.mRotAngleMin
			>> ignore >> gLS.mParam.mRotAngleMax
			>> ignore >> gLS.mParam.mTrunkSize
			>> ignore >> gLS.mParam.mTrunkSizeAtt
			>> ignore >> gLS.mParam.mRadiusRate
			>> ignore >> gLS.mParam.mLeaveSize
			>> ignore >> gLS.mParam.mStart
			>> ignore >> gLS.mParam.mIsTrunk
			>> ignore >> gLS.mParam.mIsLeave
			>> ignore >> gLS.mParam.mIsToSun
			>> ignore >> gLS.mParam.mSunFactor
			>> ignore >> gLS.mParam.mLeafOrder;

		int rulesCount;
		is >> ignore >> rulesCount;
		gLS.mParam.mRules.clear();
		for (int i = 0; i < rulesCount; i++)
		{
			char k;
			is >> ignore >> k;
			int count;
			is >> count;
			std::vector<RuleRight> vs;
			for (int j = 0; j < count; j++)
			{
				RuleRight s;
				is >> s.production;
				is >> s.probability;
				vs.push_back(s);
			}
			gLS.mParam.mRules.insert(std::make_pair(k, vs));
		}

		int vertexsCount;
		is >> ignore >> vertexsCount;
		gPlantData.mVertexs.clear();
		gPlantData.mVertexs.resize(vertexsCount);
		for (int i = 0; i < vertexsCount; i++)
		{
			XMFLOAT3 p;
			XMFLOAT4 c;
			is >> p.x >> p.y >> p.z;
			is >> c.x >> c.y >> c.z >> c.w;
			gPlantData.mVertexs[i].pos = p;
			gPlantData.mVertexs[i].color = c;
		}

		int indexCount;
		is >> ignore >> indexCount;
		gPlantData.mIndices.resize(indexCount);
		for (int i = 0; i < indexCount; i++)
		{
			is >> gPlantData.mIndices[i];
		}
	
		int trunkCount;
		is >> ignore >> trunkCount;
		gPlantData.mTrunks.resize(trunkCount);
		for (int i = 0; i < trunkCount; i++)
		{
			XMFLOAT3 p;
			XMFLOAT3 r;
			is >> p.x >> p.y >> p.z;
			is >> r.x >> r.y >> r.z;
			gPlantData.mTrunks[i].pos = p;
			gPlantData.mTrunks[i].rotAxis = r;
			is >> gPlantData.mTrunks[i].sizeScal
				>> gPlantData.mTrunks[i].scalY
				>> gPlantData.mTrunks[i].angle;
		}
		
		int leafCount;
		is >> ignore >> leafCount;
		gPlantData.mLeaves.resize(leafCount);
		for (int i = 0; i < leafCount; i++)
		{
			XMFLOAT3 p;
			XMFLOAT3 r;
			is >> p.x >> p.y >> p.z;
			is >> r.x >> r.y >> r.z;
			gPlantData.mLeaves[i].pos = p;
			gPlantData.mLeaves[i].rotAxis = r;
			is >> gPlantData.mLeaves[i].scal
				>> gPlantData.mLeaves[i].angle
				>> gPlantData.mLeaves[i].rotY;

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
		os << "Iterations: " << gLS.mParam.mIterations << endl
			<< "StepMin: " << gLS.mParam.mStepMin << endl
			<< "StepMax: " << gLS.mParam.mStepMax << endl
			<< "StepAtt: " << gLS.mParam.mStepAtt << endl
			<< "RotAngleMin: " << gLS.mParam.mRotAngleMin << endl
			<< "RotAngleMax: " << gLS.mParam.mRotAngleMax << endl
			<< "TrunkSize: " << gLS.mParam.mTrunkSize << endl
			<< "TrunkSizeAtt: " << gLS.mParam.mTrunkSizeAtt << endl
			<< "RadiusRate: " << gLS.mParam.mRadiusRate << endl
			<< "LeaveSize: " << gLS.mParam.mLeaveSize << endl
			<< "Start: " << gLS.mParam.mStart << endl
			<< "IsTrunk: " << gLS.mParam.mIsTrunk << endl
			<< "IsLeave: " << gLS.mParam.mIsLeave << endl
			<< "IsToSun: " << gLS.mParam.mIsToSun << endl
			<< "SunFactor: " << gLS.mParam.mSunFactor << endl
			<< "LeafOrder: " << gLS.mParam.mLeafOrder << endl;
		os << endl;
		os << "RulesCount: " << gLS.mParam.mRules.size() << endl;
		for (std::map<char, std::vector<RuleRight> >::iterator it = gLS.mParam.mRules.begin(); it != gLS.mParam.mRules.end(); ++it)
		{
			char k = it->first;
			vector<RuleRight> vs = it->second;

			os << "key: " << k << endl;
			os << vs.size() << endl;
			for (int i = 0; i < vs.size(); i++)
			{
				os << vs[i].production << endl;
				os << vs[i].probability << endl;
			}
			os << endl;
		}


		os << "vertexsCount: " << gPlantData.mVertexs.size() << endl;
		for (int i = 0; i < gPlantData.mVertexs.size(); i++)
		{
			XMFLOAT3 p = gPlantData.mVertexs[i].pos;
			XMFLOAT4 c = gPlantData.mVertexs[i].color;
			os << p.x << " " << p.y << " " << p.z << endl;
			os << c.x << " " << c.y << " " << c.z << " " << c.w << endl;
		}

		os << "indexCount: " << gPlantData.mIndices.size() << endl;
		for (int i = 0; i < gPlantData.mIndices.size(); i++)
			os << gPlantData.mIndices[i] << " ";
		os << endl;

		os << "trunkCount: " << gPlantData.mTrunks.size() << endl;
		for (int i = 0; i < gPlantData.mTrunks.size(); i++)
		{
			XMFLOAT3 p = gPlantData.mTrunks[i].pos;
			XMFLOAT3 r = gPlantData.mTrunks[i].rotAxis;
			os << p.x << " " << p.y << " " << p.z << endl;
			os << r.x << " " << r.y << " " << r.z << endl;
			os << gPlantData.mTrunks[i].sizeScal << endl
				<< gPlantData.mTrunks[i].scalY << endl
				<< gPlantData.mTrunks[i].angle << endl;
		}
		os << endl;

		os << "leafCount: " << gPlantData.mLeaves.size() << endl;
		for (int i = 0; i < gPlantData.mLeaves.size(); i++)
		{
			XMFLOAT3 p = gPlantData.mLeaves[i].pos;
			XMFLOAT3 r = gPlantData.mLeaves[i].rotAxis;
			os << p.x << " " << p.y << " " << p.z << endl;
			os << r.x << " " << r.y << " " << r.z << endl;
			os << gPlantData.mLeaves[i].scal << endl
				<< gPlantData.mLeaves[i].angle << endl
				<< gPlantData.mLeaves[i].rotY << endl;
		}
		os << endl;

	
		os.close();
	}
}
/*



*/

void SettingDialog::OnBnClickedCheck1()
{
	gLS.mParam.mIsTrunk = mIsTrunkCheck.GetCheck();
	gD3d.Draw(gPlantData, gLS.mParam.mIsTrunk, gLS.mParam.mIsLeave);
}


void SettingDialog::OnBnClickedCheck2()
{
	gLS.mParam.mIsLeave = mIsLeaveCheck.GetCheck();
	gD3d.Draw(gPlantData, gLS.mParam.mIsTrunk, gLS.mParam.mIsLeave);
}


void SettingDialog::OnBnClickedRadio1()
{
	mLeafOrder = 0;
}


void SettingDialog::OnBnClickedRadio2()
{
	mLeafOrder = 1;
}


void SettingDialog::OnBnClickedRadio3()
{
	mLeafOrder = 2;
}
