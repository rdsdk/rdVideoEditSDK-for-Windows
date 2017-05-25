// AdvancedSettingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "AdvancedSettingPictureDialog.h"
#include "XPKAddMedia.h"


// CAdvancedSettingPictureDialog 对话框

IMPLEMENT_DYNAMIC(CAdvancedSettingPictureDialog, CDialog)

CAdvancedSettingPictureDialog::CAdvancedSettingPictureDialog(int id ,int imageWidth,int imageHeight,enum MEDIA_TYPES type,int64_t totalTime,CWnd* pParent /*=NULL*/)
	: CDialog(CAdvancedSettingPictureDialog::IDD, pParent)
{
	m_resouceId = id;
	m_media_width = imageWidth;
	m_media_height = imageHeight;
	m_media_type = type;
	angle = 0;
	propertyChange = 0;
	m_media_total_time = totalTime;
	memset(&rtClip,0,sizeof(RECTF));
	m_pParent = (CXPKAddMedia *)pParent;

	
}

CAdvancedSettingPictureDialog::~CAdvancedSettingPictureDialog()
{
	delete m_pRorateBox;
	delete m_pFlipBox;
	delete m_pClipBox;
	delete m_pSpeedBox;
}

void CAdvancedSettingPictureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAdvancedSettingPictureDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CAdvancedSettingPictureDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CAdvancedSettingPictureDialog 消息处理程序
int CAdvancedSettingPictureDialog::refreshUI()
{
	CenterWindow();
	ShowWindow(SW_SHOW);

	return 1;
}
int CAdvancedSettingPictureDialog::picturePropertyChange( int change)
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
		GetDlgItem(IDC_EDIT_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_RIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_BOTTOM)->EnableWindow(FALSE);
		//没有裁剪
		rtClip.left = 0;
		rtClip.top = 0;

		rtClip.right = w;
		rtClip.bottom = h;
	}
	else if (1 == clip_type)
	{
		GetDlgItem(IDC_EDIT_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_RIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_BOTTOM)->EnableWindow(FALSE);
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
		GetDlgItem(IDC_EDIT_LEFT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_RIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_BOTTOM)->EnableWindow(TRUE);
	}
	sprintf(buf,"%d",(int)rtClip.left);
	GetDlgItem(IDC_EDIT_LEFT)->SetWindowText(buf);
	sprintf(buf,"%d",(int)rtClip.top);
	GetDlgItem(IDC_EDIT_TOP)->SetWindowText(buf);
	sprintf(buf,"%d",(int)rtClip.right);
	GetDlgItem(IDC_EDIT_RIGHT)->SetWindowText(buf);
	sprintf(buf,"%d",(int)rtClip.bottom);
	GetDlgItem(IDC_EDIT_BOTTOM)->SetWindowText(buf);

	propertyChange = change;
	return 1;
}

void CAdvancedSettingPictureDialog::set_DropDownHight(CXPKCombobox*box,UINT LinesToDisplay)
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
void CAdvancedSettingPictureDialog::set_DropDownWidth(CXPKCombobox*box,UINT nWidth )
{
	//	ASSERT(IsWindow(box));
	box->SetDroppedWidth(nWidth);
}


