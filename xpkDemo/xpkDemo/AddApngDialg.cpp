// AddApng.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "AddApngDialg.h"
#include "XPKPreview.h"


// CAddApngDialg 对话框

IMPLEMENT_DYNAMIC(CAddApngDialg, CDialog)

CAddApngDialg::CAddApngDialg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddApngDialg::IDD, pParent)
{
	memset(szPath,0,MAX_PATH);
	apng_index = 0;
	pApng = NULL;
}

CAddApngDialg::~CAddApngDialg()
{
}

void CAddApngDialg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, listCtrl);
}


BEGIN_MESSAGE_MAP(CAddApngDialg, CDialog)
	ON_BN_CLICKED(IDC_APNG_FINISH, &CAddApngDialg::OnBnClickedApngFinish)
	ON_BN_CLICKED(IDC_RADIO_SNOW, &CAddApngDialg::OnBnClickedRadioSnow)
	ON_BN_CLICKED(IDC_RADIO_FISH, &CAddApngDialg::OnBnClickedRadioFish)
	ON_BN_CLICKED(IDC_CLEAR_ALL_APNG, &CAddApngDialg::OnBnClickedClearAllApng)
	ON_BN_CLICKED(IDC_DELETE_APNG, &CAddApngDialg::OnBnClickedDeleteApng)
	ON_BN_CLICKED(IDC_RADIO_BLACK_LINE, &CAddApngDialg::OnBnClickedRadioBlackLine)
	ON_BN_CLICKED(IDC_RADIO_LOVE, &CAddApngDialg::OnBnClickedRadioLove)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CAddApngDialg::OnNMClickList1)
END_MESSAGE_MAP()


char* CAddApngDialg::GetResourcePath(char* name)
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
// CAddApngDialg 消息处理程序

void CAddApngDialg::OnBnClickedApngFinish()
{
	
	// TODO: 在此添加控件通知处理程序代码
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	int64_t timeline_start = 0;
	int64_t timeline_end = 0;
	CString str = NULL;
	int ret = 0;

	if (!szPath || 0 == strlen(szPath))
		return ;

	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &timeline_end);

	GetDlgItem(IDC_EDIT_APNG_TIMELINE_START)->GetWindowText(str);
	_stscanf(str, _T("%ld"), &timeline_start);


	ret = pDlg->addApng(szPath,timeline_start,timeline_end,apng_index);
	if (ret<=0)
	{
		char buf[1024];
		sprintf(buf,"add apng error :%d ",ret);
		AfxMessageBox(buf);
		return;
	}

	//添加到listCtrl中
	int nCount = listCtrl.GetItemCount();
	int nRow = listCtrl.InsertItem(nCount, szPath);//插入行

	GetDlgItem(IDC_EDIT_APNG_TIMELINE_START)->GetWindowText(str);
	listCtrl.SetItemText(nRow, 1, str);//设置数据开始时间

	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->GetWindowText(str);
	listCtrl.SetItemText(nRow, 2, str);//设置数据结束时间

	str.Format("%d",apng_index);
	listCtrl.SetItemText(nRow, 3, str);//设置数据id

	apng_index++;
	return;
}
int CAddApngDialg::setTotalTime(int64_t totalTime)
{
	char buf[1024];
	sprintf(buf,"%lld",totalTime);
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->SetWindowText(buf);
	return 1;
}
void CAddApngDialg::OnBnClickedRadioSnow()
{
	// TODO: 在此添加控件通知处理程序代码
	char buf[1024];
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	int64_t total_time = 0;
	memset(szPath,0,MAX_PATH);
	strcpy(szPath,GetResourcePath(XPK_APNG_SNOW));
	
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_START)->SetWindowText("0");
	total_time = pDlg->getTotalTime();
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->SetWindowText(buf);
	

}

