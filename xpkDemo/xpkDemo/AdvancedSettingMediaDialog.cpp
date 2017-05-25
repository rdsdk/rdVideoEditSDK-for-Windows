// CAdvancedSettingMediaDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "AdvancedSettingMediaDialog.h"


// CAdvancedSettingMediaDialog 对话框

IMPLEMENT_DYNAMIC(CAdvancedSettingMediaDialog, CDialog)

CAdvancedSettingMediaDialog::CAdvancedSettingMediaDialog(int id ,int imageWidth,int imageHeight,enum MEDIA_TYPES type,int64_t totalTime,CWnd* pParent /*=NULL*/)
	: CDialog(CAdvancedSettingMediaDialog::IDD, pParent)
{
	m_resouceId = id;
	m_pRorateBox = NULL;
	m_pFlipBox = NULL;
	m_pClipBox = NULL;
	m_pSpeedBox = NULL;
	angle = 0;
	m_init = 0;
	pClipList = NULL;
	propertyChange = 0;
	m_media_height = imageHeight;
	m_media_width = imageWidth;
	m_media_total_time = totalTime;
	memset(&rtClip,0,sizeof(RECTF));
	m_pParent = (CXPKAddMedia *)pParent;
}

CAdvancedSettingMediaDialog::~CAdvancedSettingMediaDialog()
{
	XPK_DYNAMAIC_CLIP_LIST* list= NULL;
	delete m_pClipBox;
	delete m_pFlipBox;
	delete m_pRorateBox;
	delete m_pSpeedBox;
	
	while(pClipList)
	{
		list = pClipList->next;
		free(pClipList);
		pClipList = list;
	}
}

void CAdvancedSettingMediaDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_useDynamicClip);
}


BEGIN_MESSAGE_MAP(CAdvancedSettingMediaDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CAdvancedSettingMediaDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK1, &CAdvancedSettingMediaDialog::OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_MEDIA_CLIP_END_TIME_1, &CAdvancedSettingMediaDialog::OnEnChangeMediaClipEndTime1)
	ON_EN_CHANGE(IDC_MEDIA_CLIP_END_TIME_2, &CAdvancedSettingMediaDialog::OnEnChangeMediaClipEndTime2)
END_MESSAGE_MAP()


