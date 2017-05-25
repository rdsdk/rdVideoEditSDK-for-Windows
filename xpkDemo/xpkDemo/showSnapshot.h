#ifndef __SHOW_SNAPSHOW_H__
#define __SHOW_SNAPSHOW_H__


#include "windows.h"
#include "vfw.h"


#include "XPKCommon.h"


// CShowSnapshot

class CShowSnapshot : public CStatic
{
	DECLARE_DYNAMIC(CShowSnapshot)

public:
	CShowSnapshot();
	virtual ~CShowSnapshot();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();

public:

	int init(int show_width = SNAPSHOT_WIDTH,int show_height = SNAPSHOT_HEIGHT);
	//
	int SetSnapshot(int8_t* buff,int width,int height);
	//绘制函数
	void DrawVideoRGB32(int8_t*buff, int width, int height);

	int  rgb24torgb32(int8_t* rgb24,unsigned int width,unsigned int height,int8_t* rgb32);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	int isSelected();
	int cancelSelected();
	int setSelected();


	HDRAWDIB m_hDrawDib;
	HDC m_hDC;
	RECT m_rectClient;
	BITMAPINFOHEADER m_bi;
	//rgb32数据
	int8_t* m_pImage;
	int m_imageWidth;
	int m_imageHeigt;

	UCHAR* m_pDisplay;
	//是否被选中
	int m_selected;
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


#endif


