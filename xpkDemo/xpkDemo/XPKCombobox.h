#pragma once


// CXPKCombobox

class CXPKCombobox : public CComboBox
{
	DECLARE_DYNAMIC(CXPKCombobox)

public:
	CXPKCombobox(int combobox_type);
	virtual ~CXPKCombobox();

	void set_DropDownHight(CComboBox*box,UINT LinesToDisplay);
	void set_DropDownWidth(CComboBox*box,UINT nWidth );
	int init();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCbnSelchange();
	int getCurTransition();

	int m_curSel;
	int m_change;
	int m_type;
};


