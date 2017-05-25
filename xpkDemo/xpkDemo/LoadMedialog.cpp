// LoadMedialog.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "LoadMedialog.h"


// CLoadMedialog 对话框

IMPLEMENT_DYNAMIC(CLoadMedialog, CDialog)

CLoadMedialog::CLoadMedialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadMedialog::IDD, pParent)
{

	loaded = 0;
	 
}

CLoadMedialog::~CLoadMedialog()
{
	loaded = 0;
}

void CLoadMedialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoadMedialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CLoadMedialog::OnBnClickedButton1)
//	ON_WM_TIMER()
ON_WM_TIMER()
END_MESSAGE_MAP()


// CLoadMedialog 消息处理程序



int CLoadMedialog::loadMediaing()
{
	DoModal();
	return 0;
}
int CLoadMedialog::loadMediaFinish()
{
	loaded = 1;
	return 0;
}
void CLoadMedialog::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	loaded = 1;
	
}


void CLoadMedialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (loaded)
	{
		EndDialog(IDCANCEL);
		loaded = 0;
	
		return ;
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CLoadMedialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetTimer(1,50,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