// CAdvancedSettingMediaDialog 消息处理程序
int CAdvancedSettingMediaDialog::refreshUI()
{
	CenterWindow();
	ShowWindow(SW_SHOW);

	return 1;
}
double CAdvancedSettingMediaDialog::getMediaSpeed()
{
	CString str = NULL;
	m_pSpeedBox->GetWindowText(str);
	return (atof(str));
}
int64_t CAdvancedSettingMediaDialog::getMediaCutStartTime()
{

	CString str = NULL;
	GetDlgItem(IDC_EDIT_MEDIA_CUT_START)->GetWindowText(str);
	return (_ttoi(str));
}
int	CAdvancedSettingMediaDialog::getMediaRotateAngle()
{
	CString str = NULL;
	m_pRorateBox->GetWindowText(str);
	return _ttoi(str);
}
int	CAdvancedSettingMediaDialog::getMediaFileType()
{
	CString str = NULL;
	m_pFlipBox->GetWindowText(str);
	return _ttoi(str);
}
int64_t CAdvancedSettingMediaDialog::getMediaCutEndTime()
{
	CString str = NULL;
	int64_t nTime = 0;
	GetDlgItem(IDC_EDIT_MEDIA_CUT_END)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &nTime);
	return nTime;
}
int64_t CAdvancedSettingMediaDialog::getMediaInalienableTime()
{
	CString str = NULL;
	GetDlgItem(IDC_EDIT_MEDIA_INALIENABLE_TIME)->GetWindowText(str);
	return (_ttoi(str));
}	
int CAdvancedSettingMediaDialog::picturePropertyChange( int change)
{
	char buf[MAX_PATH];

	int clip_type = 0;
	int w = 0;
	int h = 0;
	CString str = NULL;

	m_pRorateBox->GetWindowText(str);
	angle = _ttoi(str);
	if(90== angle || 270 == angle)
	{
		w = m_media_height;
		h = m_media_width;
	}
	else
	{
		w = m_media_width;
		h = m_media_height;
	}

	m_pClipBox->GetWindowText(str);
	clip_type = _ttoi(str);
	if (0 == clip_type)
	{
		GetDlgItem(IDC_EDIT_MEDIA_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MEDIA_RIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MEDIA_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MEDIA_BOTTOM)->EnableWindow(FALSE);
		//没有裁剪
		rtClip.left = 0;
		rtClip.top = 0;

		rtClip.right = w;
		rtClip.bottom = h;
	}
	else if (1 == clip_type)
	{
		GetDlgItem(IDC_EDIT_MEDIA_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MEDIA_RIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MEDIA_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MEDIA_BOTTOM)->EnableWindow(FALSE);
		//1:1裁剪
		if(w > h)
		{
			//左右裁剪
			rtClip.top = 0;
			rtClip.bottom = h;
			rtClip.left = (w - h)/2;
			rtClip.right = w - rtClip.left;
		}
		else
		{
			//上下裁剪
			rtClip.top = (h - w)/2;
			rtClip.bottom = h - rtClip.top;
			rtClip.left = 0;
			rtClip.right = w;
		}
	}
	else
	{
		//自由裁剪
		GetDlgItem(IDC_EDIT_MEDIA_LEFT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MEDIA_RIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MEDIA_TOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MEDIA_BOTTOM)->EnableWindow(TRUE);
	}
	sprintf(buf,"%d",(int)rtClip.left);
	GetDlgItem(IDC_EDIT_MEDIA_LEFT)->SetWindowText(buf);
	sprintf(buf,"%d",(int)rtClip.top);
	GetDlgItem(IDC_EDIT_MEDIA_TOP)->SetWindowText(buf);
	sprintf(buf,"%d",(int)rtClip.right);
	GetDlgItem(IDC_EDIT_MEDIA_RIGHT)->SetWindowText(buf);
	sprintf(buf,"%d",(int)rtClip.bottom);
	GetDlgItem(IDC_EDIT_MEDIA_BOTTOM)->SetWindowText(buf);

	propertyChange = change;
	UpdateClipList();
	return 1;
}

BOOL CAdvancedSettingMediaDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char buf[MAX_PATH];
	RECT rect = {0};
	int  interval = 28;
	int top = 30;
	int left = 0;
	int right = 0;
	int bottom = 0;
	GetClientRect(&rect);

	left = rect.left + 160;
	right = rect.left + 360;
	top = rect.top;
	bottom = rect.bottom;

	//旋转
	m_pRorateBox = new CXPKCombobox(COMBOBOX_MEDIA_PROPERTY);	
	rect.left = left;
	rect.right = right;
	rect.top = top + 40;
	rect.bottom = top + 20;
	m_pRorateBox->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	m_pRorateBox->InsertString(0,"0");
	m_pRorateBox->InsertString(1,"90");
	m_pRorateBox->InsertString(2,"180");
	m_pRorateBox->InsertString(3,"270");
	m_pRorateBox->SetCurSel(0);

	//翻转
	m_pFlipBox = new CXPKCombobox(COMBOBOX_MEDIA_PROPERTY);
	rect.left = left;
	rect.right = right;
	rect.top = rect.top + 10 + interval;
	rect.bottom = rect.top + 20 + interval;
	m_pFlipBox->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	m_pFlipBox->InsertString(0,"0");
	m_pFlipBox->InsertString(1,"1.horizontal");
	m_pFlipBox->InsertString(2,"2.vertical");
	m_pFlipBox->SetCurSel(0);


	//裁剪
	m_pClipBox = new CXPKCombobox(COMBOBOX_MEDIA_PROPERTY);
	rect.left = left;
	rect.right = right;
	rect.top = rect.top + 10 + interval;
	rect.bottom = rect.top + 20 + interval;
	m_pClipBox->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	m_pClipBox->InsertString(0,"0");
	m_pClipBox->InsertString(1,"1.1:1");
	m_pClipBox->InsertString(2,"2.自由裁剪");
	m_pClipBox->SetCurSel(0);

	
	sprintf(buf,"%d",0);
	GetDlgItem(IDC_EDIT_MEDIA_LEFT)->SetWindowText(buf);
	sprintf(buf,"%d",0);
	GetDlgItem(IDC_EDIT_MEDIA_TOP)->SetWindowText(buf);
	sprintf(buf,"%d",m_media_width);
	GetDlgItem(IDC_EDIT_MEDIA_RIGHT)->SetWindowText(buf);
	sprintf(buf,"%d",m_media_height);
	GetDlgItem(IDC_EDIT_MEDIA_BOTTOM)->SetWindowText(buf);

	GetDlgItem(IDC_EDIT_MEDIA_LEFT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MEDIA_RIGHT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MEDIA_TOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MEDIA_BOTTOM)->EnableWindow(FALSE);


	//调速
	rect.left = 130;
	rect.right = 230;
	rect.top = 430;
	rect.bottom = 450;
	m_pSpeedBox = new CXPKCombobox(COMBOBOX_MEDIA_PROPERTY);
	m_pSpeedBox->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	m_pSpeedBox->InsertString(0,"0.5");
	m_pSpeedBox->InsertString(1,"1.0");
	m_pSpeedBox->InsertString(2,"2.0");
	m_pSpeedBox->InsertString(3,"4.0");
	m_pSpeedBox->SetCurSel(1);

	//截取
	GetDlgItem(IDC_EDIT_MEDIA_CUT_START)->SetWindowText("0");
	sprintf(buf,"%lld",m_media_total_time);
	GetDlgItem(IDC_EDIT_MEDIA_CUT_END)->SetWindowText(buf);
	m_cut_start_time = 0;
	m_cut_end_time = m_media_total_time;

	//分割
	GetDlgItem(IDC_EDIT_MEDIA_INALIENABLE_TIME)->SetWindowText("0");
	GetDlgItem(IDC_EDIT_MEDIA_INALIENABLE_TIME)->EnableWindow(FALSE);

	rtClip.right = m_media_width;
	rtClip.bottom = m_media_height;

	UpdateClipList();

	//获取总时间
	sprintf(buf,"< %lld",m_media_total_time);
	GetDlgItem(IDC_MEDIA_CUT_END_TIME_STATIC)->SetWindowText(buf);
	
	m_init = 1;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


int CAdvancedSettingMediaDialog::getDynamicClipRect(CString str,RECTF* rect)
{
	CString str_temp = NULL;
	int x = 0;
	int y = 0;
	char buf[1024];

	x = str.Find(":");
	strncpy(buf,str,x);
	rect->left = atoi(buf);
	y = str.GetLength()-x-1;
	str_temp = str.Right(y);
	str = str_temp;

	x = str.Find(":");
	strncpy(buf,str,x+1);
	rect->top = atoi(buf);
	y = str.GetLength()-x-1;
	str_temp = str.Right(y);
	str = str_temp;

	x = str.Find(":");
	strncpy(buf,str,x+1);
	rect->right = atoi(buf);
	y = str.GetLength()-x-1;
	str_temp = str.Right(y);
	str = str_temp;
	rect->bottom = atoi(str);
	return 1;
}
int CAdvancedSettingMediaDialog::addClipList(RECTF rect,int64_t time_start,int64_t time_end)
{
	if (!pClipList)
	{
		pClipList = (XPK_DYNAMAIC_CLIP_LIST*)malloc(sizeof(XPK_DYNAMAIC_CLIP_LIST));
		if (!pClipList)
			return -__LINE__;
		memset(pClipList,0,sizeof(XPK_DYNAMAIC_CLIP_LIST));
		pClipList->rect = rect;
		pClipList->time_end = time_end;
		pClipList->time_start = time_start;
	}
	else
	{
		XPK_DYNAMAIC_CLIP_LIST* list = pClipList;
		while(list && list->next)
			list = list->next;

		list->next = (XPK_DYNAMAIC_CLIP_LIST*)malloc(sizeof(XPK_DYNAMAIC_CLIP_LIST));
		if (!list->next)
			return -__LINE__;
		memset(list->next,0,sizeof(XPK_DYNAMAIC_CLIP_LIST));
		list->next->rect = rect;
		list->next->time_end = time_end;
		list->next->time_start = time_start;
	}
	return 1;
}
RECTF CAdvancedSettingMediaDialog::getMediaClip()
{
	return rtClip;
}

void CAdvancedSettingMediaDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//检查设置的裁剪区域
	CString str = NULL;
	int w = 0;
	int h = 0;
	int64_t curTime = 0;
	if(90== angle || 270 == angle)
	{
		w = m_media_height;
		h = m_media_width;
	}
	else
	{
		w = m_media_width;
		h = m_media_height;
	}
	if (2 == m_pClipBox->GetCurSel())
	{
		//如果是自由裁剪，获取自由裁剪的裁剪区域
		GetDlgItem(IDC_EDIT_MEDIA_LEFT)->GetWindowText(str);
		rtClip.left = _ttoi(str);
		GetDlgItem(IDC_EDIT_MEDIA_TOP)->GetWindowText(str);
		rtClip.top = _ttoi(str);
		GetDlgItem(IDC_EDIT_MEDIA_RIGHT)->GetWindowText(str);
		rtClip.right = _ttoi(str);
		GetDlgItem(IDC_EDIT_MEDIA_BOTTOM)->GetWindowText(str);
		rtClip.bottom = _ttoi(str);
	}

	if (rtClip.left <0 ||rtClip.left >w || rtClip.right<0 ||rtClip.right > w ||
		rtClip.top <0 ||rtClip.top >h || rtClip.bottom<0 ||rtClip.bottom > h )
	{
		AfxMessageBox("set clip rect error！");
		return ;
	}

	//检查是否使用动态裁剪
	if(m_useDynamicClip.GetCheck())
	{
		
		int64_t start_time_1 = 0;
		int64_t start_time_2 = 0;
		int64_t start_time_3 = 0;
		int64_t end_time_1 = 0;
		int64_t end_time_2 = 0;
		int64_t end_time_3 = 0;
		RECTF rect = {0};
		CString str = NULL;
		XPK_DYNAMAIC_CLIP_LIST* list= NULL;
		while(pClipList)
		{
			list = pClipList->next;
			free(pClipList);
			pClipList = list;
		}
		

		GetDlgItem(IDC_MEDIA_CLIP_START_TIME_1)->GetWindowText(str);
		_stscanf(str, _T("%ld"), &start_time_1);

		GetDlgItem(IDC_MEDIA_CLIP_START_TIME_2)->GetWindowText(str);
		_stscanf(str, _T("%ld"), &start_time_2);

		GetDlgItem(IDC_MEDIA_CLIP_START_TIME_3)->GetWindowText(str);
		_stscanf(str, _T("%ld"), &start_time_3);

		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_1)->GetWindowText(str);
		_stscanf(str, _T("%ld"), &end_time_1);

		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_2)->GetWindowText(str);
		_stscanf(str, _T("%ld"), &end_time_2);

		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_3)->GetWindowText(str);
		_stscanf(str, _T("%ld"), &end_time_3);

		//获取第一次裁剪区域
		GetDlgItem(IDC_MEDIA_CLIP_1)->GetWindowText(str);
		getDynamicClipRect(str,&rect);

		if (addClipList(rect,start_time_1,end_time_1) <= 0)
		{
			AfxMessageBox("addClipList error ");
			return;
		}
		
		//获取第二次裁剪区域
		GetDlgItem(IDC_MEDIA_CLIP_2)->GetWindowText(str);
		getDynamicClipRect(str,&rect);

		if (addClipList(rect,start_time_2,end_time_2) <= 0)
		{
			AfxMessageBox("addClipList error ");
			return;
		}


		//获取第三次裁剪区域
		GetDlgItem(IDC_MEDIA_CLIP_3)->GetWindowText(str);
		getDynamicClipRect(str,&rect);

		if (addClipList(rect,start_time_3,end_time_3) <= 0)
		{
			AfxMessageBox("addClipList error ");
			return;
		}
	}
	
	GetDlgItem(IDC_EDIT_MEDIA_CUT_START)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &curTime);
	if (curTime != m_cut_start_time)
	{
		propertyChange = 1;	
		m_cut_start_time = curTime;
	}

	GetDlgItem(IDC_EDIT_MEDIA_CUT_END)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &curTime);

	if (curTime != m_cut_end_time)
	{
		propertyChange = 1;	
		m_cut_end_time = curTime;
	}


	m_pParent->display(NULL);
	OnOK();
}
XPK_DYNAMAIC_CLIP_LIST* CAdvancedSettingMediaDialog::getDynamicClipList()
{
	if(!pClipList)
		return NULL;
	else 
		return pClipList;
}
int CAdvancedSettingMediaDialog::UpdateClipList()
{
	CString str = NULL;
	int angle = 0;
	int w = 0;
	int h = 0;
	char buf[1024];
	m_pRorateBox->GetWindowText(str);
	angle = _ttoi(str);
	if (angle ==  90 || angle == 270)
	{
		w = m_media_height;
		h = m_media_width;
	}
	else
	{
		w = m_media_width;
		h = m_media_height;
	}
	sprintf(buf,"%d:%d:%d:%d",0,0,w,h);
	GetDlgItem(IDC_MEDIA_CLIP_1)->SetWindowText(buf);
	sprintf(buf,"%d:%d:%d:%d",(w-w/2)/2,(h-h/2)/2,w-(w-w/2)/2,h-(h-h/2)/2);
	GetDlgItem(IDC_MEDIA_CLIP_2)->SetWindowText(buf);
	sprintf(buf,"%d:%d:%d:%d",0,0,w,h);
	GetDlgItem(IDC_MEDIA_CLIP_3)->SetWindowText(buf);


	if(!m_init)
	{
		GetDlgItem(IDC_MEDIA_CLIP_START_TIME_1)->SetWindowText("0");
		GetDlgItem(IDC_MEDIA_CLIP_START_TIME_2)->SetWindowText("0");
		GetDlgItem(IDC_MEDIA_CLIP_START_TIME_3)->SetWindowText("0");

		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_1)->SetWindowText("0");
		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_2)->SetWindowText("0");
		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_3)->SetWindowText("0");
	}
	

	GetDlgItem(IDC_MEDIA_CLIP_1)->EnableWindow(FALSE);
	GetDlgItem(IDC_MEDIA_CLIP_START_TIME_1)->EnableWindow(FALSE);
	GetDlgItem(IDC_MEDIA_CLIP_START_TIME_2)->EnableWindow(FALSE);
	GetDlgItem(IDC_MEDIA_CLIP_START_TIME_3)->EnableWindow(FALSE);
	
	if(m_useDynamicClip.GetCheck())
	{
		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_1)->EnableWindow(TRUE);

		GetDlgItem(IDC_MEDIA_CLIP_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_MEDIA_CLIP_3)->EnableWindow(TRUE);

		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_3)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_1)->EnableWindow(FALSE);

		GetDlgItem(IDC_MEDIA_CLIP_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIA_CLIP_3)->EnableWindow(FALSE);

		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIA_CLIP_END_TIME_3)->EnableWindow(FALSE);
	}
	return 1;
}

void CAdvancedSettingMediaDialog::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateClipList();
	propertyChange = 1;
	
}

void CAdvancedSettingMediaDialog::OnEnChangeMediaClipEndTime1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	//第一次裁剪的结束时间就是第二次裁剪的开始时间
	CString str = NULL;
	GetDlgItem(IDC_MEDIA_CLIP_END_TIME_1)->GetWindowText(str);
	GetDlgItem(IDC_MEDIA_CLIP_START_TIME_2)->SetWindowText(str);

}



void CAdvancedSettingMediaDialog::OnEnChangeMediaClipEndTime2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	//第二次裁剪的结束时间就是第三次裁剪的开始时间
	CString str = NULL;
	GetDlgItem(IDC_MEDIA_CLIP_END_TIME_2)->GetWindowText(str);
	GetDlgItem(IDC_MEDIA_CLIP_START_TIME_3)->SetWindowText(str);
}
