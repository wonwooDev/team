// GlassDetectView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PyroSoftM.h"

#include "MainFrm.h"

#include "GlassDetectView.h"



// CGlassDetectView

IMPLEMENT_DYNCREATE(CGlassDetectView, CView)

CGlassDetectView::CGlassDetectView()
{
	m_Dlg_H = 100;
	pDoc = NULL;

	m_bmp_size_x = 0;
	m_bmp_size_y = 0;
	m_bmp_ofs_x = 0;
	m_bmp_ofs_y = 0;
	m_zoomRate = 3.0;

	m_Margin = 24;
}

CGlassDetectView::~CGlassDetectView()
{
	
}

BEGIN_MESSAGE_MAP(CGlassDetectView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()	
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// CGlassDetectView 진단입니다.

#ifdef _DEBUG
void CGlassDetectView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CGlassDetectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CGlassDetectView 메시지 처리기입니다.


int CGlassDetectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
		
	if(m_ResultDlg.GetSafeHwnd() == 0)
	{		
		if(!m_ResultDlg.Create(IDD_RESULT, this))
			return FALSE;	
		
		m_ResultDlg.ModifyStyle(0,WS_VISIBLE, 0);

		AdjustLayout();
	}	
	

	return 0;
}

void CGlassDetectView::AdjustLayout()
{	
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	//int dlgH = rectClient.Height() - m_Dlg_H;
	int dlgH = 0;

	m_ResultDlg.SetWindowPos(NULL, rectClient.left, 
									rectClient.top + dlgH, 
									rectClient.Width(), 
									rectClient.Height() - dlgH, 
									SWP_NOACTIVATE | SWP_NOZORDER);
}

