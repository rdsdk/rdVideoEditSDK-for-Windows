// TransitionCombobox.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "XPKCombobox.h"
#include "XPKAddMedia.h"
#include "AdvancedSettingAudioDialog.h"

// CXPKCombobox

IMPLEMENT_DYNAMIC(CXPKCombobox, CComboBox)


CXPKCombobox::CXPKCombobox(int combobox_type)
{
	m_curSel = 0;
	m_change = 0;
	m_type = combobox_type;
	
}

CXPKCombobox::~CXPKCombobox()
{
}


BEGIN_MESSAGE_MAP(CXPKCombobox, CComboBox)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CXPKCombobox::OnCbnSelchange)
END_MESSAGE_MAP()



// CXPKCombobox 消息处理程序

// CFontTypeCombobox 消息处理程序
void CXPKCombobox::set_DropDownHight(CComboBox*box,UINT LinesToDisplay)
{
	CRect cbSize;
	int Height;

	box->GetClientRect(cbSize); 
	Height = box->GetItemHeight(-1); 
	Height += box->GetItemHeight(0) * LinesToDisplay;

	Height += GetSystemMetrics(SM_CYEDGE) * 2;

	Height += GetSystemMetrics(SM_CYEDGE) * 2;

	box->SetWindowPos(NULL,
		0, 0,
		cbSize.right, Height,
		SWP_NOMOVE | SWP_NOZORDER
		); 
}
void CXPKCombobox::set_DropDownWidth(CComboBox*box,UINT nWidth )
{
	box->SetDroppedWidth(nWidth);
}
int CXPKCombobox::init()
{
	//设置字体
	set_DropDownHight(this,5);
	set_DropDownWidth(this,100);

	if (m_type == COMBOBOX_TRANSITION)
	{
#if 0
		InsertString(0,"0");
		InsertString(1,"1.左推");
		InsertString(2,"2.右推");
		InsertString(3,"3.上推");
		InsertString(4,"4.下推");
		InsertString(5,"5.淡入");
		InsertString(6,"6.淡出");
		InsertString(7,"7.时钟");
		InsertString(8,"8.马赛克");
#else

		int number = xpkGetTotalTransition();
		InsertString(0,"0");
   		for (int i= 1;i <= number;i++)
			InsertString(i,xpkGetTransitionName(i));

#endif
		SetCurSel(0);
	}
	return 1;
}
int CXPKCombobox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	init();	
	return 0;
}
int CXPKCombobox::getCurTransition()
{
	return GetCurSel();
}

void CXPKCombobox::OnCbnSelchange()
{
	// TODO: 在此添加控件通知处理程序代码
	
	int pos = GetCurSel();
	if (pos != m_curSel)
		m_change = 1;
	else
		m_change = 0;
	m_curSel = pos;
	if (COMBOBOX_PICTURE_PROPERTY == m_type)
	{
		//图片属性设置改变
		CAdvancedSettingPictureDialog* pDlg = (CAdvancedSettingPictureDialog*)GetParent();
		pDlg->picturePropertyChange(m_change);
	}
	else if(COMBOBOX_TRANSITION == m_type)
	{
		//转场方式改变
		CXPKAddMedia* pDlg = (CXPKAddMedia*)GetParent();
		pDlg->transitionChange(m_change);
	}
	else if (COMBOBOX_MEDIA_PROPERTY == m_type && m_change)
	{
		//视频属性设置改变
		CAdvancedSettingMediaDialog* pDlg = (CAdvancedSettingMediaDialog*)GetParent();
		pDlg->picturePropertyChange(m_change);
	}
	m_change = 0;

}
