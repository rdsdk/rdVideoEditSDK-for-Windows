#pragma once



#include "afxwin.h"
#include "XPKCombobox.h"
#include "XPKCommon.h"
#include "XPKAddMedia.h"

// CAdvancedSettingMediaDialog 对话框

class CAdvancedSettingMediaDialog : public CDialog
{
	DECLARE_DYNAMIC(CAdvancedSettingMediaDialog)

public:
	CAdvancedSettingMediaDialog(int id ,int imageWidth,int imageHeight,enum MEDIA_TYPES type,int64_t totalTime,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdvancedSettingMediaDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_MEDIA_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	double getMediaSpeed();
	int64_t getMediaCutStartTime();
	int64_t getMediaCutEndTime();
	int64_t getMediaInalienableTime();

	afx_msg void OnBnClickedCheck1();

	int getDynamicClipRect(CString str,RECTF* rect);
	XPK_DYNAMAIC_CLIP_LIST* getDynamicClipList();
	int UpdateClipList();
	afx_msg void OnEnChangeMediaClipEndTime1();
	afx_msg void OnEnChangeMediaClipEndTime2();
	int addClipList(RECTF rect,int64_t time_start,int64_t time_end);

	int refreshUI();
	int picturePropertyChange( int change);
	int		getMediaRotateAngle();
	int		getMediaFileType();
	RECTF   getMediaClip();

	int m_resouceId;
	RECTF rtClip;				//裁剪区域
	int propertyChange ;		//媒体属性是否发生改变
	int64_t m_media_total_time;	//媒体的总时间
	int angle;					//旋转角度
	int m_media_height;			//媒体的宽高
	int m_media_width;			//媒体的宽高
	int64_t m_cut_start_time;	//媒体的截取开始时间
	int64_t m_cut_end_time;		//媒体的截取结束时间

	CXPKCombobox *m_pRorateBox;
	CXPKCombobox *m_pFlipBox;
	CXPKCombobox *m_pClipBox;
	CXPKCombobox *m_pSpeedBox;
	CButton m_useDynamicClip;

	//动态裁剪
	XPK_DYNAMAIC_CLIP_LIST* pClipList;
	int m_init;
	CXPKAddMedia *m_pParent;	//父窗口
	
};
