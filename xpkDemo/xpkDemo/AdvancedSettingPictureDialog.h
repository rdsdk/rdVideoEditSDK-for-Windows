#pragma once
#include "afxwin.h"
#include "XPKCombobox.h"
#include "XPKCommon.h"
#include "XPKEditCtrlEx.h"


// CAdvancedSettingPictureDialog 对话框

class CXPKAddMedia;

class CAdvancedSettingPictureDialog : public CDialog
{
	DECLARE_DYNAMIC(CAdvancedSettingPictureDialog)

public:
	CAdvancedSettingPictureDialog(int id ,int imageWidth,int imageHeight,enum MEDIA_TYPES type,int64_t totalTime,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdvancedSettingPictureDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_PICTURE_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void set_DropDownHight(CXPKCombobox*box,UINT LinesToDisplay);
	void set_DropDownWidth(CXPKCombobox*box,UINT nWidth );
	int refreshUI();	
	int picturePropertyChange(int change);
	double getMediaSpeed();
	int64_t getMediaCutStartTime();
	int64_t getMediaCutEndTime();
	int64_t getMediaInalienableTime();
	int		getMediaRotateAngle();
	int		getMediaFileType();
	RECTF   getMediaClip();

	int m_resouceId;				

	CXPKCombobox *m_pSpeedBox;
	CXPKCombobox *m_pRorateBox;
	CXPKCombobox *m_pFlipBox;
	CXPKCombobox *m_pClipBox;
	
	int m_media_width;						//媒体宽高
	int m_media_height;						//媒体宽高
	enum MEDIA_TYPES m_media_type;			//媒体类型

	RECTF rtClip;							//裁剪区域
	int angle;								//旋转角度
	int propertyChange;						//媒体属性是否发生改变
	int64_t m_media_total_time;				//媒体总时间

	CXPKAddMedia *m_pParent;
};
