// AddMusicDialg.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "AddMusicDialg.h"
#include "XPKPreview.h"


// CAddMusicDialg 对话框

IMPLEMENT_DYNAMIC(CAddMusicDialg, CDialog)

CAddMusicDialg::CAddMusicDialg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddMusicDialg::IDD, pParent)
{
	m_music_fade_in = NULL;
	m_music_fade_out = NULL;
	m_music_factor = NULL;
	music_id = 0;
	pMusic = NULL;

}

CAddMusicDialg::~CAddMusicDialg()
{
	if(m_music_fade_in)
	{
		delete m_music_fade_in;
		m_music_fade_in = NULL;
	}
	if(m_music_fade_out)
	{
		delete m_music_fade_out;
		m_music_fade_out = NULL;
	}
	if(m_music_factor)
	{
		delete m_music_factor;
		m_music_factor = NULL;
	}
}

void CAddMusicDialg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, listCtrl);
}


BEGIN_MESSAGE_MAP(CAddMusicDialg, CDialog)
	
	ON_BN_CLICKED(IDC_MUSISC_SAVE, &CAddMusicDialg::OnBnClickedChooseMusicFinish)
	ON_BN_CLICKED(IDC_CHOOSE_MUSIC, &CAddMusicDialg::OnBnClickedChooseMusic)
	ON_BN_CLICKED(IDC_RADIO_MINGYUE, &CAddMusicDialg::OnBnClickedRadioMOON)
	ON_BN_CLICKED(IDC_RADIO_YU, &CAddMusicDialg::OnBnClickedRadioFISH)
	ON_BN_CLICKED(IDC_CLEAR_ALL_MUSIC, &CAddMusicDialg::OnBnClickedClearAllMusic)
	ON_BN_CLICKED(IDC_DELETE_MUSIC, &CAddMusicDialg::OnBnClickedDeleteMusic)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CAddMusicDialg::OnNMClickList1)
END_MESSAGE_MAP()


// CAddMusicDialg 消息处理程序

void CAddMusicDialg::set_DropDownHight(CComboBox*box,UINT LinesToDisplay)
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
void CAddMusicDialg::set_DropDownWidth(CComboBox*box,UINT nWidth )
{
	box->SetDroppedWidth(nWidth);
}

