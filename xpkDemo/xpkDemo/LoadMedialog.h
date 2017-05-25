#pragma once


// CLoadMedialog 对话框

class CLoadMedialog : public CDialog
{
	DECLARE_DYNAMIC(CLoadMedialog)

public:
	CLoadMedialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoadMedialog();

	int loadMediaFinish();
	int loadMediaing();
// 对话框数据
	enum { IDD = IDD_LOADIMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
//	afx_msg void OnTimer(UINT_PTR nIDEvent);

	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	int loaded;
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
};
