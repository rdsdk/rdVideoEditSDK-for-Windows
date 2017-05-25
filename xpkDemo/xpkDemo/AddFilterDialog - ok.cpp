// AddFilterDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "xpkDemo.h"
#include "AddFilterDialog.h"
#include "XPKPreview.h"


// CAddFilterDialog 对话框

IMPLEMENT_DYNAMIC(CAddFilterDialog, CDialog)

CAddFilterDialog::CAddFilterDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFilterDialog::IDD, pParent)
{

}

CAddFilterDialog::~CAddFilterDialog()
{
}

void CAddFilterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, listCtrl);
}


BEGIN_MESSAGE_MAP(CAddFilterDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CAddFilterDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_FLEETING_TIME, &CAddFilterDialog::OnBnClickedLomo)
	ON_BN_CLICKED(IDC_FINISH, &CAddFilterDialog::OnBnClickedFinish)
	ON_BN_CLICKED(IDC_SUNSET_GLOW, &CAddFilterDialog::OnBnClickedSunsetGlow)
	ON_BN_CLICKED(IDC_HAWAIIAN_LSLANDS, &CAddFilterDialog::OnBnClickedHawaiianLslands)
	ON_BN_CLICKED(IDC_OVERCAST, &CAddFilterDialog::OnBnClickedOvercast)
	ON_BN_CLICKED(IDC_DUSK, &CAddFilterDialog::OnBnClickedDusk)
	ON_BN_CLICKED(IDC_FLEETING_TIME, &CAddFilterDialog::OnBnClickedFleetingTime)
	ON_BN_CLICKED(IDC_MIGRATORY_BIRD, &CAddFilterDialog::OnBnClickedMigratoryBird)
	ON_BN_CLICKED(IDC_CLOUD, &CAddFilterDialog::OnBnClickedCloud)
	ON_BN_CLICKED(IDC_DELETE, &CAddFilterDialog::OnBnClickedDelete)
END_MESSAGE_MAP()


char* CAddFilterDialog::GetResourcePath(char* name)
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

// CAddFilterDialog 消息处理程序

void CAddFilterDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	//遍历listCtrl添加fliter
	


	OnOK();
}

BOOL CAddFilterDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	DWORD dwStyle = listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	listCtrl.SetExtendedStyle(dwStyle); //设置扩展风格

	listCtrl.InsertColumn( 0, "PATH", LVCFMT_LEFT, 150 );
	listCtrl.InsertColumn( 1, "filter_id", LVCFMT_LEFT, 40 );//插入列
	listCtrl.InsertColumn( 2, "ctrl_id", LVCFMT_LEFT, 40 );//插入列

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

int CAddFilterDialog::refreshUI(int ctrl_id,int filter_id)
{

	CString str = NULL;
	char szPath[MAX_PATH] = {0};

	if(((CButton*)GetDlgItem(ctrl_id))->GetCheck())
	{
		//如果选中添加到listCtrl中
		int nRow = 0;
		int nCount = listCtrl.GetItemCount();
		strcpy(szPath,findFile(filter_id));
		nRow = listCtrl.InsertItem(nCount, szPath);//插入行
		str.Format("%d",filter_id);
		listCtrl.SetItemText(nRow, 1, str);//
		str.Format("%d",ctrl_id);
		listCtrl.SetItemText(nRow, 2, str);//
	}
	else
	{
		//如果没有选中，listCtrl删除该滤镜
		int nCount = listCtrl.GetItemCount();
		for (int i = 0; i< nCount;i++)
		{
			str = listCtrl.GetItemText(i,0);
			if (str == findFile(filter_id))
				listCtrl.DeleteItem(i);
		}
	}

	return 1;
}
char* CAddFilterDialog::findFile(int id)  
{  
	int filter_id = 0;
	char szFind[MAX_PATH],szFile[MAX_PATH];  
	WIN32_FIND_DATA FindFileData;  
	HANDLE hFind;

	char pFilePath[MAX_PATH+1];
	char path[MAX_PATH + 1];

	//获取当前支援路径
	memset(pFilePath,0,MAX_PATH+1);
	memset(path,0,MAX_PATH+1);
	GetModuleFileName(NULL, path, MAX_PATH);
	(strrchr(path, ('\\')))[1] = 0; //删除文件名，只获得路径
	memcpy(pFilePath,path,strlen(path));
	strcat(pFilePath,"\\resource\\filter\\"); 

	//遍历资源目录
	strcpy(szFind,pFilePath);  
	strcat(szFind,"\\*.*"); 
	hFind = FindFirstFile(szFind,&FindFileData);  
	if(INVALID_HANDLE_VALUE == hFind)  
		return NULL;  
	while(TRUE)  
	{  
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  
		{  
			if(FindFileData.cFileName[0]!='.')  
			{  
				strcpy(szFile,pFilePath);  
				strcat(szFile,"\\");  
				strcat(szFile,FindFileData.cFileName);  
				findFile(id);  
			}  
		}  
		else  
		{  
			filter_id = atoi(FindFileData.cFileName);
			if (filter_id == id)
			{
				strcat(pFilePath,FindFileData.cFileName);
				return pFilePath;
			}
			printf("%d\n",filter_id); 
		}  
		if(!FindNextFile(hFind,&FindFileData))  
			break;  
	}  
	return NULL;
}  
void CAddFilterDialog::OnBnClickedLomo()
{
	// TODO: 在此添加控件通知处理程序代码
	refreshUI(IDC_FLEETING_TIME,VFT_ACV_FLEETING_TIME);
}

