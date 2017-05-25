// XPKEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "XPKEdit.h"
#include "XPKPreview.h"
#include "XPKWizard.h"


extern struct MEDIA_LIST *g_pMediaList;
// CXPKEdit 对话框

IMPLEMENT_DYNAMIC(CXPKEdit, CPropertyPage)

CXPKEdit::CXPKEdit()
	: CPropertyPage(CXPKEdit::IDD)
{

	m_exit = 0;
	edit_width = 0;
	edit_height = 0;
	hSlide = NULL;
	m_UIThread = NULL;
	m_EditThread = NULL;
	m_static_out_left_zoom = 0;
	m_static_resolution_left_zoom = 0;
	m_edit_out_left_zoom = 0;
	m_edit_resolution_left_zoom = 0;
	m_static_out_top_zoom = 0;
	m_button_edit_top_zoom = 0 ;
	m_progress_top_zoom = 0;
	m_progress_left_zoom = 0;

}

CXPKEdit::~CXPKEdit()
{
	TerminateThread(m_UIThread,0);
	WaitForSingleObject(m_UIThread,INFINITE );
}

void CXPKEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_PATH, m_outPutPathCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressCtrl);
	DDX_Control(pDX, IDC_EDIT_RESOLUTION, m_ResolutionCtrl);
}


BEGIN_MESSAGE_MAP(CXPKEdit, CPropertyPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_EDIT, &CXPKEdit::OnBnClickedEdit)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_EDIT_RESOLUTION, &CXPKEdit::OnCbnSelchangeEditResolution)
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CXPKEdit 消息处理程序

BOOL CXPKEdit::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类

	RECT dlg_rect = {0};
	RECT page_rect = {0};
	CPropertySheet *pSheet=(CPropertySheet *)GetParent();
	ASSERT_KINDOF(CPropertySheet,pSheet);
	pSheet->SetWizardButtons(PSWIZB_FINISH|PSWIZB_BACK); 
	//pSheet->GetDlgItem(ID_WIZFINISH)->SetWindowText("finish");
	//pSheet->GetDlgItem(ID_WIZBACK)->SetWindowText("back");
	pSheet->GetDlgItem(ID_WIZBACK)->ShowWindow(SW_SHOW);
	m_progressCtrl.SetPos(0);

	pSheet->GetClientRect(&dlg_rect);
	page_rect = dlg_rect;
	page_rect.bottom = page_rect.bottom - 50;
	MoveWindow(&page_rect);

	return CPropertyPage::OnSetActive();
}

BOOL CXPKEdit::OnWizardFinish()
{
	// TODO: 在此添加专用代码和/或调用基类

	XPKWizard *pSheet=(XPKWizard *)GetParent();
	m_exit = 1;
	if(pSheet->hSlide)
	{
		if(XKP_EDIT == pSheet->statu)
			xpkEditStop(pSheet->hSlide);
		if (m_EditThread)
		{
			//TerminateThread(m_EditThread,0);
			WaitForSingleObject( m_EditThread, INFINITE );
		}
		if (m_UIThread)
		{
			TerminateThread(m_UIThread,0);
			WaitForSingleObject( m_UIThread, INFINITE );
		}
		xpkCloseMediaEditor(pSheet->hSlide);
	}

	return CPropertyPage::OnWizardFinish();
}

