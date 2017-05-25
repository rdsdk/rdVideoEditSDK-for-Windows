// XPKPreview.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "XPKPreview.h"
#include "XPKWizard.h"



// CXPKPreview 对话框

IMPLEMENT_DYNAMIC(CXPKPreview, CPropertyPage)

CXPKPreview::CXPKPreview()
	: CPropertyPage(CXPKPreview::IDD)
{
	preview_width = 0;
	preview_height = 0;
	hSlide = NULL;
	hUIThread = NULL;
	pSlider = NULL;
	pMusicList = NULL;
	pSubtitleList = NULL;
	pApngList = NULL;

	m_button_play_left_zoom = 0.0;
	m_button_stop_left_zoom = 0.0;
	m_static_start_time_top_zoom = 0.0;
	m_static_end_time_top_zoom = 0.0;
	m_tabCtrl_left_zoom = 0.0;
	m_display_width_zoom = 0.0;
	m_display_left_zoom = 0.0;
	m_display_top_zoom = 0.0f;
	m_slider_top_zoom = 0.0;
}	

CXPKPreview::~CXPKPreview()
{
	XPK_MUSIC_LIST* pMusic = NULL;
	XPK_SUBTITLE_LIST* pSub = NULL;
	XPK_APNG_RGB_OVERLAY_LIST* pApng = NULL;
	if(pSlider)
		delete pSlider;
	TerminateThread(hUIThread,0);
	WaitForSingleObject(hUIThread,INFINITE );

	while(pSubtitleList)
	{
		pSub = pSubtitleList->next;
		free(pSubtitleList);
		pSubtitleList = pSub;
	}

	while(pMusicList)
	{
		pMusic = pMusicList->next;
		free(pMusicList);
		pMusicList = pMusic;
	}

	while(pApngList)
	{
		pApng = pApngList->next;
		free(pApngList);
		pApngList = pApng;
	}
}

void CXPKPreview::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
}


BEGIN_MESSAGE_MAP(CXPKPreview, CPropertyPage)
	ON_BN_CLICKED(IDC_PLAY, &CXPKPreview::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_STOP, &CXPKPreview::OnBnClickedStop)
	ON_MESSAGE(WM_USER+1,OnPreviewSeek)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CXPKPreview::OnTcnSelchangeTab1)
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CXPKPreview 消息处理程序

CString PreviewTransformTime(float ntime)
{
	char buf[1024];
	int x = (int)ntime%60;
	int y = (int)ntime/60;
	sprintf(buf,"%02d:%02d",y,x);
	return buf;
}

int __stdcall preview_play_finish(void* pUser)
{
	//播放完成
	CXPKPreview* pDlg = (CXPKPreview*)pUser;
	OutputDebugString("preview_play_finish IN");
	if(pDlg->hUIThread)
	{
		TerminateThread(pDlg->hUIThread,0);
		WaitForSingleObject(pDlg->hUIThread,INFINITE );
		pDlg->hUIThread = NULL;
	}
	pDlg->GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	pDlg->pSlider->SetPos(0);
	pDlg->GetDlgItem(IDC_TIME_START)->SetWindowText("00:00");
	pDlg->GetDlgItem(IDC_PLAY)->SetWindowText("PLAY");
	pDlg->stop = 1;
	OutputDebugString("preview_play_finish OUT");
	return 1;
}
DWORD WINAPI UIRefreshThreadProc(LPVOID lparams)
{
	CXPKPreview* pDlg =(CXPKPreview * )lparams ;
	int maxSliderPos = 0;
	int64_t curTime = 0;
	RECT rect = {0};

	int64_t totalTime = xpkGetTotalPlaybackTime(pDlg->hSlide);
	pDlg->GetDlgItem(IDC_TIME_END)->SetWindowText(PreviewTransformTime(totalTime/(float)1000000.0 + 0.5));
	pDlg->pSlider->SetRange(0,totalTime/100,0);
	pDlg->pSlider->SetPageSize(1);
	pDlg->pSlider->SetPos(0);
	//刷新tabCtrl
	pDlg->refreshTabCtrlUI();

	while(1)
	{
		if (pDlg->pSlider->seek)
		{
			//如果正在seek操作不更新滑块
			Sleep(500);
			continue;
		}
		curTime = xpkGetCurrentPlaybackTime(pDlg->hSlide);
		pDlg->GetDlgItem(IDC_TIME_START)->SetWindowText(PreviewTransformTime(curTime/(float)1000000.0 + 0.5));
		pDlg->pSlider->SetPos(curTime/100);
		Sleep(500);
	}

	return 1;
}

