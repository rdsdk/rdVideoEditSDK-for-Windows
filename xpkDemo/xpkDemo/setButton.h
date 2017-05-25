#pragma once

#include "XPKCommon.h"
// CSetButton

class CSetButton : public CButton
{
	DECLARE_DYNAMIC(CSetButton)

public:
	CSetButton();
	virtual ~CSetButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

};


