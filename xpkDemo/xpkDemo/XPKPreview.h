#pragma once




#include "XPKSlider.h"
#include "XPKCommon.h"
#include "afxcmn.h"
#include "AddApngDialg.h"
#include "AddMusicDialg.h"
#include "AddSubtitleDialg.h"
#include "AddFilterDialog.h"


// CXPKPreview 对话框

class CXPKPreview : public CPropertyPage
{
	DECLARE_DYNAMIC(CXPKPreview)

public:
	CXPKPreview();
	virtual ~CXPKPreview();

// 对话框数据
	enum { IDD = IDD_XPKPREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnPreviewSeek(WPARAM,LPARAM);
	afx_msg LRESULT OnPreviewPause(WPARAM,LPARAM);
	int refreshTabCtrlUI();


public:
	virtual BOOL OnInitDialog();
	char* GetResourcePath(char* name);
	int getShowRect(RECT *rect);
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	int64_t getTotalTime();

	int clearAllFilter();
	int addFilter(int id);

	int addMusic(char* path,int64_t timeline_start,int64_t timeline_end,
		int64_t time_starat,int64_t time_end,int64_t fade_in,int64_t fade_out,float factor,int index);
	int addApng(char* szPath,int64_t timeline_start,int64_t timeline_end,int index);
	int addSubtitle(char* szText,int64_t timeline_start,int64_t timeline_end,FONT_PEP font,RECTF fontRect,char* srtFile,int index);
	int clearAllSubtitle();
	int clearAllApng();
	int clearAllMusic();
	int deleteSubtitle(int index);
	int deleteApng(int index);
	int deleteMusic(int index);
	struct XPK_APNG_RGB_OVERLAY_LIST* getApngParams(int index);
	XPK_MUSIC_LIST* getMusicParams(int index);
	XPK_SUBTITLE_LIST* getSubtitleParams(int index);

	CTabCtrl m_tabctrl;					//tabCtrl
	CAddApngDialg m_addApng;
	CAddMusicDialg m_addMusic;
	CAddSubtitleDialg m_addSubtitle;
	CAddFilterDialog m_addFilter;


	char szFilePath[MAX_PATH + 1];
	int preview_width;								//openGL显示窗口的宽高
	int preview_height;								//openGL显示窗口的宽高
	HANDLE hSlide;
	CXPKSlider *pSlider;
	HANDLE hUIThread;								//UI线程

	int stop;										//停止播放
	struct XPK_MUSIC_LIST *pMusicList ;				//配乐链表
	struct XPK_SUBTITLE_LIST *pSubtitleList;		//字幕链表
	struct XPK_APNG_RGB_OVERLAY_LIST *pApngList;	//apng特效链表


	//缩放比例
	float m_button_stop_left_zoom;
	float m_button_play_left_zoom;

	float m_display_top_zoom;
	float m_display_left_zoom;
	float m_display_width_zoom;

	float m_tabCtrl_left_zoom;

	float m_slider_top_zoom;

	float m_static_start_time_top_zoom;
	float m_static_end_time_top_zoom;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
};
