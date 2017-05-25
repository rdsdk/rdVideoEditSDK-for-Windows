// AddSubtitle.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "AddSubtitleDialg.h"
#include "XPKWizard.h"


// CAddSubtitleDialg 对话框

IMPLEMENT_DYNAMIC(CAddSubtitleDialg, CDialog)

CAddSubtitleDialg::CAddSubtitleDialg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddSubtitleDialg::IDD, pParent)
{

	m_ColorRed = 255;
	m_ColorGreen = 255;
	m_ColorBlue = 255;
	subtitle_id = 0;
	pSubtitle = NULL;

	m_fontNameNumber = 0;
	m_bold = 0;
	m_italics = 0;
	m_underline = 0;
	memset(szText,0,2048);
	memset(&font,0,sizeof(FONT_PEP));

//	memset(&m_susbtile,0,sizeof(XPK_SUBTITLE_LIST));
}

CAddSubtitleDialg::~CAddSubtitleDialg()
{

}

void CAddSubtitleDialg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_fontSizeCtrl);
	DDX_Control(pDX, IDC_COMBO2, m_fontNameCtrl);
	DDX_Control(pDX, IDC_LIST1, listCtrl);
}


BEGIN_MESSAGE_MAP(CAddSubtitleDialg, CDialog)
	ON_BN_CLICKED(IDC_SUBTITLE_FINISH, &CAddSubtitleDialg::OnBnClickedSubtitleFinish)
	ON_BN_CLICKED(IDC_BOLD, &CAddSubtitleDialg::OnBnClickedBold)
	ON_BN_CLICKED(IDC_ITALICS, &CAddSubtitleDialg::OnBnClickedItalics)
	ON_BN_CLICKED(IDC_UNDERLINE, &CAddSubtitleDialg::OnBnClickedUnderline)

	ON_BN_CLICKED(IDC_FONT_COLOR, &CAddSubtitleDialg::OnBnClickedFontColor)
	ON_BN_CLICKED(IDC_CLEAR_ALL_SUBTITLE, &CAddSubtitleDialg::OnBnClickedClearAllSubtitle)
	ON_BN_CLICKED(IDC_BUTTON1, &CAddSubtitleDialg::OnBnClickedDeleteSubtitle)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CAddSubtitleDialg::OnNMClickList1)
END_MESSAGE_MAP()

int CAddSubtitleDialg::setTotalTime(int64_t totalTime)
{

	char buf[1024];
	sprintf(buf,"%lld",totalTime);
	GetDlgItem(IDC_FONT_TIMELINE_END)->SetWindowText(buf);
	return 1;
}
void CAddSubtitleDialg::set_DropDownHight(CComboBox*box,UINT LinesToDisplay)
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
void CAddSubtitleDialg::set_DropDownWidth(CComboBox*box,UINT nWidth )
{
	box->SetDroppedWidth(nWidth);
}
// CAddSubtitleDialg 消息处理程序

void CAddSubtitleDialg::OnBnClickedSubtitleFinish()
{
	// TODO: 在此添加控件通知处理程序代码
	int64_t timeline_start = 0;
	int64_t timeline_end = 0;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	CString str = NULL;
	char buf[1024];
	int ret = 0;
	char srtPath[1024] = {0};
	char *pSub = "123萨芬的萨芬、\n das 佛挡杀佛、\n sadasd放大\n";
	memset(srtPath,0,1024);

	memset(szText,0,2048);
	//获取字幕文本
	GetDlgItem(IDC_SUBTITLE_TEXT)->GetWindowText(str);
/*	memcpy(szText,pSub,strlen(pSub));*/

	memcpy(szText,str,strlen(str));

	//获取开始时间
	GetDlgItem(IDC_FONT_TIMELINE_START)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &timeline_start);

	//获取结束时间
	GetDlgItem(IDC_FONT_TIMELINE_END)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &timeline_end);


	if (timeline_end == 0)
		timeline_end = pDlg->getTotalTime();

	//加粗/斜体/下划线
	font.blod = m_bold;
	font.underline = m_underline;
	font.bias = m_italics;

	//显示区域
	GetDlgItem(IDC_FONT_LEFT)->GetWindowText(str);
	fontRect.left = _ttoi(str);

	GetDlgItem(IDC_FONT_RIGHT)->GetWindowText(str);
	fontRect.right = _ttoi(str);

	GetDlgItem(IDC_FONT_TOP)->GetWindowText(str);
	fontRect.top = _ttoi(str);

	GetDlgItem(IDC_FONT_BOTTOM)->GetWindowText(str);
	fontRect.bottom = _ttoi(str);

	//字体大小
	m_fontSizeCtrl.GetWindowText(str);
	font.width = _ttoi(str);
	font.height = _ttoi(str);
	
	//字体颜色
	font.fontClr.rgbRed = m_ColorRed;
	font.fontClr.rgbGreen = m_ColorGreen;
	font.fontClr.rgbBlue = m_ColorBlue;

	//字体名字
	m_fontNameCtrl.GetWindowText(str);
	strcpy(font.name,str);

	ret = pDlg->addSubtitle(szText,timeline_start,timeline_end,font,fontRect,srtPath,subtitle_id);
	if (ret<=0)
	{
		sprintf(buf,"addSubtitle error :%d ",ret);
		AfxMessageBox(buf);
		return;
	}
	
	//添加到listCtrl中
	int nCount = listCtrl.GetItemCount();
	GetDlgItem(IDC_SUBTITLE_TEXT)->GetWindowText(str);
	int nRow = listCtrl.InsertItem(nCount, str);//插入行
	
	GetDlgItem(IDC_FONT_TIMELINE_START)->GetWindowText(str);
	listCtrl.SetItemText(nRow, 1, str);//设置数据开始时间

	GetDlgItem(IDC_FONT_TIMELINE_END)->GetWindowText(str);
	listCtrl.SetItemText(nRow, 2, str);//设置数据结束时间

	str.Format("%d",subtitle_id);
	listCtrl.SetItemText(nRow, 3, str);//设置数据id

	subtitle_id++;
}