int CAddMusicDialg::setTotalTime(int64_t totalTime)
{
	char buf[1024];
	sprintf(buf,"%lld",totalTime);
	GetDlgItem(IDC_MUSIC_TIMELINE_END)->SetWindowText(buf);
	return 1;
}
BOOL CAddMusicDialg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char buf[MAX_PATH];
	int64_t total_time = 0;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	RECT rect;
	GetClientRect(&rect);
	m_music_factor = new CXPKCombobox(COMBOBOX_MUSIC_PROPERTY);
	rect.left = 110;
	rect.right = rect.left + COMBOBOX_MUSIC_PROPERTY_WIDTH;
	rect.top =  110;
	rect.bottom = rect.top + COMBOBOX_MUSIC_PROPERTY_HEIGHT;

	//混音因子
	m_music_factor->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	set_DropDownHight(m_music_factor,5);
	set_DropDownWidth(m_music_factor,100);

	for (int i = 0;i<=10;i++)
	{
		sprintf(buf,"%0.1f",i/(float)10);
		m_music_factor->InsertString(i,buf);
	}
	m_music_factor->SetCurSel(5);

	//淡入
	m_music_fade_in = new CXPKCombobox(COMBOBOX_MUSIC_PROPERTY);
	rect.left = 110;
	rect.right = rect.left + COMBOBOX_MUSIC_PROPERTY_WIDTH;
	rect.top =  110 + COMBOBOX_MUSIC_PROPERTY_HEIGHT + 10;
	rect.bottom = rect.top + COMBOBOX_MUSIC_PROPERTY_HEIGHT;
	m_music_fade_in->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	set_DropDownHight(m_music_fade_in,5);
	set_DropDownWidth(m_music_fade_in,100);
	m_music_fade_in->InsertString(0,"0");
	m_music_fade_in->InsertString(1,"300000");
	m_music_fade_in->InsertString(2,"600000");
	m_music_fade_in->SetCurSel(1);

	//淡出
	m_music_fade_out = new CXPKCombobox(COMBOBOX_MUSIC_PROPERTY);
	rect.left = 110;
	rect.right = rect.left + COMBOBOX_MUSIC_PROPERTY_WIDTH;
	rect.top =  110 + COMBOBOX_MUSIC_PROPERTY_HEIGHT * 3;
	rect.bottom = rect.top + COMBOBOX_MUSIC_PROPERTY_HEIGHT;
	m_music_fade_out->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	set_DropDownHight(m_music_fade_out,5);
	set_DropDownWidth(m_music_fade_out,100);
	m_music_fade_out->InsertString(0,"0");
	m_music_fade_out->InsertString(1,"300000");
	m_music_fade_out->InsertString(2,"600000");
	m_music_fade_out->SetCurSel(1);


	GetDlgItem(IDC_MUSIC_PATH)->EnableWindow(FALSE);
	OnBnClickedRadioMOON();
	((CButton *)GetDlgItem(IDC_RADIO_YU))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_MINGYUE))->SetCheck(TRUE);

	total_time = pDlg->getTotalTime();
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_MUSIC_TIMELINE_END)->SetWindowText(buf);

	DWORD dwStyle = listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	listCtrl.SetExtendedStyle(dwStyle); //设置扩展风格

	listCtrl.InsertColumn( 0, "PATH", LVCFMT_LEFT, 100 );
	listCtrl.InsertColumn( 1, "开始时间", LVCFMT_LEFT, 100 );
	listCtrl.InsertColumn( 2, "结束时间", LVCFMT_LEFT, 100 );
	listCtrl.InsertColumn( 3, "id", LVCFMT_LEFT, 40 );//插入列
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAddMusicDialg::OnBnClickedChooseMusicFinish()
{
	// TODO: 在此添加控件通知处理程序代码
	int64_t timeline_start = 0;
	int64_t timeline_end = 0;
	int64_t time_start = 0;
	int64_t time_end = 0;
	float factor = 0.0f;
	int64_t time_fade_in = 0;
	int64_t time_fade_out = 0;
	char szPath[MAX_PATH];
	CString str = NULL;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();

	GetDlgItem(IDC_MUSIC_TIMELINE_START)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &timeline_start);

	GetDlgItem(IDC_MUSIC_TIMELINE_END)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &timeline_end);

	GetDlgItem(IDC_MUSIC_CUT_TIME_START)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &time_start);

	GetDlgItem(IDC_MUSIC_CUT_TIME_END)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &time_end);

	m_music_factor->GetWindowText(str);
	factor = atof(str);

	m_music_fade_in->GetWindowText(str);
	time_fade_in = _ttoi(str);

	m_music_fade_out->GetWindowText(str);
	time_fade_out = _ttoi(str);

	GetDlgItem(IDC_MUSIC_PATH)->GetWindowText(str);
	strcpy(szPath,str);
	
	if(pDlg->addMusic(szPath,timeline_start,timeline_end,time_start,time_end,time_fade_in,time_fade_out,factor,music_id)<=0)
	{
		AfxMessageBox("add music error ");
		return;
	}

	//添加到listCtrl中
	int nCount = listCtrl.GetItemCount();
	int nRow = listCtrl.InsertItem(nCount, szPath);//插入行

	GetDlgItem(IDC_MUSIC_TIMELINE_START)->GetWindowText(str);
	listCtrl.SetItemText(nRow, 1, str);//设置数据开始时间

	GetDlgItem(IDC_MUSIC_TIMELINE_END)->GetWindowText(str);
	listCtrl.SetItemText(nRow, 2, str);//设置数据结束时间

	str.Format("%d",music_id);
	listCtrl.SetItemText(nRow, 3, str);//设置数据id

	music_id++;

	return ;
}