void CXPKEdit::set_DropDownHight(CComboBox*box,UINT LinesToDisplay)
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
void CXPKEdit::set_DropDownWidth(CComboBox*box,UINT nWidth )
{
	box->SetDroppedWidth(nWidth);
}
BOOL CXPKEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	RECT dlg_rect = {0};
	RECT page_rect = {0};
	CPropertySheet *pSheet=(CPropertySheet *)GetParent();
	pSheet->GetClientRect(&dlg_rect);
	page_rect = dlg_rect;
	page_rect.bottom = page_rect.bottom - 50;
	MoveWindow(&page_rect);

	m_outPutPathCtrl.EnableWindow(FALSE);
	strcpy(szFilePath,GetResourcePath(OUTPUT_FILE));
	m_outPutPathCtrl.SetWindowText(szFilePath);
	m_progressCtrl.SetRange(0,100);

	set_DropDownHight(&m_ResolutionCtrl,5);
	set_DropDownWidth(&m_ResolutionCtrl,100);
	m_ResolutionCtrl.InsertString(0,"640x480");
	m_ResolutionCtrl.InsertString(1,"720x480");
	m_ResolutionCtrl.InsertString(2,"1080x720");
	m_ResolutionCtrl.InsertString(3,"1920x1080");
	m_ResolutionCtrl.SetCurSel(0);

	RECT rect = {0};
	RECT showRect = {0};
	int width = 0;
	GetClientRect(&rect);

	//static: out put path
	GetDlgItem(IDC_STATIC_OUTPUT_FILE)->GetWindowRect(&showRect);
	ScreenToClient(&showRect);
	width = showRect.right - showRect.left;
	showRect.left = showRect.left +110;
	showRect.right = showRect.left + width;
	GetDlgItem(IDC_STATIC_OUTPUT_FILE)->MoveWindow(&showRect);
	m_static_out_top_zoom = (float)showRect.top / (float)rect.bottom;
	m_static_out_left_zoom = (float)showRect.left/ (float)rect.right;

	//static: out put resolution
	GetDlgItem(IDC_STATIC_OUT_RESOLUTION)->GetWindowRect(&showRect);
	ScreenToClient(&showRect);
	width = showRect.right - showRect.left;
	showRect.left = showRect.left +110;
	showRect.right = showRect.left + width;
	GetDlgItem(IDC_STATIC_OUT_RESOLUTION)->MoveWindow(&showRect);
	m_static_resolution_left_zoom = (float)showRect.left / (float)rect.right;

	//edit: out put path
	GetDlgItem(IDC_EDIT_OUTPUT_PATH)->GetWindowRect(&showRect);
	ScreenToClient(&showRect);
	width = showRect.right - showRect.left;
	showRect.left = showRect.left +110;
	showRect.right = showRect.left + width;
	GetDlgItem(IDC_EDIT_OUTPUT_PATH)->MoveWindow(&showRect);
	m_edit_out_left_zoom = (float)showRect.left / (float)rect.right;

	//edit: out put resolution
	GetDlgItem(IDC_EDIT_RESOLUTION)->GetWindowRect(&showRect);
	ScreenToClient(&showRect);
	width = showRect.right - showRect.left;
	showRect.left = showRect.left +110;
	showRect.right = showRect.left + width;
	GetDlgItem(IDC_EDIT_RESOLUTION)->MoveWindow(&showRect);
	m_edit_resolution_left_zoom = (float)showRect.left / (float)rect.right;

	//static : display
	GetDlgItem(IDC_SHOW_EDIAT_PICTURE)->GetWindowRect(&showRect);
	ScreenToClient(&showRect);
	
	m_display_width_zoom = (float)(showRect.right - showRect.left) / (float)rect.right;
	m_display_top_zoom = (float)showRect.top / (float)rect.bottom;
	
	//progress
	GetDlgItem(IDC_PROGRESS1)->GetWindowRect(&showRect);
	ScreenToClient(&showRect);
	width =	showRect.right - showRect.left;
	showRect.left = (rect.right - (showRect.right - showRect.left))/2;
	showRect.right = showRect.left + width;
	GetDlgItem(IDC_PROGRESS1)->MoveWindow(&showRect);
	m_progress_left_zoom = (float)showRect.left / (float)rect.right;
	m_progress_top_zoom = (float)showRect.top / (float)rect.bottom;


	GetDlgItem(IDC_XPK_EDIT)->GetWindowRect(&showRect);
	ScreenToClient(&showRect);
	width =	showRect.right - showRect.left;
	showRect.left = (rect.right - (showRect.right - showRect.left))/2;
	showRect.right = showRect.left + width;
	GetDlgItem(IDC_XPK_EDIT)->MoveWindow(&showRect);
	m_button_edit_top_zoom = (float)showRect.top / (float)rect.bottom;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CXPKEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	UpdateData();

	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CPropertyPage::OnPaint()
}
char* CXPKEdit::GetResourcePath(char* name)
{
	char pFilePath[MAX_PATH+1];
	char path[MAX_PATH + 1];
	if(!name)
		return NULL;
	if(!pFilePath)
		return NULL;

	memset(pFilePath,0,MAX_PATH+1);
	memset(path,0,MAX_PATH+1);
	GetModuleFileName(NULL, path, MAX_PATH);
	(strrchr(path, ('\\')))[1] = 0; //删除文件名，只获得路径
	memcpy(pFilePath,path,strlen(path));
	strcat(pFilePath,name);
	return pFilePath;

}