void CAddSubtitleDialg::OnBnClickedBold()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bold = !m_bold;
}

void CAddSubtitleDialg::OnBnClickedItalics()
{
	// TODO: 在此添加控件通知处理程序代码
	m_italics = !m_italics;
}

void CAddSubtitleDialg::OnBnClickedUnderline()
{
	// TODO: 在此添加控件通知处理程序代码
	m_underline = !m_underline;
}


void CAddSubtitleDialg::OnBnClickedFontColor()
{
	// TODO: 在此添加控件通知处理程序代码
	COLORREF color = RGB(255, 255, 255);
	CColorDialog cdlg(color); // 设置默认颜色
	if(cdlg.DoModal() == IDOK)
		color = cdlg.GetColor();

	m_ColorRed = GetRValue(color);
	m_ColorGreen = GetGValue(color);
	m_ColorBlue = GetBValue(color);

	CString str;
	str.Format("R:%d  G:%d  B:%d",m_ColorRed,m_ColorGreen,m_ColorBlue);
	GetDlgItem(IDC_CURRENT_COLOR)->SetWindowText(str);
}

//枚举函数
int CALLBACK EnumFontFamProc(LPENUMLOGFONT lpelf,LPNEWTEXTMETRIC lpntm,DWORD nFontType,long lparam)
{
	// Create a pointer to the dialog window
	CAddSubtitleDialg* pWnd = (CAddSubtitleDialg*) lparam;
	// add the font name to the list box
	pWnd->m_fontNameCtrl.AddString(lpelf ->elfLogFont.lfFaceName);
	pWnd->m_fontNameNumber++;
	// Return 1 to continue font enumeration
	return 1;
}