void CAddMusicDialg::OnBnClickedChooseMusic()
{
	// TODO: 在此添加控件通知处理程序代码
	int64_t timeline_total = 0;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	int ret = 0;
	char buf[256];
	char path[MAX_PATH];
	int64_t total_time = 0;
	//CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("位图文件(*.BMP;*.jpg;*mp4;*.mov)|*.BMP;*.jpg;*mp4;*.mov|"));

	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ALLOWMULTISELECT,_T("音频文件(*.mp3;*.avi;*mp4;*.mov)|*.mp3;*.avi;*mp4;*.mov|"));
	TCHAR *pBuffer = new TCHAR[MAX_PATH    * 20];//最多允许同时打开20个文件
	dlg.m_ofn.lpstrFile = pBuffer;
	dlg.m_ofn.nMaxFile = MAX_PATH * 20;
	dlg.m_ofn.lpstrFile[0] = '\0';
	Audio_Stream_Info audio_info;
	Video_Stream_Info video_info;


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
			if(xpkGetMediaInfo(path,&total_time,&audio_info,&video_info)<=0)
			{
				AfxMessageBox("xpkGetMediaInfo error ");
				return ;
			}
			
			GetDlgItem(IDC_MUSIC_PATH)->SetWindowText(path);
			//截取时间
			GetDlgItem(IDC_MUSIC_CUT_TIME_START)->SetWindowText("0");
			sprintf(buf,"%lld",total_time);
			GetDlgItem(IDC_MUSIC_CUT_TIME_END)->SetWindowText(buf);
			//时间轴上的播放时间
			GetDlgItem(IDC_MUSIC_TIMELINE_START)->SetWindowText("0");
			timeline_total = pDlg->getTotalTime();
			sprintf(buf,"%lld",timeline_total);
			GetDlgItem(IDC_MUSIC_TIMELINE_END)->SetWindowText(buf);
		}
	}
	((CButton *)GetDlgItem(IDC_RADIO_YU))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_MINGYUE))->SetCheck(FALSE);
}
char* CAddMusicDialg::GetResourcePath(char* name)
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
void CAddMusicDialg::OnBnClickedRadioMOON()
{
	// TODO: 在此添加控件通知处理程序代码
	char buf[256];
	int64_t total_time = 0;
	int64_t timeline_total = 0;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	Audio_Stream_Info audio_info;
	Video_Stream_Info video_info;
	char path[MAX_PATH] = {0};

	strcpy(path,GetResourcePath(XPK_MUSIC_MOON));
	if(xpkGetMediaInfo(path,&total_time,&audio_info,&video_info)<=0)
	{
		AfxMessageBox("xpkGetMediaInfo error ");
		return ;
	}

	GetDlgItem(IDC_MUSIC_PATH)->SetWindowText(path);
	//截取时间
	GetDlgItem(IDC_MUSIC_CUT_TIME_START)->SetWindowText("0");
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_MUSIC_CUT_TIME_END)->SetWindowText(buf);
	//时间轴上的播放时间
	GetDlgItem(IDC_MUSIC_TIMELINE_START)->SetWindowText("0");
	timeline_total = pDlg->getTotalTime();
	sprintf(buf,"%lld",timeline_total);
	GetDlgItem(IDC_MUSIC_TIMELINE_END)->SetWindowText(buf);

}