BOOL CXPKPreview::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类
	int ret = 0;
	char buf[1024];
	RECT rect = {0};
	RECT dlg_rect = {0};
	RECT page_rect = {0};
	CWnd *wnd=GetDlgItem(IDC_SHOW_PICTURE);
	XPKWizard* pSheet = reinterpret_cast<XPKWizard*>(GetParent());
	pSheet->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK); 
	pSheet->GetDlgItem(ID_WIZBACK)->ShowWindow(SW_SHOW);

	pSheet->GetClientRect(&dlg_rect);
	page_rect = dlg_rect;
	page_rect.bottom = page_rect.bottom - 50;
	MoveWindow(&page_rect);

	GetDlgItem(IDC_SHOW_PICTURE)->GetClientRect(&rect);
	preview_width = rect.right - rect.left;
	if(0 != preview_width%2)
		preview_width -= 1;
	preview_height = rect.bottom - rect.top;
	if(0 != preview_height%2)
		preview_height -= 1;

	//检查当前状态，当切换到该页面，直接播放
	hSlide = pSheet->hSlide;
	if (XKP_NONE == pSheet->statu)
	{
		pSheet->statu = XKP_PREVIEW;

		xpkSetFrameSize(hSlide,preview_width,preview_height);
		//设置回调函数
		xpkSetPreviewPlayFinishCallback(hSlide,preview_play_finish,this);
		ret = xpkPreviewInit(pSheet->hSlide,wnd->m_hWnd);
		if (ret <= 0)
		{
			sprintf(buf,"xpkPreviewInit error :%d line:%d \n",ret,__LINE__);
			OutputDebugString(buf);
			AfxMessageBox(buf);
			return 0;
		}
	}
	else if (XKP_EDIT == pSheet->statu)
	{
		//初始化字幕链表
		while (pSubtitleList)
		{
			XPK_SUBTITLE_LIST* temp = pSubtitleList->next;
			free(pSubtitleList);
			pSubtitleList = temp;
		}
		m_addSubtitle.OnBnClickedClearAllSubtitle();
		//初始化配乐链表
		while (pMusicList)
		{
			XPK_MUSIC_LIST *temp = pMusicList->next;
			free(pMusicList);
			pMusicList = temp;
		}
		m_addMusic.OnBnClickedClearAllMusic();
		//初始化特效链表
		while (pApngList)
		{
			XPK_APNG_RGB_OVERLAY_LIST *temp = pApngList->next;
			free(pApngList);
			pApngList = temp;
		}
		m_addApng.OnBnClickedClearAllApng();

		//初始化滤镜链表
		clearAllFilter();
		m_addFilter.OnClearAllFilter();
		
		if (pSheet->hSlide)
			pSheet->init();

		xpkSetFrameSize(pSheet->hSlide,preview_width,preview_height);
		xpkSetPreviewPlayFinishCallback(pSheet->hSlide,preview_play_finish,this);
		ret = xpkPreviewInit(pSheet->hSlide,wnd->m_hWnd);
		if (ret <= 0)
		{
			AfxMessageBox("apiPreviewInit error ");
			return 0;
		}
	}
	ret = xpkPreview(pSheet->hSlide);
	if (ret <= 0)
	{
		char buf[1024];
		sprintf(buf,"xpkPreview error :%d line:%d \n",ret,__LINE__);
		OutputDebugString(buf);
		AfxMessageBox(buf);
	}
	pSheet->statu = XKP_PREVIEW;
	GetDlgItem(IDC_PLAY)->SetWindowText("PAUSE");
	stop = 0;
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);

	if(hUIThread)
	{
		TerminateThread(hUIThread,0);
		WaitForSingleObject(hUIThread,INFINITE );
	}
	hUIThread = CreateThread(NULL,0,UIRefreshThreadProc,this,0,NULL);
	return CPropertyPage::OnSetActive();
}

LRESULT CXPKPreview::OnWizardNext()
{
	// TODO: 在此添加专用代码和/或调用基类

	UpdateData();
	OnBnClickedStop();
	hSlide = NULL;
	return CPropertyPage::OnWizardNext();
}

void CXPKPreview::OnBnClickedPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	int ret = 0;
	char buf[MAX_PATH];
	CWnd *wnd=GetDlgItem(IDC_SHOW_PICTURE);
	XPKWizard* pSheet = reinterpret_cast<XPKWizard*>(GetParent());
	RECT rect = {0};

	OutputDebugString("OnBnClickedPlay  IN \n");

	if (pSheet->statu == XKP_PREVIEW)
	{
		CString csText = NULL;
		if (!stop && xpkGetCurrentPlaybackTime(hSlide))
		{
			GetDlgItem(IDC_PLAY)->GetWindowText(csText);
			if (csText == "PLAY")
			{
				ret = xpkPreviewConintue(hSlide);
				GetDlgItem(IDC_PLAY)->SetWindowText("PAUSE");
			}
			else if (csText == "PAUSE")
			{
				ret = xpkPreviewPause(hSlide);
				GetDlgItem(IDC_PLAY)->SetWindowText("PLAY");
			}
			return;
		}
	}
	else 
	{
		int preview_width = 0;
		int preview_height = 0;
		pSheet->statu = XKP_PREVIEW;
		if (pSheet->init() <= 0)
		{
			AfxMessageBox("init error ");
			return;
		}
		
		GetDlgItem(IDC_SHOW_PICTURE)->GetClientRect(&rect);
		preview_width = rect.right - rect.left;
		if(0 != preview_width%2)
			preview_width -= 1;
		preview_height = rect.bottom - rect.top;
		if(0 != preview_height%2)
			preview_height -= 1;

		xpkSetFrameSize(pSheet->hSlide,preview_width,preview_height);
		xpkSetPreviewPlayFinishCallback(pSheet->hSlide,preview_play_finish,this);
		ret = xpkPreviewInit(pSheet->hSlide,wnd->m_hWnd);
		if (ret <= 0)
		{
			AfxMessageBox("apiPreviewInit error ");
			return;
		}
	}
	hSlide = pSheet->hSlide;
	stop = 0;
	OutputDebugString("apiPreview ....\n");
	 ret = xpkPreview(hSlide);
	if (ret <= 0)
	{
		sprintf(buf,"apiPreview error :%d line:%d \n",ret,__LINE__);
		OutputDebugString(buf);
		AfxMessageBox(buf);
		return;
	}
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);

	if(hUIThread)
	{
		TerminateThread(hUIThread,0);
		WaitForSingleObject(hUIThread,INFINITE );
	}
	hUIThread = CreateThread(NULL,0,UIRefreshThreadProc,this,0,NULL);
	GetDlgItem(IDC_PLAY)->SetWindowText("PAUSE");
	OutputDebugString("OnBnClickedPlay  OUT \n");
}