DWORD WINAPI Thread_XPK_UIRefresh(LPVOID lpParam)
{
	CXPKEdit * pEditDlg = (CXPKEdit*)lpParam;
	int pos  = 0;
	
	while(1)
	{
		Sleep(100);
		if(pEditDlg->hSlide)
			pos = xpkEditProgress(pEditDlg->hSlide);
		pEditDlg->m_progressCtrl.SetPos(pos);
	}
	return 0;
}
DWORD WINAPI Thread_XPK_Edit(LPVOID lpParam)
{

	CXPKEdit * pEditDlg = (CXPKEdit*)lpParam;
	char buf[MAX_PATH];
	int ret = 0;
	
	ret = xpkEditMedia(pEditDlg->hSlide);
	if (ret <= 0)
	{
		sprintf(buf,"xpkEditMedia error! error : %d \n",ret);
		AfxMessageBox(buf);
		OutputDebugString(buf);
	}
	else
		OutputDebugString("xpkEditMedia success!\n");

	if (pEditDlg->m_exit)
		return 1;
	pEditDlg->GetDlgItem(IDC_XPK_EDIT)->SetWindowText("EDIT");
	pEditDlg->m_ResolutionCtrl.EnableWindow(TRUE);
	
	return 1;
}
int CXPKEdit::getOutPutResolution()
{

	int curSel = 0;
	CString str= NULL;

	curSel = m_ResolutionCtrl.GetCurSel();
	switch(curSel)
	{
	case 0:
		edit_width = 640;
		edit_height = 480;
		break;
	case  1:
		edit_width = 720;
		edit_height = 480;
		break;
	case 2:
		edit_width = 1080;
		edit_height = 720;
		break;
	case 3:
		edit_width = 1920;
		edit_height = 1080;
		break;
	}
	return 1;
}
void CXPKEdit::OnBnClickedEdit()
{
	// TODO: 在此添加控件通知处理程序代码
	int ret = 0;
	XPKWizard* pSheet = reinterpret_cast<XPKWizard*>(GetParent());
	CWnd *wnd=GetDlgItem(IDC_SHOW_EDIAT_PICTURE);

	if (XKP_PREVIEW == pSheet->statu)
	{
		xpkPreviewStop(pSheet->hSlide);
		if(pSheet->init()<=0)
		{
			AfxMessageBox("reinit init error ");
			return ;
		}
		pSheet->statu = XKP_EDIT;
		if (!edit_width || !edit_height)
		{
			ret = getOutPutResolution();
			if(ret <= 0)
			{
				AfxMessageBox("getOutPutResolution error ");
				return;
			}
		}
		//设置输出文件的分辨率大小
		xpkSetFrameSize(pSheet->hSlide,edit_width,edit_height);

		ret = xpkEditInit(pSheet->hSlide,wnd->m_hWnd);
		if (ret <= 0)
		{
			AfxMessageBox("edit apiPreviewInit error");
			return ;
		}
	}
	else
	{
		CString str= NULL;
		GetDlgItem(IDC_XPK_EDIT)->GetWindowText(str);
		if (str == "STOP")
		{
			pSheet->statu = XKP_EDIT;
			xpkEditStop(pSheet->hSlide);
			GetDlgItem(IDC_XPK_EDIT)->SetWindowText("EDIT");
			if(m_UIThread)
			{
				TerminateThread(m_UIThread,0);
				WaitForSingleObject(m_UIThread,INFINITE );
			}
			m_UIThread = NULL;
			m_progressCtrl.SetPos(0);
			m_ResolutionCtrl.EnableWindow(TRUE);
			return ;
		}
		
	}
	
	strcpy(szFilePath,GetResourcePath(OUTPUT_FILE));
	ret = xpkSetOutPath(pSheet->hSlide,szFilePath);
	if (ret <= 0)
		return ;

	WaitForSingleObject(m_EditThread,INFINITE );
// 	添加特效
// 		if(pSheet->addApngList()<=0)
// 		{
// 			AfxMessageBox("addApngList error");
// 			return ;
// 		}
// 		//添加配乐
// 		if(pSheet->adddubmusic()<=0)
// 		{
// 			AfxMessageBox("adddubmusic error");
// 			return ;
// 		}
// 		//添加字幕
// 		if(pSheet->addSubtitle()<=0)
// 		{
// 			AfxMessageBox("addSubtitle error");
// 			return ;
// 		}
	
	hSlide = pSheet->hSlide;
	m_EditThread = CreateThread(NULL,0,Thread_XPK_Edit,(LPVOID)this,0,NULL);

	if(m_UIThread)
	{
		TerminateThread(m_UIThread,0);
		WaitForSingleObject(m_UIThread,INFINITE );
	}
	m_UIThread = CreateThread(NULL,0,Thread_XPK_UIRefresh,(LPVOID)this,0,NULL);
	GetDlgItem(IDC_XPK_EDIT)->SetWindowText("STOP");
	m_ResolutionCtrl.EnableWindow(FALSE);
}