void CAddMusicDialg::OnBnClickedRadioFISH()
{
	// TODO: 在此添加控件通知处理程序代码

	int64_t timeline_total = 0;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	char buf[256];
	int64_t total_time = 0;
	char path[MAX_PATH] = {0};
	HANDLE hAudio = NULL;
	Audio_Stream_Info audio_info;
	Video_Stream_Info video_info;

	strcpy(path,GetResourcePath(XPK_MUSIC_FISH));
	if(xpkGetMediaInfo(path,&total_time,&audio_info,&video_info)<=0)
	{
		AfxMessageBox("xpkGetMediaInfo error ");
		return ;
	}
	GetDlgItem(IDC_MUSIC_PATH)->SetWindowText(path);
	//截取时间
	GetDlgItem(IDC_MUSIC_CUT_TIME_START)->SetWindowText("0");
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_MUSIC_CUT_TIME_END)->SetWindowText(buf);
	//时间轴上的播放时间
	GetDlgItem(IDC_MUSIC_TIMELINE_START)->SetWindowText("0");
	timeline_total = pDlg->getTotalTime();
	sprintf(buf,"%lld",timeline_total);
	GetDlgItem(IDC_MUSIC_TIMELINE_END)->SetWindowText(buf);

}

void CAddMusicDialg::OnBnClickedClearAllMusic()
{
	// TODO: 在此添加控件通知处理程序代码

	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	
	if (pDlg->clearAllMusic()<=0)
		AfxMessageBox("clearAllMusic error ");

	//清空listCtrl
	int nCount = listCtrl.GetItemCount();
	while(nCount)
	{
		listCtrl.DeleteItem(0);
		nCount--;
	}

}

void CAddMusicDialg::OnBnClickedDeleteMusic()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str = NULL;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	// TODO: 在此添加控件通知处理程序代码
	while(listCtrl.GetNextItem(-1,LVNI_ALL | LVNI_SELECTED) != -1)
	{
		int nItem = listCtrl.GetNextItem(-1,LVNI_ALL | LVNI_SELECTED);
		str = listCtrl.GetItemText(nItem,3);
		pDlg->deleteMusic(_ttoi(str));
		listCtrl.DeleteItem(nItem);
	}

	str = listCtrl.GetItemText(0,3);
	pMusic = pDlg->getMusicParams(_ttoi(str));
	if(pMusic)
		refreshUI();
}

int CAddMusicDialg::refreshUI()
{
	CString str = NULL;
	CString path = NULL;
	int pos = 0;

	if (!pMusic)
		return 1;

	path = pMusic->path;
	GetDlgItem(IDC_MUSIC_PATH)->SetWindowText(path);

	pos = path.Find("//");
	path = path.Right(path.GetLength() - pos);

	if (path == XPK_MUSIC_FISH)
	{
		((CButton*)GetDlgItem(IDC_RADIO_YU))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_MINGYUE))->SetCheck(0);
	}
	else if (path == XPK_MUSIC_MOON)
	{
		((CButton*)GetDlgItem(IDC_RADIO_YU))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_MINGYUE))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_YU))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_MINGYUE))->SetCheck(0);
	}

	str.Format("%lld",pMusic->timeline_start);
	GetDlgItem(IDC_MUSIC_TIMELINE_START)->SetWindowText(str);

	str.Format("%lld",pMusic->timeline_end);
	GetDlgItem(IDC_MUSIC_TIMELINE_END)->SetWindowText(str);

	str.Format("%lld",pMusic->start_time);
	GetDlgItem(IDC_MUSIC_CUT_TIME_START)->SetWindowText(str);

	str.Format("%lld",pMusic->end_time);
	GetDlgItem(IDC_MUSIC_CUT_TIME_END)->SetWindowText(str);

	m_music_factor->SetCurSel(pMusic->factor*10);
	m_music_fade_in->SetCurSel(pMusic->fade_in/300000);
	m_music_fade_out->SetCurSel(pMusic->fade_out/300000);

	return 1;
}
void CAddMusicDialg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	*pResult = 0;

	//根据选中的索引获取相关参数
	while(listCtrl.GetNextItem(-1,LVNI_ALL | LVNI_SELECTED) != -1)
	{
		int nItem = listCtrl.GetNextItem(-1,LVNI_ALL | LVNI_SELECTED);
		CString str = listCtrl.GetItemText(nItem,3);
		pMusic = pDlg->getMusicParams(_ttoi(str));
		if (!pMusic)
			AfxMessageBox("获取apng参数失败！");
		else
		{
			refreshUI();
			break;
		}
	}
}
