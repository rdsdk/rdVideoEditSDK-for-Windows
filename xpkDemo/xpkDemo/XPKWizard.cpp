// XPKWizard.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "XPKWizard.h"


// XPKWizard

IMPLEMENT_DYNAMIC(XPKWizard, CPropertySheet)

XPKWizard::XPKWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

XPKWizard::XPKWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_addDlg);
	AddPage(&m_previewDlg);
	AddPage(&m_editDlg);

	total_time = NULL;
	hSlide = NULL;
	m_pMediaList = NULL;
	statu = XKP_NONE;
	m_pFilterList = NULL;
	m_button_next_zoom = 0.0;
	m_button_back_zoom = 0.0;
}

XPKWizard::~XPKWizard()
{
}


BEGIN_MESSAGE_MAP(XPKWizard, CPropertySheet)
	ON_WM_SIZE()
	ON_MESSAGE (WM_RESIZEPAGE, OnResizePage)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// XPKWizard 消息处理程序

BOOL XPKWizard::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO:  在此添加您的专用代码
	RECT rect  = {0};
	RECT button_rect  = {0};
	//隐藏取消按钮
	CButton* btnFinish = reinterpret_cast<CButton*>(GetDlgItem(IDCANCEL));
	btnFinish->ShowWindow(FALSE);
	//隐藏帮助按钮
	CButton* btnClose = reinterpret_cast<CButton*>(GetDlgItem(IDHELP));
	btnClose->ShowWindow(FALSE);


	//计算缩放比例
	GetClientRect(&rect);
	GetDlgItem(ID_WIZBACK)->GetWindowRect(&button_rect);
	ScreenToClient(&button_rect);
	m_button_back_zoom = (float)button_rect.left/(float)rect.right;

	GetDlgItem(ID_WIZNEXT)->GetWindowRect(&button_rect);
	ScreenToClient(&button_rect);
	m_button_next_zoom = (float)button_rect.left/(float)rect.right;

	GetDlgItem(ID_WIZFINISH)->GetWindowRect(&button_rect);
	ScreenToClient(&button_rect);
	m_button_finish_zoom = (float)button_rect.left/(float)rect.right;


 	ModifyStyle(0, WS_TABSTOP | TCS_BOTTOM);
	// 隐藏分割线
	for (HWND hChildWnd = ::GetTopWindow(m_hWnd); hChildWnd != NULL; hChildWnd = ::GetNextWindow(hChildWnd, GW_HWNDNEXT)) 
	{
		CWnd* pWnd = CWnd::FromHandle(hChildWnd);
		if (pWnd != NULL) 
		{
			
			pWnd->GetClientRect(&rect);
			if (rect.bottom == 0 && rect.right)
				pWnd->ShowWindow(SW_HIDE);
		}
	}	
	return bResult;
}

char* XPKWizard::GetResourcePath(char* name)
{
	char Path[MAX_PATH + 1];
	GetModuleFileName(NULL, Path, MAX_PATH);
	memset(szFilePath,0,MAX_PATH);
	memcpy(szFilePath,Path,strlen(Path)-12);
	strcat(szFilePath,name);
	return szFilePath;
}

int XPKWizard::addApngList()
{
	int ret = 0;
	Handle hMedia = NULL;
	XPK_APNG_RGB_OVERLAY_LIST* pList = m_previewDlg.pApngList;
	if (!pList)
		return 1;

	while(pList)
	{
		hMedia = xpkAddMedia(hSlide,pList->szPath,pList->timeline_start,pList->timeline_end,pList->time_start,pList->time_end);
		if(!hMedia)
			return -__LINE__;
		pList = pList->next;
	}
	return 1;

}
int XPKWizard::addSubtitle()
{
	int ret = 0;
	Handle hSubtitle = NULL;
	XPK_SUBTITLE_LIST* pList = m_previewDlg.pSubtitleList;
	if (!pList)
		return 1;
	while(pList)
	{
		if(pList->szText && strlen(pList->szText))
			hSubtitle = xpkAddSubtitleText(hSlide,pList->szText);
		else
			hSubtitle = xpkSubtitleStrPath(hSlide,pList->srtPath);
		if (!hSubtitle)
		{
			return -__LINE__;
		}
		ret = xpkSetSubtitleTime(hSlide,hSubtitle,pList->timeline_start,pList->timeline_end);
		if (ret <= 0)
		{
			return ret;
		}
		ret = xpkSetSubtitleRect(hSlide,hSubtitle,pList->rect);
		if (ret <= 0)
		{
			return ret;
		}
		ret = xpkSetSubtitleFont(hSlide,hSubtitle,pList->font);
		if (ret <= 0)
		{
			return ret;
		}
		ret = xpkSetSubTitleEffect(hSlide,hSubtitle,pList->effect,pList->clr);
		if (ret <= 0)
		{
			return ret;
		}
		pList = pList->next;
	}
	return 1;
}
int XPKWizard::adddubmusic()
{
	Handle hAudio = NULL;
	int ret = 0;
	int64_t start_time = 0;
	int64_t end_time = 0;
	int64_t timeline_start = 0;
	int64_t timeline_end = 0;
	int64_t fade_in_time = 0;
	int64_t fade_out_time = 0;
	CString str = NULL;
	
	XPK_MUSIC_LIST* pList = m_previewDlg.pMusicList;

	if(!pList)
		return 1;
	
	while(pList)
	{
		hAudio = xpkAddAudioMedia(hSlide,pList->path,pList->factor,pList->start_time,
			pList->end_time,pList->timeline_start,pList->timeline_end);
		if (!hAudio)
		{
			AfxMessageBox("xpkAddAudioMedia error ");
			return -__LINE__;
		}

		pList = pList->next;
	}
	
	return 1;

}
int XPKWizard::getClipRect(XPK_MEDIA_LIST* pList,RECTF* rtClip)
{
	if (MEDIA_PICTURE == pList->type)
	{
		RECTF rtClip1 = pList->pPictureDlg->getMediaClip();
//		memcpy(rtClip,pList->pPictureDlg->getMediaClip(),sizeof(RECTF));
// 		rtClip->left = pList->pPictureDlg->rtClip.left;
// 		rtClip->top = pList->pPictureDlg->rtClip.top;
// 		rtClip->right = pList->pPictureDlg->rtClip.right;
// 		rtClip->bottom = pList->pPictureDlg->rtClip.bottom;
	}
	else
	{
		RECTF rtClip1 = pList->pMediaDlg->getMediaClip();
// 		rtClip->left = pList->pMediaDlg->rtClip.left;
// 		rtClip->top = pList->pMediaDlg->rtClip.top;
// 		rtClip->right = pList->pMediaDlg->rtClip.right;
// 		rtClip->bottom = pList->pMediaDlg->rtClip.bottom;
	}
	
	return 1;
}

