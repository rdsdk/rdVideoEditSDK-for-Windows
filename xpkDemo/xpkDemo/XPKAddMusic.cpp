// XPKAddMusic.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "XPKAddMusic.h"
#include "XPKWizard.h"


// CXPKAddMusic 对话框

IMPLEMENT_DYNAMIC(CXPKAddMusic, CPropertyPage)

CXPKAddMusic::CXPKAddMusic()
	: CPropertyPage(CXPKAddMusic::IDD)
{
	m_pMusicList = NULL;
	m_change = 0;
	hSlide = NULL;
}

CXPKAddMusic::~CXPKAddMusic()
{
	XPK_MUSIC_LIST* pList = m_pMusicList;
	while(pList)
	{
		pList = m_pMusicList->next;
		delete m_pMusicList->button;
		delete m_pMusicList->pAudioDlg;
		delete m_pMusicList->snapshot;
		if(m_pMusicList->pImage)
			av_free(m_pMusicList->pImage);
		av_free(m_pMusicList);
		m_pMusicList = pList;
	}
}

void CXPKAddMusic::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CXPKAddMusic, CPropertyPage)
	ON_BN_CLICKED(IDC_ADD_MUSIC, &CXPKAddMusic::OnBnClickedAddMusic)
	ON_BN_CLICKED(IDC_ADD_MUSIC4, &CXPKAddMusic::OnBnClickedDeleteMusic)
	ON_BN_CLICKED(IDC_DELETE_ALL_MUSIC, &CXPKAddMusic::OnBnClickedDeleteAllMusic)
END_MESSAGE_MAP()


// CXPKAddMusic 消息处理程序

LRESULT CXPKAddMusic::OnWizardBack()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CPropertyPage::OnWizardBack();
}

LRESULT CXPKAddMusic::OnWizardNext()
{
	// TODO: 在此添加专用代码和/或调用基类
	int ret = 0;
	int64_t start_time = 0;
	int64_t end_time = 0;
	int64_t timeline_start = 0;
	int64_t timeline_end = 0;
	int64_t fade_in_time = 0;
	int64_t fade_out_time = 0;
	CString str = NULL;
	XPKWizard *pSheet = (XPKWizard*)GetParent();
	XPK_MUSIC_LIST* pList = m_pMusicList;
	hSlide = pSheet->hSlide;
	while(pList)
	{
		if ( (m_change || pList->pAudioDlg->m_change) && pList->hAudio)
		{
			start_time = pList->pAudioDlg->getMusicCutStartTime();
			end_time = pList->pAudioDlg->getMusicCutEndTime();
			if(end_time == 0)
				end_time = pList->timeline_end;

			//混音因子
			pList->pAudioDlg->m_music_factor->GetWindowText(str);
			pList->factor = atof(str);

			//淡入
			pList->pAudioDlg->m_music_fade_in->GetWindowText(str);
			pList->fade_in = _ttoi(str);
			//淡出
			pList->pAudioDlg->m_music_fade_out->GetWindowText(str);
			pList->fade_out = _ttoi(str);
			//速度
			pList->pAudioDlg->m_music_speed->GetWindowText(str);
			pList->speed = atof(str);

			//默认播放的开始时间和结束时间
			timeline_start = 0;
			
			ret = xpkSetAudioMediaVolFactor(hSlide,pList->hAudio,pList->factor);
			if (ret <= 0)
			{
				AfxMessageBox("xpkSetAudioMediaVolFactor error !");
				return 0;
			}
			ret = xpkSetAudioMediaPlaySpeed(hSlide,pList->hAudio,pList->speed);
			if (ret <= 0)
			{
				AfxMessageBox("xpkSetAudioMediaPlaySpeed error !");
				return 0;
			}
			ret = xpkSetAudioMediaFadeTime(hSlide,pList->hAudio,pList->fade_in,pList->fade_out);
			if (ret <= 0)
			{
				AfxMessageBox("xpkSetAudioMediaFadeTime error !");
				return 0;
			}
		}
		pList = pList->next;
	}
	m_change = 0;
	return CPropertyPage::OnWizardNext();
}

BOOL CXPKAddMusic::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类

	CPropertySheet *pSheet=(CPropertySheet *)GetParent();
	ASSERT_KINDOF(CPropertySheet,pSheet);
	pSheet->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK);//ID_WIZNEXT
	//pSheet->GetDlgItem(ID_WIZNEXT)->SetWindowText("next");
	//pSheet->GetDlgItem(ID_WIZBACK)->SetWindowText("back");
	pSheet->GetDlgItem(ID_WIZBACK)->ShowWindow(SW_SHOW);
	return CPropertyPage::OnSetActive();
}