HBRUSH CXPKEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if( pWnd->GetDlgCtrlID() == IDC_SHOW_EDIAT_PICTURE )
	{
		// 		pDC->SetTextColor(RGB(255,0,0)); //字体颜色 
		// 		pDC->SetBkColor(RGB(0, 0, 0)); //字体背景色 
		// 		return (HBRUSH)::GetStockObject(BLACK_BRUSH);  // 设置背景色

		pDC->SetTextColor(RGB(255,0,0));  //设置字体颜色
		pDC->SetBkMode(TRANSPARENT); //设置字体背景为透明
		return (HBRUSH)::GetStockObject(BLACK_BRUSH);  // 设置背景色
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

LRESULT CXPKEdit::OnWizardBack()
{
	// TODO: 在此添加专用代码和/或调用基类
	XPKWizard* pSheet = reinterpret_cast<XPKWizard*>(GetParent());
	CString str= NULL;
	GetDlgItem(IDC_XPK_EDIT)->GetWindowText(str);
	if (str == "STOP")
	{
		AfxMessageBox("请先停止制作...");
		return 1;
	}

	if(m_UIThread)
	{
		TerminateThread(m_UIThread,0);
		WaitForSingleObject( m_UIThread, INFINITE );
	}
	m_UIThread = NULL;

	return CPropertyPage::OnWizardBack();
}

void CXPKEdit::OnBnClickedSetOutputPath()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CXPKEdit::OnCbnSelchangeEditResolution()
{
	// TODO: 在此添加控件通知处理程序代码

	XPKWizard* pSheet = reinterpret_cast<XPKWizard*>(GetParent());
	int pos = 0;
	int len = 0;
	CString str= NULL;
	int w = 0;
	int h = 0;
	int curSel = 0;
	int ret  = 0;
	CWnd *wnd=GetDlgItem(IDC_SHOW_EDIAT_PICTURE);

	curSel = m_ResolutionCtrl.GetCurSel();
	switch(curSel)
	{
	case 0:
		w = 640;
		h = 480;
		break;
	case  1:
		w = 720;
		h = 480;
		break;
	case 2:
		w = 1080;
		h = 720;
		break;
	case 3:
		w = 1920;
		h = 1080;
		break;
	}
	
	if (w != edit_width || h !=edit_height)
	{
		//如果修改了宽高，重新初始化
		
		if(pSheet->init()<=0)
		{
			AfxMessageBox("reinit init error ");
			return ;
		}
		pSheet->statu = XKP_EDIT;
		//设置输出文件的分辨率大小
		xpkSetFrameSize(pSheet->hSlide,w,h);

		ret = xpkEditInit(pSheet->hSlide,wnd->m_hWnd);
		if (ret <= 0)
		{
			AfxMessageBox("edit apiPreviewInit error");
			return ;
		}

		edit_width = w;
		edit_height = h;
	}
}

void CXPKEdit::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_static_out_left_zoom)
	{
		//更新控件
		int width = 0;
		int height = 0;
		RECT rect = {0};
		RECT showRect = {0};
		int ctrl_top = 0;
		int ctrl_left = 0;

		//static: display
		GetDlgItem(IDC_SHOW_EDIAT_PICTURE)->GetClientRect(&rect);
		float factor = (float)rect.bottom /(float)rect.right;

		GetClientRect(&rect);
		width = rect.right * m_display_width_zoom;
		height = width * factor;
		showRect.left =  (rect.right - width)/2 ;
		showRect.right = showRect.left + width;
		showRect.top = rect.bottom * m_display_top_zoom;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_SHOW_EDIAT_PICTURE)->MoveWindow(&showRect);


		//static: out put path
		GetDlgItem(IDC_STATIC_OUTPUT_FILE)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.right*m_static_out_left_zoom ;
		showRect.right = showRect.left + width;
		showRect.top = rect.bottom*m_static_out_top_zoom;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_STATIC_OUTPUT_FILE)->MoveWindow(&showRect);
		ctrl_top = showRect.top;


		//edit: out put path
		GetDlgItem(IDC_EDIT_OUTPUT_PATH)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top+3;

		GetClientRect(&rect);
		showRect.left =  rect.right*m_edit_out_left_zoom ;
		showRect.right = showRect.left + width;
		showRect.top = ctrl_top;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_EDIT_OUTPUT_PATH)->MoveWindow(&showRect);
		

		//static: out put resolution
		GetDlgItem(IDC_STATIC_OUT_RESOLUTION)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.right*m_static_resolution_left_zoom ;
		showRect.right = showRect.left + width;
		showRect.top = ctrl_top;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_STATIC_OUT_RESOLUTION)->MoveWindow(&showRect);


		//edit: out put resolution
		GetDlgItem(IDC_EDIT_RESOLUTION)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.right*m_edit_resolution_left_zoom ;
		showRect.right = showRect.left + width;
		showRect.top = ctrl_top;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_EDIT_RESOLUTION)->MoveWindow(&showRect);

		//progress 
		GetDlgItem(IDC_SHOW_EDIAT_PICTURE)->GetClientRect(&rect);
		width = rect.right - rect.left;
		GetDlgItem(IDC_PROGRESS1)->GetClientRect(&rect);
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.right*m_progress_left_zoom ;
		showRect.right = showRect.left + width;
		showRect.top = rect.bottom * m_progress_top_zoom;;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_PROGRESS1)->MoveWindow(&showRect);

		//edit button
		GetDlgItem(IDC_XPK_EDIT)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		GetClientRect(&rect);
		showRect.left =  (rect.right - width)/2 ;
		showRect.right = showRect.left + width;
		showRect.top = rect.bottom * m_button_edit_top_zoom;;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_XPK_EDIT)->MoveWindow(&showRect);

	}
}


void CXPKEdit::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_UIThread)
	{
		TerminateThread(m_UIThread,0);
		WaitForSingleObject(m_UIThread,INFINITE );
	}
	
	CPropertyPage::OnClose();
}