void CXPKPreview::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	XPKWizard* pSheet = reinterpret_cast<XPKWizard*>(GetParent());
	OutputDebugString("OnBnClickedStop IN\n");

	if (pSheet->hSlide && XKP_PREVIEW == pSheet->statu && xpkGetCurrentPlaybackTime(pSheet->hSlide))
		xpkPreviewStop(pSheet->hSlide);
	
	if(hUIThread)
	{
		TerminateThread(hUIThread,0);
		WaitForSingleObject(hUIThread,INFINITE );
		hUIThread = NULL;
	}
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	pSlider->SetPos(0);
	GetDlgItem(IDC_TIME_START)->SetWindowText("00:00");
	GetDlgItem(IDC_PLAY)->SetWindowText("PLAY");
	stop = 1;
	OutputDebugString("OnBnClickedStop OUT");
}

afx_msg LRESULT CXPKPreview::OnPreviewSeek(WPARAM,LPARAM)
{
	XPKWizard* pSheet = reinterpret_cast<XPKWizard*>(GetParent());
	//preview seek
	int64_t nPos = pSlider->GetPos();
	int ret = 0;
	char buf[1024] = {0};
	if(!hSlide)
		return 1;
	if (stop)
	{
		pSlider->SetPos(0);
		pSlider->seek = 0;
		return 1;
	}
	if (XKP_PREVIEW != pSheet->statu)
	{
		pSlider->seek = 0;
		return 1;
	}
	
	ret = xpkPreviewSeek(hSlide,nPos*100);
	if (ret <= 0)
	{
		pSlider->seek = 0;
		return 1;
	}	

	pSlider->seek = 0;
	return 1;
}
int CXPKPreview::getShowRect(RECT *rect)
{
	RECT rtShow = {0};
	GetDlgItem(IDC_SHOW_PICTURE)->GetClientRect(&rtShow);
	*rect = rtShow;
	return 1;
}
char* CXPKPreview::GetResourcePath(char* name)
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

BOOL CXPKPreview::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	XPKWizard* pSheet = reinterpret_cast<XPKWizard*>(GetParent());
	// TODO:  在此添加额外的初始化
	CWnd *wnd=GetDlgItem(IDC_SHOW_PICTURE);
	int ret = 0;
	int totalTime = 0;
	RECT page_rect = {0};
	//预览滑块
	RECT rect = {0};
	GetClientRect(&page_rect);
	GetClientRect(&rect);
	rect.left = rect.left + 50;
	rect.right = rect.right - 50;
	rect.top = rect.bottom - 80;
	rect.bottom = rect.bottom - 60;

	pSlider = new CXPKSlider;
	pSlider->Create(TBS_HORZ,rect,this,RESOURCE_ID+1);
	pSlider->SetRange(0,1000000,0);
	pSlider->SetPageSize(1);
	pSlider->ShowWindow(SW_SHOW);
	pSlider->SetPos(0);
	m_slider_top_zoom = (float)rect.top/(float)page_rect.bottom;

	GetClientRect(&rect);
	rect.left = rect.left + 200 + 90;
	rect.right = rect.left + 80;
	rect.top = rect.bottom - 50;
	rect.bottom = rect.bottom - 20;
	GetDlgItem(IDC_PLAY)->MoveWindow(&rect);
	m_button_play_left_zoom = (float)rect.left/(float)page_rect.right;

	GetClientRect(&rect);
	rect.left = rect.left + 200 + 90 + 100 + 80;
	rect.right = rect.left + 80;
	rect.top = rect.bottom - 50;
	rect.bottom = rect.bottom - 20;
	GetDlgItem(IDC_STOP)->MoveWindow(&rect);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	m_button_stop_left_zoom = (float)rect.left/(float)page_rect.right;

	GetClientRect(&rect);
	rect.left = rect.left + 10;
	rect.right = rect.left + 40;
	rect.top = rect.bottom - 80;
	rect.bottom = rect.bottom - 50;
	GetDlgItem(IDC_TIME_START)->MoveWindow(&rect);
	m_static_start_time_top_zoom = (float)rect.top/(float)page_rect.bottom;

	GetClientRect(&rect);
	rect.left = rect.right - 40;
	rect.right = rect.left + 80;
	rect.top = rect.bottom - 80;
	rect.bottom = rect.bottom - 50;
	GetDlgItem(IDC_TIME_END)->MoveWindow(&rect);
	m_static_end_time_top_zoom = (float)rect.top/(float)page_rect.bottom;

	//显示区域
	GetClientRect(&rect);
	rect.left = 40;
	rect.top = 20;
	rect.right = rect.left + PREVIEW_WIDTH;
	rect.bottom = PREVIEW_HEIGHT+20;
	GetDlgItem(IDC_SHOW_PICTURE)->MoveWindow(&rect);
	m_display_left_zoom = (float)rect.left/(float)page_rect.right;
	m_display_width_zoom = (float)rect.right/(float)page_rect.right;
	m_display_top_zoom = (float)rect.top/(float)page_rect.bottom;

	if(0 != (rect.bottom - rect.top)%2)
		rect.bottom -= 1;
	if(0 != (rect.right - rect.left)%2)
		rect.right -= 1;

	preview_width = rect.right - rect.left;
	preview_height = rect.bottom - rect.top;

	hSlide = pSheet->hSlide;
	xpkSetFrameSize(hSlide,preview_width,preview_height);
	//设置回调函数
	xpkSetPreviewPlayFinishCallback(hSlide,preview_play_finish,this);
	//初始化获取总时间
	ret = xpkPreviewInit(hSlide,wnd->m_hWnd);
	if (ret <= 0)
	{
		char buf[1024];
		sprintf(buf,"xpkPreviewInit error :%d line:%d \n",ret,__LINE__);
		OutputDebugString(buf);
		AfxMessageBox(buf);
	}

	totalTime = xpkGetTotalPlaybackTime(hSlide);
	pSlider->SetRange(0,totalTime,0);
	pSlider->SetPageSize(1);
	pSlider->SetPos(0);
	stop = 0;
	GetDlgItem(IDC_TIME_END)->SetWindowText(PreviewTransformTime(totalTime/(float)1000000.0 + 0.5));


	TCITEM tc1,tc2,tc3,tc4;
	tc1.mask = TCIF_TEXT;
	tc1.pszText = _T("配乐");
	tc2.mask = TCIF_TEXT;
	tc2.pszText = _T("特效");
	tc3.mask = TCIF_TEXT;
	tc3.pszText = _T("字幕");
	tc4.mask = TCIF_TEXT;
	tc4.pszText = _T("滤镜");

	m_tabctrl.SetParent(this);
	m_tabctrl.InsertItem(0, &tc1);
	m_tabctrl.InsertItem(1, &tc2); 
	m_tabctrl.InsertItem(2, &tc3);
	m_tabctrl.InsertItem(3, &tc4);


	CRect rec;
	m_tabctrl.GetClientRect(&rec);//获得TAB控件的坐标

	//定位选项卡页的位置，这里可以根据情况自己调节偏移量
	rec.bottom -= 2;
	rec.left += 1;
	rec.top += 22;
	rec.right -= 3;

	//创建子页面
	m_addMusic.Create(IDD_MUSIC, GetDlgItem(IDC_TAB1));
	m_addApng.Create(IDD_APNG_RGBA, GetDlgItem(IDC_TAB1));
	m_addSubtitle.Create(IDD_SUBTITLE, GetDlgItem(IDC_TAB1));
	m_addFilter.Create(IDD_FILTER,GetDlgItem(IDC_TAB1));

	//将子页面移动到指定的位置
	m_addMusic.MoveWindow(&rec);
	m_addApng.MoveWindow(&rec);
	m_addSubtitle.MoveWindow(&rec);
	m_addFilter.MoveWindow(&rec);

	//显示子页面
	m_addMusic.ShowWindow(SW_SHOW);
	m_addApng.ShowWindow(SW_HIDE);
	m_addSubtitle.ShowWindow(SW_HIDE);
	m_addFilter.ShowWindow(SW_HIDE);

	//设置默认的选项卡
	m_tabctrl.SetCurSel(0);


	CRect rec1;
	m_tabctrl.GetWindowRect(&rec1);//获得TAB控件的坐标
	ScreenToClient(rec1);
	m_tabCtrl_left_zoom = (float)rec1.left/(float)page_rect.right;

	//初始化状态
	totalTime = xpkGetTotalPlaybackTime(hSlide);
	pSlider->SetRange(0,totalTime,0);
	pSlider->SetPageSize(1);
	pSlider->SetPos(0);
	stop = 0;
	GetDlgItem(IDC_TIME_END)->SetWindowText(PreviewTransformTime(totalTime/(float)1000000.0 + 0.5));
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);

	pSheet->statu = XKP_PREVIEW;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

