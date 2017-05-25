// AdvancedSettingAudio.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "AdvancedSettingAudioDialog.h"



// CAdvancedSettingAudioDialog 对话框

IMPLEMENT_DYNAMIC(CAdvancedSettingAudioDialog, CDialog)

CAdvancedSettingAudioDialog::CAdvancedSettingAudioDialog(int id,CWnd* pParent /*=NULL*/)
	: CDialog(CAdvancedSettingAudioDialog::IDD, pParent)
{
	m_resouceId = id;
	m_change = 0;
}

CAdvancedSettingAudioDialog::~CAdvancedSettingAudioDialog()
{
	delete m_music_factor;
	delete m_music_fade_in;
	delete m_music_fade_out;
	delete m_music_speed;
}

void CAdvancedSettingAudioDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAdvancedSettingAudioDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CAdvancedSettingAudioDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CAdvancedSettingAudioDialog 消息处理程序

int CAdvancedSettingAudioDialog::refreshUI()
{
	CenterWindow();
	ShowWindow(SW_SHOW);

	return 1;
}

void CAdvancedSettingAudioDialog::set_DropDownHight(CXPKCombobox*box,UINT LinesToDisplay)
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
void CAdvancedSettingAudioDialog::set_DropDownWidth(CXPKCombobox*box,UINT nWidth )
{
	box->SetDroppedWidth(nWidth);
}
BOOL CAdvancedSettingAudioDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	char buf[1024];
	
	// TODO:  在此添加额外的初始化	

	RECT rect;
	GetClientRect(&rect);
	m_music_factor = new CXPKCombobox(COMBOBOX_MUSIC_PROPERTY);
	rect.left = 135;
	rect.right = rect.left + COMBOBOX_MUSIC_PROPERTY_WIDTH;
	rect.top =  80;
	rect.bottom = rect.top + COMBOBOX_MUSIC_PROPERTY_HEIGHT;

	m_music_factor->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	set_DropDownHight(m_music_factor,5);
	set_DropDownWidth(m_music_factor,100);
	
	for (int i = 0;i<=10;i++)
	{
		sprintf(buf,"%0.1f",i/(float)10);
		m_music_factor->InsertString(i,buf);
	}
	m_music_factor->SetCurSel(5);


	m_music_fade_in = new CXPKCombobox(COMBOBOX_MUSIC_PROPERTY);
	rect.left = 135;
	rect.right = rect.left + COMBOBOX_MUSIC_PROPERTY_WIDTH;
	rect.top =  80 + COMBOBOX_MUSIC_PROPERTY_HEIGHT * 2;
	rect.bottom = rect.top + COMBOBOX_MUSIC_PROPERTY_HEIGHT;
	m_music_fade_in->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	set_DropDownHight(m_music_fade_in,5);
	set_DropDownWidth(m_music_fade_in,100);
	m_music_fade_in->InsertString(0,"0");
	m_music_fade_in->InsertString(1,"300000");
	m_music_fade_in->InsertString(2,"600000");
	m_music_fade_in->SetCurSel(1);

	m_music_fade_out = new CXPKCombobox(COMBOBOX_MUSIC_PROPERTY);
	rect.left = 135;
	rect.right = rect.left + COMBOBOX_MUSIC_PROPERTY_WIDTH;
	rect.top =  80 + COMBOBOX_MUSIC_PROPERTY_HEIGHT * 4;
	rect.bottom = rect.top + COMBOBOX_MUSIC_PROPERTY_HEIGHT;
	m_music_fade_out->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	set_DropDownHight(m_music_fade_out,5);
	set_DropDownWidth(m_music_fade_out,100);
	m_music_fade_out->InsertString(0,"0");
	m_music_fade_out->InsertString(1,"300000");
	m_music_fade_out->InsertString(2,"600000");
	m_music_fade_out->SetCurSel(1);

	m_music_speed = new CXPKCombobox(COMBOBOX_MUSIC_PROPERTY);
	rect.left = 135;
	rect.right = rect.left + COMBOBOX_MUSIC_PROPERTY_WIDTH;
	rect.top =  80 + COMBOBOX_MUSIC_PROPERTY_HEIGHT * 6;
	rect.bottom = rect.top + COMBOBOX_MUSIC_PROPERTY_HEIGHT;
	m_music_speed->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST ,rect,this,0);
	set_DropDownHight(m_music_speed,5);
	set_DropDownWidth(m_music_speed,100);
	m_music_speed->InsertString(0,"1");
	m_music_speed->InsertString(1,"2");
	m_music_speed->InsertString(2,"4");
	m_music_speed->SetCurSel(0);
	

	GetDlgItem(IDC_MUSIC_START_TIME_H)->SetWindowText("00");
	GetDlgItem(IDC_MUSIC_START_TIME_M)->SetWindowText("00");
	GetDlgItem(IDC_MUSIC_START_TIME_S)->SetWindowText("00");

	GetDlgItem(IDC_MUSIC_END_TIME_H)->SetWindowText("00");
	GetDlgItem(IDC_MUSIC_END_TIME_M)->SetWindowText("00");
	GetDlgItem(IDC_MUSIC_END_TIME_S)->SetWindowText("00");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
int CAdvancedSettingAudioDialog::musicPropertyChange()
{
	m_change = 1;
	return 1;
}
int64_t CAdvancedSettingAudioDialog::getMusicCutStartTime()
{
	CString str = NULL;
	int h = 0;	//时
	int m = 0;	//分
	int s = 0;	//秒
	GetDlgItem(IDC_MUSIC_START_TIME_H)->GetWindowText(str);
	h = _ttoi(str);
	GetDlgItem(IDC_MUSIC_START_TIME_M)->GetWindowText(str);
	m = _ttoi(str);
	GetDlgItem(IDC_MUSIC_START_TIME_S)->GetWindowText(str);
	s = _ttoi(str);
	return (h*60*60+m*60+s)*1000000;
}
int64_t CAdvancedSettingAudioDialog::getMusicCutEndTime()
{
	CString str = NULL;
	int h = 0;	//时
	int m = 0;	//分
	int s = 0;	//秒
	GetDlgItem(IDC_MUSIC_END_TIME_H)->GetWindowText(str);
	h = _ttoi(str);
	GetDlgItem(IDC_MUSIC_END_TIME_M)->GetWindowText(str);
	m = _ttoi(str);
	GetDlgItem(IDC_MUSIC_END_TIME_S)->GetWindowText(str);
	s = _ttoi(str);
	return (h*60*60+m*60+s)*1000000;
}

void CAdvancedSettingAudioDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}