BOOL CAdvancedSettingPictureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char buf[MAX_PATH];
	int  interval = 50;
	RECT rect = {0};
	int top = 30;
	int left = 0;
	int right = 0;
	int bottom = 0;

	GetClientRect(&rect);
	left = rect.left + 160;
	right = rect.left + 360;
	top = rect.top;
	bottom = rect.bottom;

	//旋转组合框
	m_pRorateBox = new CXPKCombobox(COMBOBOX_PICTURE_PROPERTY);	
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

	//翻转组合框
	m_pFlipBox = new CXPKCombobox(COMBOBOX_PICTURE_PROPERTY);
	rect.left = left;
	rect.right = right;
	rect.top = rect.top + 10 + interval;
	rect.bottom = rect.top + 20 + interval;
	m_pFlipBox->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	m_pFlipBox->InsertString(0,"0");
	m_pFlipBox->InsertString(1,"1.horizontal");
	m_pFlipBox->InsertString(2,"2.vertical");
	m_pFlipBox->SetCurSel(0);

	//裁剪组合框
	m_pClipBox = new CXPKCombobox(COMBOBOX_PICTURE_PROPERTY);
	rect.left = left;
	rect.right = right;
	rect.top = rect.top + 10 + interval;
	rect.bottom = rect.top + 20 + interval;
	m_pClipBox->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	m_pClipBox->InsertString(0,"0");
	m_pClipBox->InsertString(1,"1.1:1");
	m_pClipBox->InsertString(2,"2.自由裁剪");
	m_pClipBox->SetCurSel(0);

	GetDlgItem(IDC_EDIT_TOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LEFT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RIGHT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_BOTTOM)->EnableWindow(FALSE);

	//默认裁剪区域
	sprintf(buf,"%d",0);
	GetDlgItem(IDC_EDIT_LEFT)->SetWindowText(buf);
	sprintf(buf,"%d",0);
	GetDlgItem(IDC_EDIT_TOP)->SetWindowText(buf);
	sprintf(buf,"%d",m_media_width);
	GetDlgItem(IDC_EDIT_RIGHT)->SetWindowText(buf);
	sprintf(buf,"%d",m_media_height);
	GetDlgItem(IDC_EDIT_BOTTOM)->SetWindowText(buf);

	sprintf(buf,"(%d,%d,%d,%d)",0,0,m_media_width,m_media_height);
	GetDlgItem(IDC_STATIC_CLIP)->SetWindowText(buf);


	//调试组合框
	m_pSpeedBox = new CXPKCombobox(COMBOBOX_PICTURE_PROPERTY);
	rect.left = left;
	rect.right = right;
	rect.top = 275;
	rect.bottom = rect.top + 20 + interval;
	m_pSpeedBox->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	m_pSpeedBox->InsertString(0,"0.5");
	m_pSpeedBox->InsertString(1,"1.0");
	m_pSpeedBox->InsertString(2,"2.0");
	m_pSpeedBox->InsertString(3,"4.0");
	m_pSpeedBox->SetCurSel(1);

	ShowWindow(SW_HIDE); //显示非模态对话框 

	rtClip.right = m_media_width;
	rtClip.bottom = m_media_height;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
RECTF CAdvancedSettingPictureDialog::getMediaClip()
{
	return rtClip;
}
int	CAdvancedSettingPictureDialog::getMediaRotateAngle()
{
	CString str = NULL;
	m_pRorateBox->GetWindowText(str);
	return _ttoi(str);
}
int	CAdvancedSettingPictureDialog::getMediaFileType()
{
	CString str = NULL;
	m_pFlipBox->GetWindowText(str);
	return _ttoi(str);
}
double CAdvancedSettingPictureDialog::getMediaSpeed()
{
	CString str = NULL;
	m_pSpeedBox->GetWindowText(str);
	return atof(str);
}

void CAdvancedSettingPictureDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//检查设置的裁剪区域
	CString str = NULL;
	int w = 0;
	int h = 0;
	
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
		GetDlgItem(IDC_EDIT_LEFT)->GetWindowText(str);
		rtClip.left = _ttoi(str);
		GetDlgItem(IDC_EDIT_TOP)->GetWindowText(str);
		rtClip.top = _ttoi(str);
		GetDlgItem(IDC_EDIT_RIGHT)->GetWindowText(str);
		rtClip.right = _ttoi(str);
		GetDlgItem(IDC_EDIT_BOTTOM)->GetWindowText(str);
		rtClip.bottom = _ttoi(str);
	}

	if (rtClip.left <0 ||rtClip.left >w || rtClip.right<0 ||rtClip.right > w ||
		rtClip.top <0 ||rtClip.top >h || rtClip.bottom<0 ||rtClip.bottom > h )
	{
		AfxMessageBox("set clip rect error！");
		return ;
	}

	if (m_media_type >= MEDIA_SLIDE)
	{

	}
	m_pParent->display(NULL);
	OnOK();
}