HBRUSH CXPKPreview::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
 	if( pWnd->GetDlgCtrlID() == IDC_SHOW_PICTURE )
 	{
		pDC->SetTextColor(RGB(255,0,0));  //设置字体颜色
		pDC->SetBkMode(TRANSPARENT); //设置字体背景为透明
		return (HBRUSH)::GetStockObject(BLACK_BRUSH);  // 设置背景色
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


LRESULT CXPKPreview::OnWizardBack()
{
	// TODO: 在此添加专用代码和/或调用基类

	XPKWizard *pSheet=(XPKWizard *)GetParent();
	if (XKP_PREVIEW == pSheet->statu && pSheet->hSlide)
		OnBnClickedStop();

	hSlide = NULL;
	return CPropertyPage::OnWizardBack();
}
int CXPKPreview::deleteSubtitle(int index)
{
	int ret = 0;
	XPK_SUBTITLE_LIST* list = NULL;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("deleteSubtitle error");
		return -__LINE__;
	}
	if (!pSubtitleList)
	{
		AfxMessageBox("subtitle null");
		return -__LINE__;
	}
	//1.停止
	OnBnClickedStop();
	//2.删除指定的节点
	if (!pSubtitleList->next)
	{
		ret =  xpkClearSubtitle(hSlide,pSubtitleList->hSubtitle);
		if (ret <= 0)
			AfxMessageBox("xpkClearSubtitle error");
		free(pSubtitleList);
		pSubtitleList = NULL;
		goto PLAY;
	}
	if(index == pSubtitleList->index)
	{
		XPK_SUBTITLE_LIST *temp = pSubtitleList->next;
		ret =  xpkClearSubtitle(hSlide,pSubtitleList->hSubtitle);
		if (ret <= 0)
			AfxMessageBox("xpkClearSubtitle error");
		free(pSubtitleList);
		pSubtitleList = temp;
		goto PLAY;
	}
	list = pSubtitleList;
	while (list)
	{
		if (list->next->index == index)
		{
			if (list->next->next)
			{
				XPK_SUBTITLE_LIST *temp = list->next->next;
				ret =  xpkClearSubtitle(hSlide,list->next->hSubtitle);
				if (ret <= 0)
					AfxMessageBox("xpkClearSubtitle error");
				free(list->next);
				list->next = temp;
			}
			else
			{
				ret =  xpkClearSubtitle(hSlide,list->next->hSubtitle);
				if (ret <= 0)
					AfxMessageBox("xpkClearSubtitle error");
				free(list->next);
				list->next = NULL;
			}

			break;
		}
		list = list->next;
	}	
PLAY:
	//3.播放
	OnBnClickedPlay();
	return 1;
}
int CXPKPreview::deleteApng(int index)
{

	int ret = 0;
	XPK_APNG_RGB_OVERLAY_LIST* list = NULL;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("deleteApng error");
		return -__LINE__;
	}
	if (!pApngList)
	{
		AfxMessageBox("pApngList null");
		return -__LINE__;
	}
	//1.停止
	OnBnClickedStop();
	//2.删除指定的节点
	if (!pApngList->next)
	{
		ret =  xpkClearMedia(hSlide,pApngList->hApng);
		if (ret <= 0)
			AfxMessageBox("xpkClearMedia error");
		free(pApngList);
		pApngList = NULL;
		goto PLAY;
	}
	if(index == pApngList->index)
	{
		XPK_APNG_RGB_OVERLAY_LIST *temp = pApngList->next;
		ret =  xpkClearMedia(hSlide,pApngList->hApng);
		if (ret <= 0)
			AfxMessageBox("xpkClearMedia error");
		free(pApngList);
		pApngList = temp;
		goto PLAY;
	}
	list = pApngList;
	while (list)
	{
		if (list->next->index == index)
		{
			if (list->next->next)
			{
				XPK_APNG_RGB_OVERLAY_LIST *temp = list->next->next;
				ret =  xpkClearMedia(hSlide,list->next->hApng);
				if (ret <= 0)
					AfxMessageBox("xpkClearMedia error");
				free(list->next);
				list->next = temp;
			}
			else
			{
				ret =  xpkClearMedia(hSlide,list->next->hApng);
				if (ret <= 0)
					AfxMessageBox("xpkClearMedia error");
				free(list->next);
				list->next = NULL;
			}

			break;
		}
		list = list->next;
	}	
