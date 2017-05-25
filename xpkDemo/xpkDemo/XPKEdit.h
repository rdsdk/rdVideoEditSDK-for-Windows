#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "XPKCombobox.h"


// CXPKEdit 对话框

class CXPKEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CXPKEdit)

public:
	CXPKEdit();
	virtual ~CXPKEdit();

// 对话框数据
	enum { IDD = IDD_XPKEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual LRESULT OnWizardBack();
	afx_msg void OnBnClickedSetOutputPath();

	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedEdit();
	char* GetResourcePath(char* name);
	void set_DropDownHight(CComboBox*box,UINT LinesToDisplay);
	void set_DropDownWidth(CComboBox*box,UINT nWidth );
	int getOutPutResolution();
	afx_msg void OnCbnSelchangeEditResolution();
	afx_msg void OnSize(UINT nType, int cx, int cy);


	char szFilePath[MAX_PATH + 1];
	int edit_width;
	int edit_height;
	HANDLE m_EditThread;			//制作线程
	HANDLE m_UIThread;				//UI线程
	HANDLE hSlide;

	CEdit m_outPutPathCtrl;			//输出文件路径控件
	CProgressCtrl m_progressCtrl;	//输出文件制作进度控件

	CComboBox m_ResolutionCtrl;		//输出分辨率
	int m_exit;


	float m_static_out_left_zoom;
	float m_static_out_top_zoom;
	float m_static_resolution_left_zoom;
	float m_edit_out_left_zoom;
	float m_edit_resolution_left_zoom;

	float m_display_width_zoom;
	float m_display_top_zoom;

	float m_progress_left_zoom;
	float m_progress_top_zoom;

	float m_button_edit_top_zoom;

	
	afx_msg void OnClose();
};
