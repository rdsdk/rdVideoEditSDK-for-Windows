#pragma once


// CAdvancedSettingVideoDialog 对话框

class CAdvancedSettingVideoDialog : public CDialog
{
	DECLARE_DYNAMIC(CAdvancedSettingVideoDialog)

public:
	CAdvancedSettingVideoDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdvancedSettingVideoDialog();

// 对话框数据
	enum { IDD = IDD_ADVANCEDSETTINGVIDEODIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