PLAY:
	//3.播放
	OnBnClickedPlay();
	return 1;
}

XPK_APNG_RGB_OVERLAY_LIST* CXPKPreview::getApngParams(int index)
{
	XPK_APNG_RGB_OVERLAY_LIST* list = NULL;
	if (!pApngList)	
		return NULL;

	list = pApngList;
	while(list)
	{
		if (list->index == index)
			return list;
		list = list->next;
	}
	return NULL;
}
XPK_MUSIC_LIST* CXPKPreview::getMusicParams(int index)
{
	XPK_MUSIC_LIST* list = NULL;
	if (!pMusicList)
		return NULL;
	list = pMusicList;
	while(list)
	{
		if (list->index == index)
			return list;

		list  = list->next;
	}
	return NULL;
}
XPK_SUBTITLE_LIST* CXPKPreview::getSubtitleParams(int index)
{
	XPK_SUBTITLE_LIST* list = NULL;
	if(!pSubtitleList)
		return NULL;
	list = pSubtitleList;
	while(list)
	{
		if(list->index == index)
			return list;
		list = list->next;
	}
	return NULL;
}
int CXPKPreview::deleteMusic(int index)
{

	int ret = 0;
	XPK_MUSIC_LIST* list = NULL;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("deleteMusic error");
		return -__LINE__;
	}
	if (!pMusicList)
	{
		AfxMessageBox("pMusicList null");
		return -__LINE__;
	}
	//1.停止
	OnBnClickedStop();
	//2.删除指定的节点
	if (!pMusicList->next)
	{
		ret =  xpkClearAudioMedia(hSlide,pMusicList->hAudio);
		if (ret <= 0)
			AfxMessageBox("xpkClearAudioMedia error");
		free(pMusicList);
		pMusicList = NULL;
		goto PLAY;
	}
	if(index == pMusicList->index)
	{
		XPK_MUSIC_LIST *temp = pMusicList->next;
		ret =  xpkClearAudioMedia(hSlide,pMusicList->hAudio);
		if (ret <= 0)
			AfxMessageBox("xpkClearAudioMedia error");
		free(pMusicList);
		pMusicList = temp;
		goto PLAY;
	}
	list = pMusicList;
	while (list)
	{
		if (list->next->index == index)
		{
			if (list->next->next)
			{
				XPK_MUSIC_LIST *temp = list->next->next;
				ret =  xpkClearAudioMedia(hSlide,list->next->hAudio);
				if (ret <= 0)
					AfxMessageBox("xpkClearAudioMedia error");
				free(list->next);
				list->next = temp;
			}
			else
			{
				ret =  xpkClearAudioMedia(hSlide,list->next->hAudio);
				if (ret <= 0)
					AfxMessageBox("xpkClearAudioMedia error");
				free(list->next);
				list->next = NULL;
			}

			break;
		}
		list = list->next;
	}	
