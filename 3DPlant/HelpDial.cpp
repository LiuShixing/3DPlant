// HelpDial.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "3DPlant.h"
#include "HelpDial.h"
#include "afxdialogex.h"


// CHelpDial �Ի���

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
	cs = "����: ����\r\n";
	cs += "[  : ����ǰ״̬(x,y,z,��)��ջ�������浱ǰ״̬��\r\n";
	cs += "]  : ��ջ��״̬����������Ϊ��ǰ״̬��\r\n";
	cs += "x+ : ��֦������x����ʱ�뷽����ת�ȶȡ�\r\n";
	cs += "x- : ��֦������x��˳ʱ�뷽����ת�ȶȡ�\r\n";
	cs += "y+ : ��֦������y����ʱ�뷽����ת�ȶȡ�\r\n";
	cs += "y- : ��֦������y��˳ʱ�뷽����ת�ȶȡ�\r\n";
	cs += "z+ : ��֦������z����ʱ�뷽����ת�ȶȡ�\r\n";
	cs += "z- : ��֦������z��˳ʱ�뷽����ת�ȶȡ�\r\n";
	cs += "F  : ��֦��������һ����Ϊd�ķ�֦��������״̬(x��,y��,z��,��)��\r\n";
	DDX_Text(pDX, IDC_EDIT1,cs );
	
}


BEGIN_MESSAGE_MAP(CHelpDial, CDialog)
END_MESSAGE_MAP()


// CHelpDial ��Ϣ�������
