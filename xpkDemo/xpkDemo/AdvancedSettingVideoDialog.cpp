// AdvancedSettingVideoDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "AdvancedSettingVideoDialog.h"


// CAdvancedSettingVideoDialog 对话框

IMPLEMENT_DYNAMIC(CAdvancedSettingVideoDialog, CDialog)

CAdvancedSettingVideoDialog::CAdvancedSettingVideoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvancedSettingVideoDialog::IDD, pParent)
{
	
}

CAdvancedSettingVideoDialog::~CAdvancedSettingVideoDialog()
{
}

void CAdvancedSettingVideoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAdvancedSettingVideoDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CAdvancedSettingVideoDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CAdvancedSettingVideoDialog 消息处理程序

void CAdvancedSettingVideoDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}