PLAY:
	//3.播放
	OnBnClickedPlay();
	return 1;
}
int CXPKPreview::clearAllMusic()
{
	int ret = 0;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("clearAllMusic error");
		return -__LINE__;
	}
	if(!pMusicList)
		return 1;
	OnBnClickedStop();
	while(pMusicList)
	{
		XPK_MUSIC_LIST* temp = pMusicList->next;
		ret = xpkClearAudioMedia(hSlide,pMusicList->hAudio);
		if (ret <= 0)
		{
			AfxMessageBox("xpkClearAudioMedias error ");
			return -__LINE__;
		}

		free(pMusicList);
		pMusicList = temp;
	}
	OnBnClickedPlay();
	return 1;
}
int CXPKPreview::clearAllApng()
{
	struct XPK_APNG_RGB_OVERLAY_LIST* pList = NULL;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("OnBnClickedAddSubtitle1 error");
		return -__LINE__;
	}
	if (!pApngList)
		return 1;

	//1.停止
	OnBnClickedStop();
	//2.删除指定特效
	while(pApngList)
	{	
		pList = pApngList->next;
		xpkClearMedia(pSheet->hSlide,pApngList->hApng);
		free(pApngList);
		pApngList = pList;
	}
	//3.继续播放
	OnBnClickedPlay();
	return 1;
}
int CXPKPreview::clearAllSubtitle()
{

	int ret = 0;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("OnBnClickedAddSubtitle1 error");
		return -__LINE__;
	}
	if (!pSubtitleList)
		return 1;
	
	//1.停止
	OnBnClickedStop();
	//2.清除字幕
	while(pSubtitleList)
	{
		XPK_SUBTITLE_LIST* list = pSubtitleList->next;
		ret = xpkClearSubtitle(hSlide,pSubtitleList->hSubtitle);
		if (ret <= 0)
		{
			AfxMessageBox("xpkClearAllSubtitle error ");
			return -__LINE__;
		}
		free(pSubtitleList);
		pSubtitleList = list;
	}
	pSubtitleList = NULL;

	//3.继续播放
	OnBnClickedPlay();
	return 1;
}
int CXPKPreview::addSubtitle(char* pText,int64_t timeline_start,int64_t timeline_end,FONT_PEP font,RECTF fontRect,char* srtFile,int id)
{

	//添加字幕时直接添加即可
	Handle hSubtitle = NULL;
	RGBQUAD subRGB = {0};
	int ret = 0;	
	int64_t timelineStart = timeline_start;
	int64_t timelineEnd = timeline_end;
	int effect = 1;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("OnBnClickedAddSubtitle1 error");
		return -__LINE__;
	}
	//1.停止
	OnBnClickedStop();
	
	//2.添加字幕，设置字幕属性
	if (pText && 0 != strlen(pText))
		hSubtitle = xpkAddSubtitleText(hSlide,pText);
	else if(srtFile && 0 != strlen(srtFile))
		hSubtitle = xpkSubtitleStrPath(hSlide,srtFile);
	else
	{ 
		AfxMessageBox("xpkAddSubtitleText or xpkSubtitleStrPath error");
		ret = -__LINE__;
		goto ADDSUBTITLE_FINISH;
	}
	if (!hSubtitle)
	{
		AfxMessageBox("OnBnClickedAddSubtitle1 error");
		ret = -__LINE__;
		goto ADDSUBTITLE_FINISH;
	}
	

	ret = xpkSetSubtitleTime(hSlide,hSubtitle,timeline_start,timeline_end);
	if(ret <= 0)
	{
		AfxMessageBox("xpkSetSubtitleTime error");
		goto ADDSUBTITLE_FINISH;
	}
	ret = xpkSetSubtitleRect(hSlide,hSubtitle,(RECTF)fontRect);
	if(ret <= 0)
	{
		AfxMessageBox("xpkSetSubtitleRect error");
		goto ADDSUBTITLE_FINISH;
	}

	ret = xpkSetSubtitleFont(hSlide,hSubtitle,font);
	if(ret <= 0)
	{
		AfxMessageBox("xpkSetSubtitleFont error");
		goto ADDSUBTITLE_FINISH;
	}		

	subRGB.rgbRed = 255;
	ret = xpkSetSubTitleEffect(hSlide,hSubtitle,effect,subRGB);
	if(ret <= 0)
	{
		AfxMessageBox("xpkSetSubTitleEffect error");
		goto ADDSUBTITLE_FINISH;
	}


	if (!pSubtitleList)
	{
		pSubtitleList = (XPK_SUBTITLE_LIST*)malloc(sizeof(XPK_SUBTITLE_LIST));
		if (!pSubtitleList)
		{
			ret = -__LINE__;
			goto ADDSUBTITLE_FINISH;
		}
		memset(pSubtitleList,0,sizeof(XPK_SUBTITLE_LIST));
		pSubtitleList->clr = subRGB;
		pSubtitleList->effect =effect;
		pSubtitleList->font = font;
		pSubtitleList->rect = fontRect;
		pSubtitleList->hSubtitle = hSubtitle;
		pSubtitleList->timeline_end = timelineEnd;
		pSubtitleList->timeline_start =timelineStart;
		pSubtitleList->index = id;
		strcpy(pSubtitleList->szText,pText);
		strcpy(pSubtitleList->srtPath,srtFile);
	}
	else
	{
		XPK_SUBTITLE_LIST *list = pSubtitleList;
		while(list&&list->next)
			list = list->next;
		list->next = (XPK_SUBTITLE_LIST*)malloc(sizeof(XPK_SUBTITLE_LIST));
		if (!list->next)
		{
			ret = -__LINE__;
			goto ADDSUBTITLE_FINISH;
		}
		memset(list->next,0,sizeof(XPK_SUBTITLE_LIST));
		list->next->clr = subRGB;
		list->next->effect =effect;
		list->next->font = font;
		list->next->rect = fontRect;
		list->next->hSubtitle = hSubtitle;
		list->next->timeline_end = timelineEnd;
		list->next->timeline_start = timelineStart;
		list->next->index = id;
		strcpy(list->next->szText,pText);
		strcpy(list->next->srtPath,srtFile);
	}

