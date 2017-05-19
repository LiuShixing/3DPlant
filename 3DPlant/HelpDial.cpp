// HelpDial.cpp : 实现文件
//

#include "stdafx.h"
#include "3DPlant.h"
#include "HelpDial.h"
#include "afxdialogex.h"


// CHelpDial 对话框

IMPLEMENT_DYNAMIC(CHelpDial, CDialog)

CHelpDial::CHelpDial(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpDial::IDD, pParent)
{

}

CHelpDial::~CHelpDial()
{
}

void CHelpDial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mText);
	CString cs;
	cs = "符号: 含义\r\n";
	cs += "[  : 将当前状态(x,y,z,ν)入栈，即保存当前状态。\r\n";
	cs += "]  : 将栈顶状态弹出，并置为当前状态。\r\n";
	cs += "x+ : 分枝方向绕x轴逆时针方向旋转θ度。\r\n";
	cs += "x- : 分枝方向绕x轴顺时针方向旋转θ度。\r\n";
	cs += "y+ : 分枝方向绕y轴逆时针方向旋转θ度。\r\n";
	cs += "y- : 分枝方向绕y轴顺时针方向旋转θ度。\r\n";
	cs += "z+ : 分枝方向绕z轴逆时针方向旋转θ度。\r\n";
	cs += "z- : 分枝方向绕z轴顺时针方向旋转θ度。\r\n";
	cs += "F  : 分枝方向生成一长度为d的分枝，进入新状态(x′,y′,z′,ν)。\r\n";
	DDX_Text(pDX, IDC_EDIT1,cs );
	
}


BEGIN_MESSAGE_MAP(CHelpDial, CDialog)
END_MESSAGE_MAP()


// CHelpDial 消息处理程序