void CGlassDetectView::OnSize(UINT nType, int cx, int cy)
{
	if(GetSafeHwnd()==NULL) return;

	CView::OnSize(nType, cx, cy);

	AdjustLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////
void CGlassDetectView::UpateMemDC()
{
	if(pDoc->m_Result[0].pROIBuffer == NULL) return;
	//m_data_X = 8;
	//m_data_Y = 8;

	UINT start_x = pDoc->m_Result[0].left;		
	UINT end_x = pDoc->m_Result[0].right; 
	UINT start_y = pDoc->m_Result[0].top;
	UINT end_y = pDoc->m_Result[0].bottom;
		
	
	m_data_X = end_x-start_x+1;
	m_data_Y = end_y-start_y+1;
		
	CPaintDC dc(this);


	bmpBuffer.DeleteObject();
	memDC.DeleteDC();

	memDC.CreateCompatibleDC(&dc);

	// 호환성 있는 메모리 비트맵 생성    
	bmpBuffer.CreateCompatibleBitmap(&dc, m_data_X, m_data_Y);
	
    // 메모리 비트맵 선택
    CBitmap *pOB1 = (CBitmap*)memDC.SelectObject(&bmpBuffer);
	
	//******************************************************************
	COLORREF color;
	float temp = 0.0f;	
	unsigned short idx;
	unsigned short colorStep = pDoc->m_NoCol;

	float minTemp, maxTemp;

	minTemp = pDoc->m_ScaleMin;
	maxTemp = pDoc->m_ScaleMax;
	
	//*
	for(int y=0; y<m_data_Y; y++) 
	{
		for(int x=0; x<m_data_X; x++) 
		{
			//temp = pDoc->m_pSpotAreaBuffer[m_data_X*y + x];
			temp = pDoc->m_Result[0].pROIBuffer[m_data_X*y + x];
			
			if(temp>maxTemp) 
				idx = colorStep-1;
			else if(temp<minTemp) 
				idx = 0;
			else 
				idx = (unsigned short)((colorStep-1) * (temp-minTemp)/(maxTemp-minTemp));
			

			theApp.DDAQ_IRDX_PALLET_GetBarColor(pDoc->m_hIRDX_Doc, idx, &color);
			
			memDC.SetPixel(x, y, color);
		}
	}	
	//*/
}

///////////////////////////////////////////////////////////////////////////////////////////
// CGlassDetectView 그리기입니다.

void CGlassDetectView::OnDraw(CDC* pDC)
{
	return;

	if(pDoc==NULL) return;

	if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
		return;
	
	CRect	cr;
	GetClientRect(&cr);

	// window size
	int	wnd_sizex = cr.Width() - 2*m_Margin;
	int	wnd_sizey = cr.Height() - m_Dlg_H - 2*m_Margin;
	
	// calculate additional stretch zoom
	float zoomx = (float) wnd_sizex / (float) m_data_X;
	float zoomy = (float) wnd_sizey / (float) m_data_Y;
	
	m_zoomRate = __min(zoomx, zoomy);
	if (m_zoomRate < 0.1f)
		m_zoomRate = 0.1f;

	// destination size
	m_bmp_size_x = (int) (m_zoomRate*m_data_X);
	m_bmp_size_y = (int) (m_zoomRate*m_data_Y);

	// center bitmap to the window
	m_bmp_ofs_x = (wnd_sizex-m_bmp_size_x) / 2 + m_Margin;
	m_bmp_ofs_y = (wnd_sizey-m_bmp_size_y) / 2 + m_Margin;

	// SetStretchMode
	::SetStretchBltMode(pDC->m_hDC, HALFTONE);
		
	pDC->StretchBlt(m_bmp_ofs_x, m_bmp_ofs_y, m_bmp_size_x, m_bmp_size_y, &memDC, 0, 0, m_data_X, m_data_Y, SRCCOPY);

	// Mark Min/Max;
	/*
	if(pDoc->m_bShowPointer && m_bmp_size_x>4 && m_bmp_size_y>4) 
	{
		int min_X, min_Y, max_X, max_Y;
		COLORREF col;
			

		pDC->SelectStockObject(NULL_BRUSH);		
		CFont font;
		font.CreatePointFont(100, "Tahoma");
		pDC->SelectObject(&font);

		//pDC->SetBkMode(TRANSPARENT);
		//pDC->SetROP2(R2_XORPEN);
		//pDC->SetROP2(R2_NOT);

		CPen WhitePen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen RedPen(PS_SOLID, 1, RGB(255, 0, 0));
		CPen BluePen(PS_SOLID, 1, RGB(0, 0, 255));

		theApp.DDAQ_IRDX_PALLET_GetBarColor(pDoc->m_hIRDX_Doc, pDoc->m_NoCol-1, &col);
		CPen minPen(PS_SOLID, 2, col);
		theApp.DDAQ_IRDX_PALLET_GetBarColor(pDoc->m_hIRDX_Doc, 0, &col);
		CPen maxPen(PS_SOLID, 2, col);

		min_X = (int)(m_bmp_ofs_x + (pDoc->m_nMin_X + 0.5)*m_zoomRate);
		min_Y = (int)(m_bmp_ofs_y + (pDoc->m_nMin_Y + 0.5)*m_zoomRate);

		max_X = (int)(m_bmp_ofs_x + (pDoc->m_nMax_X + 0.5)*m_zoomRate);
		max_Y = (int)(m_bmp_ofs_y + (pDoc->m_nMax_Y + 0.5)*m_zoomRate);


		// Min
		int out=5, in=4;

		pDC->SelectObject(&WhitePen);		
		pDC->Rectangle(min_X-out, min_Y-1, min_X+out+1, min_Y+2);
		pDC->Rectangle(min_X-1, min_Y-out, min_X+2, min_Y+out+1);

		pDC->Rectangle(max_X-out, max_Y-1, max_X+out+1, max_Y+2);
		pDC->Rectangle(max_X-1, max_Y-out, max_X+2, max_Y+out+1);
		
		pDC->SelectObject(&BluePen);
		pDC->MoveTo(min_X-in, min_Y);
		pDC->LineTo(min_X+in+1, min_Y);
		pDC->MoveTo(min_X, min_Y-in);
		pDC->LineTo(min_X, min_Y+in+1);

		pDC->SelectObject(&RedPen);
		pDC->MoveTo(max_X-in, max_Y);
		pDC->LineTo(max_X+in+1, max_Y);
		pDC->MoveTo(max_X, max_Y-in);
		pDC->LineTo(max_X, max_Y+in+1);
	}
	*/
}


///////////////////////////////////////////////////////////////////////////////////////////
void CGlassDetectView::UpdateResult()
{
	CString str;
	
	for(int i=0; i<8; i++) 
	{
		str.Format(_T("%.2f"), pDoc->m_Result[i].Min);	
		m_ResultDlg.m_EditMin[i].SetWindowText(str);
		str.Format(_T("%.2f"), pDoc->m_Result[i].Max);	
		m_ResultDlg.m_EditMax[i].SetWindowText(str);	
		str.Format(_T("%.2f"), pDoc->m_Result[i].Avg);	
		m_ResultDlg.m_EditAvg[i].SetWindowText(str);	
	}
}

void CGlassDetectView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if(lHint<2 || pDoc->m_OpenMode==1) return;

	/*		
	//if(lHint<4) 
	if(lHint<4 && pDoc->m_Result[0].pROIBuffer != NULL)
	{
		UpdateResult();
		UpateMemDC();
	}	
	// Bitmap;
	CRect	cr;
	GetClientRect(&cr);
	cr.bottom = cr.Height() - m_Dlg_H;
	InvalidateRect(cr, TRUE);		
	*/

	if(lHint<4)
	{
		UpdateResult();
	}
}


