#pragma once
#include "afxwin.h"
#include "XPKCommon.h"
#include "afxcmn.h"
// CAddSubtitleDialg 对话框



class CAddSubtitleDialg : public CDialog
{
	DECLARE_DYNAMIC(CAddSubtitleDialg)

public:
	CAddSubtitleDialg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddSubtitleDialg();

// 对话框数据
	enum { IDD = IDD_SUBTITLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedSubtitleFinish();
	afx_msg void OnBnClickedBold();
	afx_msg void OnBnClickedItalics();
	afx_msg void OnBnClickedUnderline();
	afx_msg void OnBnClickedDeleteSubtitle();
	afx_msg void OnBnClickedClearAllSubtitle();

	afx_msg void OnBnClickedFontColor();
	void set_DropDownHight(CComboBox*box,UINT LinesToDisplay);
	void set_DropDownWidth(CComboBox*box,UINT nWidth );
	int setTotalTime(int64_t totalTime);
	
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	int refreshUI();

	//颜色
	int m_ColorRed;
	int m_ColorGreen;
	int m_ColorBlue;

	//属性
	int m_bold;
	int m_underline;
	int m_italics;
	//字幕
	char szText[2048];

	//字体
	RECTF fontRect;
	FONT_PEP font;
	RGBQUAD clr;

	CComboBox m_fontSizeCtrl;
	CComboBox m_fontNameCtrl;

	int m_fontNameNumber;
	
	CListCtrl listCtrl;
	int subtitle_id;
	
	XPK_SUBTITLE_LIST* pSubtitle;
	
};