void CAddFilterDialog::OnBnClickedFinish()
{
	// TODO: 在此添加控件通知处理程序代码

	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	CString str = NULL;
	int nCount  = listCtrl.GetItemCount();

	pDlg->clearAllFilter();
	//添加滤镜
	for (int i = 0; i< nCount;i++)
	{
		str = listCtrl.GetItemText(i,1);
		pDlg->addFilter(_ttoi(str));
	}
	pDlg->addFilter(0);
}

void CAddFilterDialog::OnBnClickedSunsetGlow()
{
	// TODO: 在此添加控件通知处理程序代码

	refreshUI(IDC_SUNSET_GLOW,VFT_ACV_SUNSET_GLOW);
}

void CAddFilterDialog::OnBnClickedHawaiianLslands()
{
	// TODO: 在此添加控件通知处理程序代码
	refreshUI(IDC_HAWAIIAN_LSLANDS,VFT_ACV_HAWAIIAN_LSLANDS);
}

void CAddFilterDialog::OnBnClickedOvercast()
{
	// TODO: 在此添加控件通知处理程序代码
	refreshUI(IDC_OVERCAST,VFT_ACV_OVERCAST);
}

void CAddFilterDialog::OnBnClickedDusk()
{
	// TODO: 在此添加控件通知处理程序代码
	refreshUI(IDC_DUSK,VFT_ACV_DUSK);
}

void CAddFilterDialog::OnBnClickedFleetingTime()
{
	// TODO: 在此添加控件通知处理程序代码
	refreshUI(IDC_FLEETING_TIME,VFT_ACV_FLEETING_TIME);
}

void CAddFilterDialog::OnBnClickedMigratoryBird()
{
	// TODO: 在此添加控件通知处理程序代码
	refreshUI(IDC_MIGRATORY_BIRD,VFT_ACV_MIGRATORY_BIRD);
	
}

void CAddFilterDialog::OnBnClickedCloud()
{
	// TODO: 在此添加控件通知处理程序代码
	refreshUI(IDC_CLOUD,VFT_ACV_CLOUD);
}

void CAddFilterDialog::OnBnClickedDelete()
{
	// TODO: 在此添加控件通知处理程序代码

	CString str = NULL;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetParent();
	CXPKPreview* pDlg = (CXPKPreview*)pTabCtrl->GetParent();
	// TODO: 在此添加控件通知处理程序代码
	while(listCtrl.GetNextItem(-1,LVNI_ALL | LVNI_SELECTED) != -1)
	{
		int nItem = listCtrl.GetNextItem(-1,LVNI_ALL | LVNI_SELECTED);
		str = listCtrl.GetItemText(nItem,2);
		((CButton*)GetDlgItem(_ttoi(str)))->SetCheck(0);
		listCtrl.DeleteItem(nItem);
	}
}