void CGlassDetectView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);
	return;

	CString s;

	// offset
	float fx = (float) point.x - m_bmp_ofs_x;
	float fy = (float) point.y - m_bmp_ofs_y;

	// zoom
	fx /= m_zoomRate;
	fy /= m_zoomRate;

	// possible internal additonal zoom!
	unsigned short	ZMode	= DDAQ_ZMODE_DIRECT;
	float			Zoom	= 1.0f;
	theApp.DDAQ_IRDX_IMAGE_GetZoom(pDoc->m_hIRDX_Doc, &ZMode, &Zoom);
	if (ZMode > DDAQ_ZMODE_DIRECT)
	{
		fx /= Zoom;
		fy /= Zoom;
	}

	// in image??

	/*
	if ((fx >= 0.0f) && (fy >= 0.0f))
	{
		// pixel coordinate 1..sizex/y
		unsigned short ux = (unsigned short) fx;
		unsigned short uy = (unsigned short) fy;

		// image size
		

		// in image??
		if ((ux < m_data_X) && (uy < m_data_Y))
		{
			// get data
			float DataPoint = pDoc->m_pSpotAreaBuffer[m_data_X*uy + ux];			
		
			s.Format(IDS_FMT_PIXEL_POINT_Cursor, ux+253, uy+189, DataPoint);
		}
	}

	CMainFrame* pMWnd = (CMainFrame*) AfxGetMainWnd();
	pMWnd->SetStatusText(s);
	*/
	
	//CView::OnMouseMove(nFlags, point);
}


BOOL CGlassDetectView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CView::OnSetCursor(pWnd, nHitTest, message);
}


void CGlassDetectView::InitROIData()
{
	unsigned short SizeX = 0;
	unsigned short SizeY = 0;
	
	theApp.DDAQ_IRDX_PIXEL_GetSize(pDoc->m_hIRDX_Doc, &SizeX, &SizeY);
	
	pDoc->m_PixelX = SizeX;
	pDoc->m_PixelY = SizeY;

	for(int i=0; i<8; i++)
	{
		m_ResultDlg.m_ROI_X[i] = pDoc->m_Result[i].left;
		m_ResultDlg.m_ROI_Y[i] = pDoc->m_Result[i].top;
		m_ResultDlg.m_ROI_W[i] = pDoc->m_Result[i].right - pDoc->m_Result[i].left + 1;
		m_ResultDlg.m_ROI_H[i] = pDoc->m_Result[i].bottom - pDoc->m_Result[i].top + 1;
	
		/*
		DDV_MinMaxUInt(pDX, m_ROI_X[i], 0, pDoc->m_PixelX-1);
		DDV_MinMaxUInt(pDX, m_ROI_Y[i], 0, pDoc->m_PixelY-1);
		DDV_MinMaxUInt(pDX, m_ROI_W[i], 1, pDoc->m_PixelX);
		DDV_MinMaxUInt(pDX, m_ROI_H[i], 1, pDoc->m_PixelY);
		*/	

		m_ResultDlg.m_Spin_X[i].SetRange(0, SizeX - pDoc->m_Result[i].right + pDoc->m_Result[i].left - 1);
		//m_ResultDlg.m_Spin_Y[i].SetRange(0, SizeY - pDoc->m_Result[i].bottom + pDoc->m_Result[i].top - 1);
		m_ResultDlg.m_Spin_Y[i].SetRange(SizeY - pDoc->m_Result[i].bottom + pDoc->m_Result[i].top - 1, 0);
		m_ResultDlg.m_Spin_W[i].SetRange(1, SizeX - pDoc->m_Result[i].left);
		//m_ResultDlg.m_Spin_H[i].SetRange(1, SizeY - pDoc->m_Result[i].top);
		m_ResultDlg.m_Spin_H[i].SetRange(SizeY - pDoc->m_Result[i].top, 1);
	}
	
	m_ResultDlg.UpdateData(FALSE);
}