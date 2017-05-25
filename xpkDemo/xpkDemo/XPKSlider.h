#pragma once


// CXPKSlider

class CXPKSlider : public CSliderCtrl
{
	DECLARE_DYNAMIC(CXPKSlider)

public:
	CXPKSlider();
	virtual ~CXPKSlider();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	int seek;
};