ADDSUBTITLE_FINISH:
	//3.继续播放
	OnBnClickedPlay();

	return ret;
}
int CXPKPreview::addApng(char* szPath,int64_t timeLine_start,int64_t timeLine_end,int index)
{

	//添加特效时直接添加即可
	int ret = 0;
	struct XPK_APNG_RGB_OVERLAY_LIST* pList = NULL;
	Handle hMedia = NULL;
	int64_t time_start = 0;
	int64_t time_end = 0;
	int64_t timeline_start = timeLine_start;
	int64_t timeline_end = timeLine_end;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("OnBnClickedAddSubtitle1 error");
		return -__LINE__;
	}

	//1.停止
	OnBnClickedStop();
	
	//2.添加特效
	hMedia = xpkAddMedia(hSlide,szPath,timeline_start,timeline_end,0,0);
	if(!hMedia)
	{
		ret = -__LINE__;
		goto ADDAPNG_FINISH;
	}

	//3.保存特效
	if (!pApngList)
	{
		pApngList = (XPK_APNG_RGB_OVERLAY_LIST*)malloc(sizeof(XPK_APNG_RGB_OVERLAY_LIST));
		if (!pApngList)
		{
			ret = -__LINE__;
			goto ADDAPNG_FINISH;
		}
		memset(pApngList,0,sizeof(XPK_APNG_RGB_OVERLAY_LIST));
		pApngList->timeline_start = timeline_start;
		pApngList->timeline_end = timeline_end;
		pApngList->time_start = time_start;
		pApngList->time_end = time_end;
		pApngList->hApng = hMedia;
		pApngList->index = index;
		strcpy(pApngList->szPath,szPath);
	}
	else
	{
		XPK_APNG_RGB_OVERLAY_LIST *list = pApngList;
		while(list&&list->next)
			list = list->next;
		list->next = (XPK_APNG_RGB_OVERLAY_LIST*)malloc(sizeof(XPK_APNG_RGB_OVERLAY_LIST));
		if (!list->next)
		{
			ret = -__LINE__;
			goto ADDAPNG_FINISH;
		}
		memset(list->next,0,sizeof(XPK_APNG_RGB_OVERLAY_LIST));
		list->next->timeline_start = timeline_start;
		list->next->timeline_end =timeline_end;
		list->next->time_start = time_start;
		list->next->hApng = hMedia;
		list->next->time_end = time_end;
		list->next->index = index;
		strcpy(list->next->szPath,szPath);
	}

ADDAPNG_FINISH:
	//4.播放
	OnBnClickedPlay();

	return 1;
}
int CXPKPreview::clearAllFilter()
{
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	pSheet->clearAllFilter();
	
	return 1;
}
int CXPKPreview::addFilter(int id)
{
	int ret = 0;
	XPK_FILTER_LIST* list = NULL;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("addFilter invalid params");
		return -__LINE__;
	}
	if (0 == id)
	{
		//添加滤镜不能直接添加，需要重新加载每一个元素和媒体
		OnBnClickedStop();
		pSheet->statu = XKP_NONE;
		OnBnClickedPlay();
		return 1;
	}	

	//如果滤镜存在则不添加
	list = pSheet->m_pFilterList;
	while(list)
	{
		if (id == list->id)
			goto ADD_FILTER_FINISH;
		list = list->next;
	}
	
	//保存滤镜
	if (!pSheet->m_pFilterList)
	{
		pSheet->m_pFilterList = (XPK_FILTER_LIST *)malloc(sizeof(XPK_FILTER_LIST));
		if (!pSheet->m_pFilterList)
		{
			AfxMessageBox("malloc error");
			return -__LINE__;
		}
		memset(pSheet->m_pFilterList,0,sizeof(XPK_FILTER_LIST));
		pSheet->m_pFilterList->id = id;
	}
	else
	{
		list = pSheet->m_pFilterList;
		while(list && list->next)
			list = list->next;

		list->next = (XPK_FILTER_LIST *)malloc(sizeof(XPK_FILTER_LIST));
		if (!list->next)
		{
			AfxMessageBox("malloc error");
			return -__LINE__;
		}
		memset(list->next,0,sizeof(XPK_FILTER_LIST));
		list->next->id = id;
	}

ADD_FILTER_FINISH:
	return 1;
}

int CXPKPreview::addMusic(char* szPath,int64_t timeline_start,int64_t timeline_end,
			 int64_t time_starat,int64_t time_end,int64_t fade_in,int64_t fade_out,float factor,int index)
{
	int ret = 0;
	Handle hAudio = NULL;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	hSlide = pSheet->hSlide;
	if (!hSlide)
	{
		AfxMessageBox("OnBnClickedAddMusic1 error");
		return -__LINE__;
	}
	OnBnClickedStop();
	

	hAudio = xpkAddAudioMedia(hSlide,szPath,factor,time_starat,time_end,timeline_start,timeline_end);
	if (!hAudio)
	{
		AfxMessageBox("xpkAddAudioMedia error ");
		return -__LINE__;
	}


	if (!pMusicList)
	{
		pMusicList = (XPK_MUSIC_LIST*)malloc(sizeof(XPK_MUSIC_LIST));
		if (!pMusicList)
			return -__LINE__;
		memset(pMusicList,0,sizeof(XPK_MUSIC_LIST));
		strcpy(pMusicList->path,szPath);
		pMusicList->fade_in = fade_in;
		pMusicList->fade_out = fade_out;
		pMusicList->factor = factor;
		pMusicList->end_time = time_end;
		pMusicList->hAudio = hAudio;
		pMusicList->index = index;
		pMusicList->start_time = time_starat;
		pMusicList->timeline_end = timeline_end;
		pMusicList->timeline_start = timeline_start;
	}
	else
	{
		XPK_MUSIC_LIST* list = pMusicList;
		while(list && list->next)
			list = list->next;
		list->next = (XPK_MUSIC_LIST*)malloc(sizeof(XPK_MUSIC_LIST));
		if (!list->next)
			return -__LINE__;
		memset(list->next,0,sizeof(list->next));
		strcpy(list->next->path,szPath);
		list->next->hAudio = hAudio;
		list->next->fade_in = fade_in;
		list->next->fade_out = fade_out;
		list->next->factor = factor;
		list->next->end_time = time_end;
		list->next->index = index;
		list->next->start_time = time_starat;
		list->next->timeline_end = timeline_end;
		list->next->timeline_start = timeline_start;
	}

	
	OnBnClickedPlay();
	return 1;

}

int64_t CXPKPreview::getTotalTime()
{
	int64_t total_time = 0;
	XPKWizard *pSheet=(XPKWizard *)GetParent();
	if (!pSheet)
		return 0;
	else
	{	
		if(!pSheet->hSlide)
			return 0;
		total_time = xpkGetTotalPlaybackTime(pSheet->hSlide);
		return total_time;
	}
	
}