int XPKWizard::clearAllFilter()
{
	while (m_pFilterList)
	{
		XPK_FILTER_LIST* temp = m_pFilterList->next;
		free(m_pFilterList);
		m_pFilterList = temp;
	}

	m_pFilterList = NULL;
	return 1;
}

int XPKWizard::init()
{
	char buf[1024];
	CString str = NULL;
	XPK_MEDIA_LIST* pList = m_addDlg.m_pMediaList;
	int transition = 0;
	int flip = 0;
	int angle = 0;
	int clip_type = 0;
	char path[MAX_PATH];
	int first = 0;
	int ret = 0;
	int64_t start_time = 0;
	int64_t end_time = 0;
	int64_t timeline_start = 0;
	int64_t timeline_end = 0;
	int64_t fade_in_time = 0;
	int64_t fade_out_time = 0;
	RECTF rtClip = {0};
	double speed = 0.0;
	int64_t cut_start = 0;
	int64_t cut_end = 0;
	int size = 0;
	XPK_FILTER_LIST* filter_list = NULL;
	XPK_DYNAMAIC_CLIP_LIST *clip_list  = NULL;

	//检查当前状态
	if (hSlide && statu != XKP_NONE)
		xpkCloseMediaEditor(hSlide);
	hSlide = xpkCreate();
	if(!hSlide)
		return -__LINE__;
	
	//初始化总时间
	total_time = 0;

	if(!pList)
		return -__LINE__;
	
	while(pList)
	{
		filter_list = m_pFilterList;

		memset(&rtClip,0,sizeof(RECTF));

		strcpy(path,pList->path);
		if (MEDIA_PICTURE == pList->type)
		{
			//旋转角度
			angle = pList->pPictureDlg->getMediaRotateAngle();
			//翻转方式
			flip = pList->pPictureDlg->getMediaFileType();
			//截取时间
			start_time = 0;
			end_time = pList->total_time;
			//调速
			speed = pList->pPictureDlg->getMediaSpeed();
			//裁剪区域
			rtClip = pList->pPictureDlg->getMediaClip();
		}
		else
		{
			//旋转角度
			angle = pList->pMediaDlg->getMediaRotateAngle();
			//翻转方式
			flip = pList->pMediaDlg->getMediaFileType();
			//截取时间
			start_time = pList->pMediaDlg->getMediaCutStartTime();
			end_time = pList->pMediaDlg->getMediaCutEndTime();
			//调速
			speed = pList->pMediaDlg->getMediaSpeed();
			//裁剪区域
			rtClip = pList->pMediaDlg->getMediaClip();
			//动态裁剪区域
			clip_list = pList->pMediaDlg->getDynamicClipList();
		}
		
		//转场
		pList->transition_box->GetWindowText(str);
		transition= pList->transition_box->GetCurSel();//(enum TRANSITION_TYPE)_ttoi(str);


		timeline_start = total_time;
		timeline_end = total_time + pList->total_time/speed;
		if (end_time - start_time < pList->total_time )
		{
			//只截取一部分
			timeline_end = timeline_start + (end_time - start_time)/speed;
		}

		Handle hMedia = xpkAddMedia(hSlide,pList->path,timeline_start,timeline_end,start_time,end_time);
		if(!hMedia)
			return -__LINE__;
		sprintf(buf,"path : %s  timeline_start = %lld timeline_end = %lld transiton:%d  \r\n ",
			pList->path,timeline_start,timeline_end,transition);
		OutputDebugString(buf);

		ret = xpkSetMediaRotateAngle(hSlide,hMedia,angle);
		if (ret <= 0)
			return ret;

		ret = xpkSetMediaFlip(hSlide,hMedia,flip);
		if (ret <= 0)
			return ret;

		ret = xpkSetMediaClip(hSlide,hMedia,rtClip);
		if (ret <= 0)
			return ret;
#if 0
		if (0 == timeline_start)
		{
			ret = xpkSetMediaFilter(hSlide,hMedia,VFT_ACV_LOMO);
			if (ret <= 0)
				return ret;
		}
		else
		{
			ret = xpkSetMediaFilter(hSlide,hMedia,VFT_ACV_LOMO_1);
			if (ret <= 0)
				return ret;
		}
#else
		while(filter_list)
		{
			ret = xpkSetMediaFilter(hSlide,hMedia,filter_list->id);
			if (ret <= 0)
				return ret;
			filter_list = filter_list->next;
		}	
#endif

//		xpkSetMediaFilter(hSlide,hMedia,VFT_BEAUTY);
		
		ret = xpkSetMediaTransition(hSlide,hMedia,transition);
		if (ret <= 0)
			return ret;

//		ret = xpkSetMediaPlaySpeed(hSlide,hMedia,speed);
		if (ret <= 0)
			return ret;

		while (clip_list)
		{
			sprintf(buf,"crop list : left = %f  right = %f top = %f bottom = %f start_time:%d end_time:%d\r\n ",clip_list->rect.left,
				clip_list->rect.right,clip_list->rect.top,clip_list->rect.bottom,clip_list->time_start,clip_list->time_end);
			OutputDebugString(buf);

			ret = xpkSetMediaDynamicClip(hSlide,hMedia,clip_list->time_start,clip_list->time_end,clip_list->rect);
			if (ret <= 0)
				return ret;
			clip_list = clip_list->next;
		}	
		total_time = timeline_end;
		pList = pList->next;
	}

	//添加特效
	if((ret = addApngList())<=0)
	{
		AfxMessageBox("addApngList error");
		return ret;
	}
	//添加配乐
	if((ret = adddubmusic())<=0)
	{
		AfxMessageBox("adddubmusic error");
		return ret;
	}
	//添加字幕
	if((ret = addSubtitle())<=0)
	{
		AfxMessageBox("addSubtitle error");
		return ret;
	}


	return 1;
}
void XPKWizard::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize(nType, cx, cy);
	
	CPropertyPage *page = NULL;