BOOL CXPKAddMusic::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* btnFinish = reinterpret_cast<CButton*>(GetDlgItem(IDCANCEL));
	btnFinish->ShowWindow(FALSE);

	CButton* btnClose = reinterpret_cast<CButton*>(GetDlgItem(IDOK));
	btnClose->ShowWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
//显示与缩略图对应的设置对话框
int CXPKAddMusic::showAdvanceDlg(int id)
{
	XPK_MUSIC_LIST* pList = m_pMusicList;
	while (pList)
	{
		if (id == pList->pAudioDlg->m_resouceId)
		{
			pList->pAudioDlg->refreshUI();
			break;
		}
		pList = pList->next;
	}
	return 1;
}
int CXPKAddMusic::addMusic(char* path,int64_t total_time)
{
	XPKWizard *pSheet = (XPKWizard*)GetParent();
	CRect rect;
	int ret = 0;
	int width = SNAPSHOT_WIDTH;
	int height = SNAPSHOT_HEIGHT;
	XPK_MUSIC_LIST* pTemp = NULL;

	if(!m_pMusicList)
	{
		m_pMusicList = (XPK_MUSIC_LIST*)av_mallocz(sizeof(XPK_MUSIC_LIST));
		if(!m_pMusicList)
			return -__LINE__;

		memset(m_pMusicList,0,sizeof(XPK_MUSIC_LIST));
		m_pMusicList->id++;
		strcpy(m_pMusicList->path,path);
		m_pMusicList->start_time = 0;
		m_pMusicList->end_time = total_time;
		
		m_pMusicList->speed = 1;

		//设置缩略图显示区域		
		m_pMusicList->snapshot_rect.left	= SNAPSHOT_INTERSPACE;
		m_pMusicList->snapshot_rect.top		= SNAPSHOT_INTERSPACE + SNAPSHOT_INTERSPACE;
		m_pMusicList->snapshot_rect.right	= SNAPSHOT_INTERSPACE + SNAPSHOT_WIDTH;
		m_pMusicList->snapshot_rect.bottom	= m_pMusicList->snapshot_rect.top + SNAPSHOT_HEIGHT;

		m_pMusicList->snapshot = new CShowSnapshot;
		m_pMusicList->snapshot->Create("",WS_CHILD|WS_VISIBLE|SS_GRAYFRAME|SS_NOTIFY,m_pMusicList->snapshot_rect,this);
		m_pMusicList->snapshot->init();
		m_pMusicList->snapshot->SetDlgCtrlID(RESOURCE_ID+m_pMusicList->id+MUSICPAGE_CTRL_ID);

		int w = CBUTTON_WIDTH;
		int h = CBUTTON_HEIGHT;
		m_pMusicList->button_rect.left		= m_pMusicList->snapshot_rect.left + (m_pMusicList->snapshot_rect.right - m_pMusicList->snapshot_rect.left - w)/2;
		m_pMusicList->button_rect.right		= m_pMusicList->snapshot_rect.right-(m_pMusicList->snapshot_rect.right - m_pMusicList->snapshot_rect.left - w)/2;

		m_pMusicList->button_rect.top		= m_pMusicList->snapshot_rect.bottom + 10;
		m_pMusicList->button_rect.bottom	= m_pMusicList->snapshot_rect.bottom + 30;

		m_pMusicList->button	 = new CSetButton;
		m_pMusicList->button->Create("....",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,m_pMusicList->button_rect,this,RESOURCE_ID*2+m_pMusicList->id+MUSICPAGE_CTRL_ID);

		//创建对话框
		m_pMusicList->pAudioDlg = new CAdvancedSettingAudioDialog(RESOURCE_ID*3+m_pMusicList->id+MUSICPAGE_CTRL_ID);
		m_pMusicList->pAudioDlg->Create(IDD_ADVANCEDSETTINGAUDIO,this);
		pTemp = m_pMusicList;
	}
	else
	{
		XPK_MUSIC_LIST* list = m_pMusicList;
		int index_w = 0;
		int index_h = 0;
		int nMaxIndex = 0;
		CRect rect;
		GetClientRect(&rect);

		nMaxIndex = rect.right / (SNAPSHOT_INTERSPACE + SNAPSHOT_WIDTH);

		while(list&&list->next)
			list = list->next;
		list->next = (XPK_MUSIC_LIST*)av_mallocz(sizeof(XPK_MUSIC_LIST));
		if(!list->next)
			return -__LINE__;

		memset(list->next,0,sizeof(XPK_MUSIC_LIST));
		list->next->id = list->id+1;
		list->next->height = height;
		list->next->width = width;
		list->next->start_time = 0;
		list->next->end_time = total_time;
		
		list->next->speed = 1;
		strcpy(list->next->path,path);
		list->next->pImage = (uint8_t* )av_mallocz(list->next->width*list->next->height*4);
		if(0 == list->next->id%nMaxIndex )
		{
			index_w = nMaxIndex;
			index_h = list->next->id/nMaxIndex;
		}
		else
		{
			index_w = list->next->id - list->next->id/nMaxIndex*nMaxIndex;
			index_h = list->next->id/nMaxIndex + 1 ;
		}
		//设置显示区域
		list->next->snapshot_rect.left		= SNAPSHOT_INTERSPACE*index_w + SNAPSHOT_WIDTH*(index_w - 1);
		list->next->snapshot_rect.top		= SNAPSHOT_INTERSPACE*index_h + SNAPSHOT_HEIGHT*(index_h -1) + SNAPSHOT_INTERSPACE;
		list->next->snapshot_rect.right		= (SNAPSHOT_INTERSPACE + SNAPSHOT_WIDTH)*index_w;
		list->next->snapshot_rect.bottom	= (SNAPSHOT_INTERSPACE + SNAPSHOT_HEIGHT)*index_h + SNAPSHOT_INTERSPACE;

		list->next->snapshot  = new CShowSnapshot;
		list->next->snapshot->Create("",WS_CHILD|WS_VISIBLE|SS_GRAYFRAME|SS_NOTIFY,list->next->snapshot_rect,this);
		list->next->snapshot->init();
		list->next->snapshot->SetDlgCtrlID(RESOURCE_ID+list->next->id+MUSICPAGE_CTRL_ID);

		//memcpy(list->next->pImage,pImage,width*height*4);
		//list->next->snapshot->SetSnapshot((int8_t*)list->next->pImage,list->next->width,list->next->height);

		//设置特效选择组合框
		int w = CBUTTON_WIDTH;
		int h = CBUTTON_HEIGHT;
		list->next->button_rect.left	= list->next->snapshot_rect.left + (list->next->snapshot_rect.right - list->next->snapshot_rect.left - w)/2;
		list->next->button_rect.right	= list->next->snapshot_rect.right-(list->next->snapshot_rect.right - list->next->snapshot_rect.left - w)/2;

		list->next->button_rect.top		= list->next->snapshot_rect.bottom + 10;
		list->next->button_rect.bottom	= list->next->snapshot_rect.bottom + 30;

		list->next->button	 = new CSetButton;
		list->next->button->Create("....",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,list->next->button_rect,this,RESOURCE_ID*2+list->next->id+MUSICPAGE_CTRL_ID);

		//创建对话框
		list->next->pAudioDlg = new CAdvancedSettingAudioDialog(RESOURCE_ID*3+list->next->id+MUSICPAGE_CTRL_ID);
		list->next->pAudioDlg->Create(IDD_ADVANCEDSETTINGAUDIO,this);
		pTemp = list->next;
	}


	
	m_change = 1;
	return 1;
}
void CXPKAddMusic::OnBnClickedAddMusic()
{
	// TODO: 在此添加控件通知处理程序代码
	int ret = 0;
	char path[MAX_PATH];
	int64_t total_time = 0;
	//CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("位图文件(*.BMP;*.jpg;*mp4;*.mov)|*.BMP;*.jpg;*mp4;*.mov|"));

	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ALLOWMULTISELECT,_T("音频文件(*.mp3;*.avi;*mp4;*.mov)|*.mp3;*.avi;*mp4;*.mov|"));
	TCHAR *pBuffer = new TCHAR[MAX_PATH    * 20];//最多允许同时打开20个文件
	dlg.m_ofn.lpstrFile = pBuffer;
	dlg.m_ofn.nMaxFile = MAX_PATH * 20;
	dlg.m_ofn.lpstrFile[0] = '\0';

	if (dlg.DoModal() == IDOK)
	{
		POSITION pos = dlg.GetStartPosition();
		while (pos != NULL)
		{
			//取得文件路径
			Handle hAudio = NULL;
			strcpy(path,dlg.GetNextPathName(pos));
			OutputDebugString("\n");
			OutputDebugString(path);

			hAudio = AudioLoad(path,0,0);
			if(!hAudio)
			{
				OutputDebugString("AudioLoad error!");
				AfxMessageBox("AudioLoad error!");
				return ;
			}
			total_time = AudioGetDuration(hAudio);
			AudioClose(hAudio);
			if(addMusic(path,total_time)<=0)
			{
				delete pBuffer;
				return ;
			}
		}
	}
}
//显示缩略图
int CXPKAddMusic::refreshSnapshot()
{
	XPK_MUSIC_LIST *list = m_pMusicList;
	int index = 0;
	int index_w = 0;
	int index_h = 0;
	int nMaxIndex = 0;
	CRect rect;
	GetClientRect(&rect);
	nMaxIndex = rect.right / (SNAPSHOT_INTERSPACE + SNAPSHOT_WIDTH);
	while(list)
	{
		index++;
		list->id = index;

		if(0 == list->id%nMaxIndex )
		{
			index_w = nMaxIndex;
			index_h = list->id/nMaxIndex;
		}
		else
		{
			index_w = list->id - list->id/nMaxIndex*nMaxIndex;
			index_h = list->id/nMaxIndex + 1 ;
		}
		//设置显示区域
		list->snapshot_rect.left		= SNAPSHOT_INTERSPACE*index_w + SNAPSHOT_WIDTH*(index_w - 1);
		list->snapshot_rect.top			= SNAPSHOT_INTERSPACE*index_h + SNAPSHOT_HEIGHT*(index_h -1) + SNAPSHOT_INTERSPACE;
		list->snapshot_rect.right		= (SNAPSHOT_INTERSPACE + SNAPSHOT_WIDTH)*index_w;
		list->snapshot_rect.bottom		= (SNAPSHOT_INTERSPACE + SNAPSHOT_HEIGHT)*index_h + SNAPSHOT_INTERSPACE;
		list->snapshot->MoveWindow(&list->snapshot_rect);

		int width  = CBUTTON_WIDTH;
		int height = CBUTTON_HEIGHT;
		list->button_rect.left		= list->snapshot_rect.left + (list->snapshot_rect.right - list->snapshot_rect.left - width)/2;
		list->button_rect.right		= list->snapshot_rect.right-(list->snapshot_rect.right - list->snapshot_rect.left - width)/2;

		list->button_rect.top		= list->snapshot_rect.bottom + 10;
		list->button_rect.bottom	= list->snapshot_rect.bottom + 30;
		list->button->MoveWindow(&list->button_rect);
		list = list->next;
	}
	return 1;
}
void CXPKAddMusic::OnBnClickedDeleteMusic()
{
	// TODO: 在此添加控件通知处理程序代码
	XPKWizard *pSheet = (XPKWizard*)GetParent();
	XPK_MUSIC_LIST* list = m_pMusicList;
	XPK_MUSIC_LIST* temp = NULL;
	hSlide = pSheet->hSlide;
	if(!list)
		return;
	if (list&&list->next)
	{
		while(list&&list->next)
		{
			if(list->snapshot->m_selected && list->id == 1)
			{
				temp = m_pMusicList->next;
				if(m_pMusicList->pImage)
					av_free(m_pMusicList->pImage);
				delete(m_pMusicList->snapshot);
				delete(m_pMusicList->button);
				delete(m_pMusicList->pAudioDlg);
				if(m_pMusicList->hAudio&&hSlide)
					xpkClearAudioMedia(hSlide,m_pMusicList->hAudio);
				av_free(m_pMusicList);
				m_pMusicList = NULL;
				m_pMusicList = temp;
				list = m_pMusicList;
			}
			else if (list->next->snapshot->m_selected)
			{
				temp = list->next->next;
				if(list->next->pImage)
					av_free(list->next->pImage);
				delete(list->next->snapshot);
				delete(list->next->button);
				delete(list->next->pAudioDlg);
				if(list->next->hAudio&&hSlide)
					xpkClearAudioMedia(hSlide,list->next->hAudio);
				av_free(list->next);
				list->next = NULL;
				list->next = temp;
			}
			else
				list = list->next;
		}
	}
	else
	{
		if (m_pMusicList->snapshot->m_selected)
		{
			av_free(m_pMusicList->pImage);
			delete(m_pMusicList->snapshot);
			delete(m_pMusicList->button);
			delete(m_pMusicList->pAudioDlg);
			if(m_pMusicList->hAudio&&hSlide)
				xpkClearAudioMedia(hSlide,m_pMusicList->hAudio);
			av_free(m_pMusicList);
			m_pMusicList = NULL;
		}
	}
	//刷新显示界面
	refreshSnapshot();
	Invalidate();
	m_change = 1;
}

void CXPKAddMusic::OnBnClickedDeleteAllMusic()
{
	// TODO: 在此添加控件通知处理程序代码
	XPKWizard *pSheet = (XPKWizard*)GetParent();
	XPK_MUSIC_LIST* pList = NULL;

	while(m_pMusicList)
	{
		pList = m_pMusicList->next;
		av_free(m_pMusicList->pImage);
		delete m_pMusicList->button;
		delete m_pMusicList->pAudioDlg;
		delete m_pMusicList->snapshot;
		av_free(m_pMusicList);
		m_pMusicList = pList;
	}
	hSlide = pSheet->hSlide;
	if(hSlide)
		xpkClearAllAudioMedias(hSlide);
	m_change = 1;
}
