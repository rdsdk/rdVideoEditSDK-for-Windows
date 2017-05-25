// ShowSnapshot.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowSnapshot.h"
#include "XPKAddMedia.h"


// CShowSnapshot

IMPLEMENT_DYNAMIC(CShowSnapshot, CStatic)

CShowSnapshot::CShowSnapshot()
{

	m_pImage		= NULL;
	m_pDisplay		= NULL;
	m_selected		= 0;
	m_imageHeigt	= 0;
	m_imageWidth	= 0;

}

CShowSnapshot::~CShowSnapshot()
{

	if(m_pDisplay)
		free(m_pDisplay);
}


BEGIN_MESSAGE_MAP(CShowSnapshot, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CShowSnapshot 消息处理程序
int  CShowSnapshot::rgb24torgb32(int8_t* rgb24,unsigned int width,unsigned int height,int8_t* rgb32)
{

	int m = 0;
	int n = 0;

	for (int i = 0;i<height; i++)
	{
		for (int j = 0;j<width;j++)
		{
			rgb32[i*width*4+j*4 +2] = rgb24[i*width*3+j*3];
			rgb32[i*width*4+j*4 +1] = rgb24[i*width*3+j*3+1];
			rgb32[i*width*4+j*4 +0] = rgb24[i*width*3+j*3+2];
			rgb32[i*width*4+j*4 +3] = 255;
		}

	}
	return 1;

}
int CShowSnapshot::init(int show_width ,int show_height )
{

	BOOL bRes; 
	UINT32 nNumPalettes;

	m_hDrawDib = DrawDibOpen();
	m_hDC = ::GetDC(m_hWnd);
	GetClientRect(&m_rectClient);

	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.biSize = sizeof(m_bi);     
	m_bi.biWidth = show_width;
	m_bi.biHeight = show_height;

	m_bi.biPlanes = 1;
	m_bi.biBitCount = 32;//assume RGB32

	m_bi.biCompression = BI_RGB;//RGB
	m_bi.biSizeImage = m_bi.biWidth*m_bi.biHeight*m_bi.biBitCount/8;

	bRes = DrawDibBegin(m_hDrawDib, m_hDC, m_rectClient.right, m_rectClient.bottom,
		&m_bi, m_bi.biWidth, m_bi.biHeight, 0);

	nNumPalettes = DrawDibRealize(m_hDrawDib, m_hDC, FALSE);
	return 1;
}

int CShowSnapshot::SetSnapshot(int8_t* buff,int width,int height)
{
	if(!buff)
		return -__LINE__;
	if (width <= 0 || height <= 0)
		return -__LINE__;

	m_pImage = buff;
	m_imageHeigt = height;
	m_imageWidth = width;
	return 1;
}
void CShowSnapshot::DrawVideoRGB32(int8_t*buff, int width, int height)
{	
	UCHAR*pBuffer;
	int sws_flags = SWS_BICUBIC;
	BOOL bResult = FALSE;
	int m_nDrawWidth;
	int m_nDrawHeight;
	int m_nDrawLeft;
	int m_nDrawTop;
	int iRet;

	int dwPitch = width*4;
	BYTE *pDown = NULL;
	BYTE *pTmp = NULL;
	int m=0;

	if (!m_pDisplay)
	{
		m_pDisplay = (UCHAR*)malloc(width*height*4);
		if(!m_pDisplay)
			return ;
	}

	pDown = (BYTE *)buff+(height-1)*dwPitch;
	pTmp = (BYTE *)m_pDisplay;

	m_nDrawLeft = 0;
	m_nDrawTop = 0;
	m_nDrawWidth = width;
	m_nDrawHeight = height;

	for (m=0; m<height; m++)
	{
		memcpy(pTmp,pDown,dwPitch);
		pTmp+=dwPitch;
		pDown -= dwPitch;
	}

	//Important
	m_bi.biWidth = m_nDrawWidth;
	m_bi.biHeight = m_nDrawHeight;

	bResult = DrawDibDraw(m_hDrawDib, m_hDC, 
		m_nDrawLeft, m_nDrawTop, /*m_nDrawWidth*/SNAPSHOT_WIDTH, /*m_nDrawHeight*/SNAPSHOT_HEIGHT, 
		&m_bi, (void*) m_pDisplay, 
		0, 0, m_nDrawWidth, m_nDrawHeight, 0);

}

void CShowSnapshot::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	if(m_pImage)
		DrawVideoRGB32(m_pImage,m_imageWidth,m_imageHeigt);


	
		CRect rect;
		CClientDC dc1(this);
		rect.left = 0;
		rect.top = 0;
		rect.right = SNAPSHOT_WIDTH;
		rect.bottom = SNAPSHOT_HEIGHT;

		if (m_selected)
		{
			CPen pen(PS_SOLID,5,RGB(234,23,53));
			CPen *pOldPen=dc1.SelectObject(&pen);
			CBrush *pBrush=CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
			CBrush *pOldBrush=dc1.SelectObject(pBrush);
			dc1.Rectangle(&rect);
			dc1.SelectObject(pOldPen);
			dc1.SelectObject(pOldBrush);
		}
		else
		{
			CPen pen(PS_SOLID,5,RGB(255,255,255));
			CPen *pOldPen=dc1.SelectObject(&pen);
			CBrush *pBrush=CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
			CBrush *pOldBrush=dc1.SelectObject(pBrush);
			dc1.Rectangle(&rect);
			dc1.SelectObject(pOldPen);
			dc1.SelectObject(pOldBrush);
		}
}

void CShowSnapshot::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_selected = !m_selected;
	Invalidate();

	if (m_selected)
	{
		CXPKAddMedia* pDlg =(CXPKAddMedia*)GetParent();
		pDlg->display(this);
	}
	CStatic::OnLButtonDown(nFlags, point);
}
int CShowSnapshot::setSelected()
{
	m_selected = !m_selected;
	Invalidate();
	return 1;
}
int CShowSnapshot::cancelSelected()
{
	m_selected = !m_selected;
	Invalidate();
	return 1;
}
int CShowSnapshot::isSelected()
{
	return m_selected;
}
void CShowSnapshot::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CStatic::OnLButtonUp(nFlags, point);
}

int CShowSnapshot::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	init();

	return 0;
}
