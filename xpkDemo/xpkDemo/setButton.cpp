// setButton.cpp : 实现文件
//

#include "stdafx.h"
#include "setButton.h"
#include "XPKAddMedia.h"



// CSetButton

IMPLEMENT_DYNAMIC(CSetButton, CButton)

CSetButton::CSetButton()
{

}

CSetButton::~CSetButton()
{
}


BEGIN_MESSAGE_MAP(CSetButton, CButton)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CSetButton 消息处理程序



void CSetButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int id = GetDlgCtrlID() - RESOURCE_ID*2;
	
	CXPKAddMedia* dlg = (CXPKAddMedia*)GetParent();
	dlg->showAdvanceDlg(RESOURCE_ID*3+id);

	CButton::OnLButtonDown(nFlags, point);
}
