#pragma once

#include "AdvancedSettingAudioDialog.h"
#include "XPKCommon.h"
#include "showSnapshot.h"
#include "setButton.h"




extern struct XPK_MUSIC_LIST;

// CXPKAddMusic 对话框

class CXPKAddMusic : public CPropertyPage
{
	DECLARE_DYNAMIC(CXPKAddMusic)

public:
	CXPKAddMusic();
	virtual ~CXPKAddMusic();

// 对话框数据
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddMusic();

	int showAdvanceDlg(int id);
	int addMusic(char* path,int64_t total_time);

	afx_msg void OnBnClickedDeleteMusic();
	afx_msg void OnBnClickedDeleteAllMusic();
	int refreshSnapshot();
	

	XPK_MUSIC_LIST* m_pMusicList;
	int m_change;
	Handle hSlide;
	
};