int CXPKPreview::refreshTabCtrlUI()
{
	int64_t totalTime = 0;
	int curSel = 0;
	if(!hSlide)
		return 0;
	if (stop)
		return 0;

	totalTime = xpkGetTotalPlaybackTime(hSlide);

	curSel = m_tabctrl.GetCurSel();
	switch(curSel)
	{
	case 0: 
		m_addMusic.setTotalTime(totalTime);
		break; 
	case 1: 
		m_addApng.setTotalTime(totalTime);
		break;
	case 2:
		m_addSubtitle.setTotalTime(totalTime);
		break;
	}

	return 1;
}
void CXPKPreview::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int curSel = m_tabctrl.GetCurSel();
	
	switch(curSel) 
	{ 
	case 0: 
		m_addMusic.ShowWindow(SW_SHOW);
		m_addApng.ShowWindow(SW_HIDE);
		m_addSubtitle.ShowWindow(SW_HIDE);
		m_addFilter.ShowWindow(SW_HIDE);
		break; 
	case 1: 

		m_addMusic.ShowWindow(SW_HIDE);
		m_addApng.ShowWindow(SW_SHOW);
		m_addSubtitle.ShowWindow(SW_HIDE);
		m_addFilter.ShowWindow(SW_HIDE);
		break;
	case 2:
		m_addMusic.ShowWindow(SW_HIDE);
		m_addApng.ShowWindow(SW_HIDE);
		m_addSubtitle.ShowWindow(SW_SHOW);
		m_addFilter.ShowWindow(SW_HIDE);

		break;
	case  3:
		m_addMusic.ShowWindow(SW_HIDE);
		m_addApng.ShowWindow(SW_HIDE);
		m_addSubtitle.ShowWindow(SW_HIDE);
		m_addFilter.ShowWindow(SW_SHOW);

	} 
	refreshTabCtrlUI();

	*pResult = 0;
}

void CXPKPreview::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_button_stop_left_zoom )
	{
#if 1
		//更新控件
		int width = 0;
		int height = 0;
		RECT rect = {0};
		RECT showRect = {0};
		int display_top = 0;
		
		//播放按钮
		GetDlgItem(IDC_PLAY)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.right*m_button_play_left_zoom ;
		showRect.right = showRect.left + width;
		showRect.top = rect.bottom - 50;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_PLAY)->MoveWindow(&showRect);
		
		//停止按钮
		GetDlgItem(IDC_STOP)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.right*m_button_stop_left_zoom ;
		showRect.right = showRect.left + width;
		showRect.top = rect.bottom - 50;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_STOP)->MoveWindow(&showRect);
		
		//滑块
		pSlider->GetClientRect(&rect);
		height = rect.bottom - rect.top;

		GetClientRect(&rect);

		showRect.left =  rect.left + 50 ;
		showRect.right = rect.right - 50;
		showRect.top = rect.bottom*m_slider_top_zoom;
		showRect.bottom = showRect.top  + height;
		pSlider->MoveWindow(&showRect);
		

		//开始时间
		GetDlgItem(IDC_TIME_START)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.left + 10;//rect.right*m_static_start_time_zoom ;
		showRect.right = showRect.left + 40;//showRect.left + width;
		showRect.top = rect.bottom * m_static_start_time_top_zoom;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_TIME_START)->MoveWindow(&showRect);

		//结束时间
		GetDlgItem(IDC_TIME_END)->GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.right - 40;//rect.right*m_static_end_time_zoom ;
		showRect.right = showRect.left + 80;
		showRect.top = rect.bottom * m_static_end_time_top_zoom ;
		showRect.bottom = showRect.top  + height;
		GetDlgItem(IDC_TIME_END)->MoveWindow(&showRect);

		//显示区域
		GetClientRect(&rect);
		showRect.left =  rect.right*m_display_left_zoom ;
		showRect.right = showRect.left + rect.right*m_display_width_zoom;
		showRect.top = rect.bottom*m_display_top_zoom  ;
		showRect.bottom =  rect.right*m_display_width_zoom * (float)PREVIEW_HEIGHT/(float)PREVIEW_WIDTH + showRect.top;

		if ((showRect.right - showRect.left)%2 != 0)
			showRect.right -= 1;
		if((showRect.bottom - showRect.top)%2 != 0)
			showRect.bottom -= 1;

		GetDlgItem(IDC_SHOW_PICTURE)->MoveWindow(&showRect);
		display_top = showRect.top;

		//tabCtrl
		m_tabctrl.GetClientRect(&rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		GetClientRect(&rect);
		showRect.left =  rect.right*m_tabCtrl_left_zoom ;
		showRect.right = showRect.left + width;
		showRect.top = display_top;
		showRect.bottom = showRect.top  + height;
		m_tabctrl.MoveWindow(&showRect);
#endif

	}

}

void CXPKPreview::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


	XPK_MUSIC_LIST* pMusic = NULL;
	XPK_SUBTITLE_LIST* pSub = NULL;
	XPK_APNG_RGB_OVERLAY_LIST* pApng = NULL;
	if(pSlider)
		delete pSlider;
	if (hUIThread)
	{
		TerminateThread(hUIThread,0);
		WaitForSingleObject(hUIThread,INFINITE );
	}
	

	while(pSubtitleList)
	{
		pSub = pSubtitleList->next;
		free(pSubtitleList);
		pSubtitleList = pSub;
	}

	while(pMusicList)
	{
		pMusic = pMusicList->next;
		free(pMusicList);
		pMusicList = pMusic;
	}

	while(pApngList)
	{
		pApng = pApngList->next;
		free(pApngList);
		pApngList = pApng;
	}
	CPropertyPage::OnClose();
}