void CAddApngDialg::OnBnClickedRadioFish()
{
	// TODO: 在此添加控件通知处理程序代码
	char buf[1024];
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	int64_t total_time = 0;
	memset(szPath,0,MAX_PATH);
	strcpy(szPath,GetResourcePath(XPK_APNG_FISH));

	GetDlgItem(IDC_EDIT_APNG_TIMELINE_START)->SetWindowText("0");
	total_time = pDlg->getTotalTime();
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->SetWindowText(buf);

}

BOOL CAddApngDialg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char buf[MAX_PATH];
	int64_t total_time = 0;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();

	((CButton *)GetDlgItem(IDC_RADIO_FISH))->SetCheck(TRUE);
	strcpy(szPath,GetResourcePath(XPK_APNG_FISH));
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->SetWindowText("0");
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_START)->SetWindowText("0");

	total_time = pDlg->getTotalTime();
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->SetWindowText(buf);

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

void CAddApngDialg::OnBnClickedClearAllApng()
{
	// TODO: 在此添加控件通知处理程序代码
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();

	if(pDlg->clearAllApng()<=0)
		AfxMessageBox("clearAllApng error ");

	//清空listCtrl
	int nCount = listCtrl.GetItemCount();
	while(nCount)
	{
		listCtrl.DeleteItem(0);
		nCount--;
	}
}

void CAddApngDialg::OnBnClickedDeleteApng()
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
		pDlg->deleteApng(_ttoi(str));
		listCtrl.DeleteItem(nItem);
	}

	str = listCtrl.GetItemText(0,3);
	pApng = pDlg->getApngParams(_ttoi(str));
	if(pApng)
		refreshUI();
}

void CAddApngDialg::OnBnClickedRadioBlackLine()
{
	// TODO: 在此添加控件通知处理程序代码

	char buf[1024];
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	int64_t total_time = 0;
	memset(szPath,0,MAX_PATH);
	strcpy(szPath,GetResourcePath(XPK_APNG_BLACK_LINE));

	GetDlgItem(IDC_EDIT_APNG_TIMELINE_START)->SetWindowText("0");
	total_time = pDlg->getTotalTime();
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->SetWindowText(buf);
}

void CAddApngDialg::OnBnClickedRadioLove()
{
	// TODO: 在此添加控件通知处理程序代码

	char buf[1024];
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	int64_t total_time = 0;
	memset(szPath,0,MAX_PATH);
	strcpy(szPath,GetResourcePath(XPK_APNG_LOVE));

	GetDlgItem(IDC_EDIT_APNG_TIMELINE_START)->SetWindowText("0");
	total_time = pDlg->getTotalTime();
	sprintf(buf,"%lld",total_time);
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->SetWindowText(buf);
}
int CAddApngDialg::refreshUI()
{
	CString str = NULL;
	CString path = NULL;
	int pos = 0;

	if (!pApng)
		return 1;

	path = pApng->szPath;
	pos = path.Find("//");
	path = path.Right(path.GetLength() - pos);

	if (path == XPK_APNG_FISH)
	{
		((CButton*)GetDlgItem(IDC_RADIO_FISH))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_LOVE))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_SNOW))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_BLACK_LINE))->SetCheck(0);
	}
	else if (path == XPK_APNG_LOVE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_FISH))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_LOVE))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_SNOW))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_BLACK_LINE))->SetCheck(0);
	}
	else if (path == XPK_APNG_SNOW)
	{
		((CButton*)GetDlgItem(IDC_RADIO_FISH))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_LOVE))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_SNOW))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_BLACK_LINE))->SetCheck(0);
	}
	else if (path == XPK_APNG_BLACK_LINE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_FISH))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_LOVE))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_SNOW))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_BLACK_LINE))->SetCheck(1);
	}

	str.Format("%lld",pApng->timeline_start);
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_START)->SetWindowText(str);

	str.Format("%lld",pApng->timeline_end);
	GetDlgItem(IDC_EDIT_APNG_TIMELINE_END)->SetWindowText(str);
	return 1;

}
void CAddApngDialg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
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
		pApng = pDlg->getApngParams(_ttoi(str));
		if (!pApng)
			AfxMessageBox("获取apng参数失败！");
		else
		{
			refreshUI();
			break;
		}
	}
}