BOOL CAddSubtitleDialg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char buf[256];
	int64_t total_time = 0;
	RECT rect;
	// TODO:  在此添加额外的初始化

	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	total_time = pDlg->getTotalTime();
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_FONT_TIMELINE_START)->SetWindowText("0");
	GetDlgItem(IDC_FONT_TIMELINE_END)->SetWindowText(buf);

	pDlg->getShowRect(&rect);
	GetDlgItem(IDC_FONT_LEFT)->SetWindowText("0");
	GetDlgItem(IDC_FONT_TOP)->SetWindowText("0");
	sprintf(buf,"%d",rect.right);
	GetDlgItem(IDC_FONT_RIGHT)->SetWindowText(buf);
	sprintf(buf,"%d",rect.bottom);
	GetDlgItem(IDC_FONT_BOTTOM)->SetWindowText(buf);

	set_DropDownHight(&m_fontSizeCtrl,5);
	set_DropDownWidth(&m_fontSizeCtrl,100);
	
	for (int i = 10; i < 72;i++)
	{
		sprintf(buf,"%d",i);
		m_fontSizeCtrl.InsertString(i-10,buf);
	}
	m_fontSizeCtrl.SetCurSel(50);
		
	set_DropDownHight(&m_fontNameCtrl,5);
	set_DropDownWidth(&m_fontNameCtrl,100);


	LOGFONT lf;
	lf.lfCharSet = DEFAULT_CHARSET; // Initialize the LOGFONT structure
	strcpy(lf.lfFaceName,"");
	CClientDC dc (this);

	//Enumerate the font families
	::EnumFontFamiliesEx((HDC) dc,&lf,
		(FONTENUMPROC) EnumFontFamProc,(LPARAM) this,0);

	if(m_fontNameNumber > 0)
		m_fontNameCtrl.SetCurSel(m_fontNameNumber-1);

	DWORD dwStyle = listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	listCtrl.SetExtendedStyle(dwStyle); //设置扩展风格

	listCtrl.InsertColumn( 0, "TEXT", LVCFMT_LEFT, 100 );
	listCtrl.InsertColumn( 1, "开始时间", LVCFMT_LEFT, 100 );
	listCtrl.InsertColumn( 2, "结束时间", LVCFMT_LEFT, 100 );
	listCtrl.InsertColumn( 3, "id", LVCFMT_LEFT, 40 );//插入列
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CAddSubtitleDialg::OnBnClickedClearAllSubtitle()
{
	// TODO: 在此添加控件通知处理程序代码

	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	if(pDlg->clearAllSubtitle()<=0)
		AfxMessageBox("clearAllSubtitle error ");

	//清空listCtrl
	int nCount = listCtrl.GetItemCount();
	while(nCount)
	{
		listCtrl.DeleteItem(0);
		nCount--;
	}

}

void CAddSubtitleDialg::OnBnClickedDeleteSubtitle()
{
	CString str = NULL;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	// TODO: 在此添加控件通知处理程序代码
	while(listCtrl.GetNextItem(-1,LVNI_ALL | LVNI_SELECTED) != -1)
	{
		int nItem = listCtrl.GetNextItem(-1,LVNI_ALL | LVNI_SELECTED);
		str = listCtrl.GetItemText(nItem,3);
		pDlg->deleteSubtitle(_ttoi(str));
		listCtrl.DeleteItem(nItem);
	}
	str = listCtrl.GetItemText(0,3);
	pSubtitle = pDlg->getSubtitleParams(_ttoi(str));
	if(pSubtitle)
		refreshUI();
}
int CAddSubtitleDialg::refreshUI()
{
	CString str = NULL;

	if(!pSubtitle)
		return 1;

	str.Format("%lld",pSubtitle->timeline_start);
	GetDlgItem(IDC_FONT_TIMELINE_START)->SetWindowText(str);

	str.Format("%lld",pSubtitle->timeline_end);
	GetDlgItem(IDC_FONT_TIMELINE_END)->SetWindowText(str);

	GetDlgItem(IDC_SUBTITLE_TEXT)->SetWindowText(pSubtitle->szText);

	str.Format("R:%d  G:%d  B:%d",pSubtitle->font.fontClr.rgbRed,pSubtitle->font.fontClr.rgbGreen,pSubtitle->font.fontClr.rgbBlue);
	GetDlgItem(IDC_CURRENT_COLOR)->SetWindowText(str);

	str.Format("%d",(int)pSubtitle->rect.left);
	GetDlgItem(IDC_FONT_LEFT)->SetWindowText(str);

	str.Format("%d",(int)pSubtitle->rect.top);
	GetDlgItem(IDC_FONT_TOP)->SetWindowText(str);

	str.Format("%d",(int)pSubtitle->rect.right);
	GetDlgItem(IDC_FONT_RIGHT)->SetWindowText(str);

	str.Format("%d",(int)pSubtitle->rect.bottom);
	GetDlgItem(IDC_FONT_BOTTOM)->SetWindowText(str);

	((CButton*)GetDlgItem(IDC_BOLD))->SetCheck(pSubtitle->font.blod);
	((CButton*)GetDlgItem(IDC_ITALICS))->SetCheck(pSubtitle->font.bias);
	((CButton*)GetDlgItem(IDC_UNDERLINE))->SetCheck(pSubtitle->font.underline);

	str.Format("%d",pSubtitle->font.width);
	m_fontSizeCtrl.SetWindowText(str);
	m_fontNameCtrl.SetWindowText(pSubtitle->font.name);

	return 1;
}
void CAddSubtitleDialg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
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
		pSubtitle = pDlg->getSubtitleParams(_ttoi(str));
		if (!pSubtitle)
			AfxMessageBox("获取apng参数失败！");
		else
		{
			refreshUI();
			break;
		}
	}

}
