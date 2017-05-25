#pragma once


#include "XPKPreview.h"
#include "XPKEdit.h"
#include "XPKAddMedia.h"
#include "XPKCommon.h"
// XPKWizard


#define WM_RESIZEPAGE WM_USER + 111

typedef struct XPK_MEDIA_LIST
{
	char path[MAX_PATH];
	Handle handle;
	int widthSrc;				//媒体的真实宽
	int heightSrc;				//媒体的真实高
	enum MEDIA_TYPES type;		//媒体类型
	int width;					//缩略图宽
	int height;					//缩略图高
	unsigned char* pImage;		//缩略图数据
	int64_t total_time;			//媒体的总时间
	int id;						//资源ID

	//缩略图显示
	CShowSnapshot* snapshot;	//显示画面控件
	RECT snapshot_rect;			//显示区域
	
	CSetButton* button;			//编辑按钮
	RECT button_rect;
	
	CAdvancedSettingPictureDialog* pPictureDlg;		//图片编辑对话框
	CAdvancedSettingMediaDialog* pMediaDlg;			//视频编辑对话框
	
	CXPKCombobox* transition_box;			//转场combobox
	RECT transition_box_rect;

	XPK_MEDIA_LIST* next;
};



class XPKWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(XPKWizard)

public:
	XPKWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	XPKWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~XPKWizard();

protected:
	DECLARE_MESSAGE_MAP()

	CXPKEdit m_editDlg;
	CXPKPreview m_previewDlg;
	CXPKAddMedia m_addDlg;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnResizePage(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	int init();

	int clearAllFilter();
	char* GetResourcePath(char* name);
	int getClipRect(XPK_MEDIA_LIST* pList,RECTF* rtClip);
	int adddubmusic();
	int addSubtitle();
	int addApngList();

	int64_t total_time;				//总时间
	Handle hSlide;
	XPK_MEDIA_LIST* m_pMediaList;	//媒体链表
	XPK_FILTER_LIST* m_pFilterList;	//滤镜链表
	char szFilePath[MAX_PATH];
	XPK_STATU statu ;				//当前状态

	float m_button_next_zoom;
	float m_button_back_zoom;
	float m_button_finish_zoom;

	afx_msg void OnClose();
};