#if DIALOG_SIZE_CHANGE

	int width = 0;
	int height = 0;
	RECT dlg_rect = {0};
	RECT page_rect = {0};
	RECT button_rect = {0};
	GetClientRect(&dlg_rect);
	page_rect = dlg_rect;
	page_rect.bottom = page_rect.bottom - 50;

	//更新子页面的大小
	page =(CPropertyPage*)  GetActivePage();
	if (page)
	{
		int index = GetActiveIndex();
		if (0 == index)
			m_addDlg.MoveWindow(&page_rect);
		else if (1 == index)
			m_previewDlg.MoveWindow(&page_rect);
		else if (2 == index)
			m_editDlg.MoveWindow(&page_rect);

		//更新对话框控件的大小
		GetDlgItem(ID_WIZBACK)->GetClientRect(&button_rect);
		width = button_rect.right - button_rect.left;
		height = button_rect.bottom - button_rect.top;
		button_rect.left = dlg_rect.right*m_button_back_zoom;
		button_rect.right = button_rect.left + width;
		button_rect.bottom = dlg_rect.bottom - 20;
		button_rect.top = button_rect.bottom - height;
		GetDlgItem(ID_WIZBACK)->MoveWindow(&button_rect);

		GetDlgItem(ID_WIZNEXT)->GetClientRect(&button_rect);
		width = button_rect.right - button_rect.left;
		height = button_rect.bottom - button_rect.top;
		button_rect.left = dlg_rect.right*m_button_next_zoom;
		button_rect.right = button_rect.left + width;
		button_rect.bottom = dlg_rect.bottom - 20;
		button_rect.top = button_rect.bottom - height;
		GetDlgItem(ID_WIZNEXT)->MoveWindow(&button_rect);

		GetDlgItem(ID_WIZFINISH)->GetClientRect(&button_rect);
		width = button_rect.right - button_rect.left;
		height = button_rect.bottom - button_rect.top;
		button_rect.left = dlg_rect.right*m_button_finish_zoom;
		button_rect.right = button_rect.left + width;
		button_rect.bottom = dlg_rect.bottom - 20;
		button_rect.top = button_rect.bottom - height;
		GetDlgItem(ID_WIZFINISH)->MoveWindow(&button_rect);
	}

#endif

	// TODO: 在此处添加消息处理程序代码
}
LRESULT XPKWizard::OnResizePage(WPARAM wParam, LPARAM lParam)
{

	return 1;
}

BOOL XPKWizard::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	
	// TODO: 在此添加专用代码和/或调用基类
	

	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

void XPKWizard::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CPropertySheet::OnClose();
}
