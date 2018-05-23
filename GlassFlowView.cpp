
// GlassFlowView.cpp : CGlassFlowView 클래스의 구현

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "PyroSoftM.h"
#endif

#include "MainFrm.h"
#include "PyroSoftMDoc.h"
#include "GlassFlowView.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define P_WIDTH				1
#define NON_SELECT_POI		-1
#define MAXMIN_POINT		-2

#define BASE_ROI_LEFT		1
#define BASE_ROI_RIGHT		2
#define BASE_ROI_TOP		3
#define BASE_ROI_BOTTOM		4

CCriticalSection irdx_cs;

// CGlassFlowView

IMPLEMENT_DYNCREATE(CGlassFlowView, CView)

BEGIN_MESSAGE_MAP(CGlassFlowView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGlassFlowView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_DATAPLAYER_PREVIOUSRECORD, &CGlassFlowView::OnDataplayerPreviousrecord)
	ON_UPDATE_COMMAND_UI(ID_DATAPLAYER_PREVIOUSRECORD, &CGlassFlowView::OnUpdateDataplayerPreviousrecord)
	ON_COMMAND(ID_DATAPLAYER_NEXTRECORD, &CGlassFlowView::OnDataplayerNextrecord)
	ON_UPDATE_COMMAND_UI(ID_DATAPLAYER_NEXTRECORD, &CGlassFlowView::OnUpdateDataplayerNextrecord)
	ON_COMMAND(ID_DATAPLAYER_PLAY, &CGlassFlowView::OnDataplayerPlay)
	ON_UPDATE_COMMAND_UI(ID_DATAPLAYER_PLAY, &CGlassFlowView::OnUpdateDataplayerPlay)
	ON_COMMAND(ID_DATAPLAYER_STOP, &CGlassFlowView::OnDataplayerStop)
	ON_UPDATE_COMMAND_UI(ID_DATAPLAYER_STOP, &CGlassFlowView::OnUpdateDataplayerStop)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_ROI_DRAW, &CGlassFlowView::OnPOIDraw)
	ON_UPDATE_COMMAND_UI(ID_ROI_DRAW, &CGlassFlowView::OnUpdatePOIDraw)
	ON_COMMAND(ID_REF_DRAW, &CGlassFlowView::OnPOIDelete)
	ON_UPDATE_COMMAND_UI(ID_REF_DRAW, &CGlassFlowView::OnUpdatePOIDelete)
	ON_COMMAND(ID_ROI_REF_MOVE, &CGlassFlowView::OnRoiRefMove)
	ON_UPDATE_COMMAND_UI(ID_ROI_REF_MOVE, &CGlassFlowView::OnUpdateRoiRefMove)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CGlassFlowView 생성/소멸
CGlassFlowView::CGlassFlowView()
{
	m_TimerID = 0;

	m_bmp_isize_x = 0;
	m_bmp_isize_y = 0;
	m_bmp_size_x = 0;
	m_bmp_size_y = 0;
	m_bmp_ofs_x = 0;
	m_bmp_ofs_y = 0;
	m_bmp_zoom = 0.0f;

	m_StretchSizeX = 0;
	m_StretchSizeY = 0;
	m_bforeStretchSizeX = 0;
	m_bforeStretchSizeY = 0;

	m_posX = 0;
	m_posY = 0;

	m_CurrentRatio = 0.0f;

	m_isPoint_clicked = false;
	m_ViewMoveFlag = false;

	//-----새로작성한코드---------
	m_XY = CPoint(0, 0);
	TmperInf = "";
	DataPoint = 0.0f;
	ux = 0;
	uy = 0;
	
	m_POI_Mode = MOVE_MODE;		// 1. ROI 그리기   2. REF 그리기   3. 이동 모드(ROI, REF)   4. 회전 모드 (REF)  5. ROI 지우기   6. REF 지우기
	//----------------------------
}

CGlassFlowView::~CGlassFlowView()
{
}

template<class T>
void SortCArray(T& t, int(__cdecl *compare)(const void *elem1, const void *elem2)) {
	if (t.GetSize() <= 0) return;
	qsort(t.GetData(), t.GetSize(), sizeof(t[0]), compare);
}

// int 자료형비교함수
int CompareInt(const void *elem1, const void *elem2) {
	int* p1 = (int*)elem1;
	int* p2 = (int*)elem2;

	if (*p1 > *p2) return +1;
	if (*p1 < *p2) return -1;

	return 0;
}

BOOL CGlassFlowView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

void CGlassFlowView::OnDestroy()
{
	if (pDoc->m_nDDAQDeviceNo)
	{
		theApp.DDAQ_DEVICE_DO_Close(pDoc->m_nDDAQDeviceNo);
	}
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

// CGlassFlowView 인쇄

void CGlassFlowView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGlassFlowView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CGlassFlowView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CGlassFlowView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CGlassFlowView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGlassFlowView 진단

#ifdef _DEBUG
void CGlassFlowView::AssertValid() const
{
	CView::AssertValid();
}

void CGlassFlowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPyroSoftMDoc* CGlassFlowView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPyroSoftMDoc)));
	return (CPyroSoftMDoc*)m_pDocument;
}
#endif //_DEBUG


// CGlassFlowView 메시지 처리기

void CGlassFlowView::OnDraw(CDC* pDC)
{
	CPyroSoftMDoc* pDoc = GetDocument();

	if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
		return;

	// CompatibleDC와 CompatibleBitmap을 사용하기 위한 클래스 선언. 
	CDC* pMemDC;
	CBitmap* pBmp;
	CBitmap* pOld;

	// CompatibleDC와 CompatibleBitmap을 생성시키기 위해서 메모리를
	// 확보한다.
	pMemDC = new CDC;
	pBmp = new CBitmap;

	// pointer to bitmap
	void*		pBits;
	BITMAPINFO* pBitmapInfo;

	// client rectangle of window
	CRect	cr;
	GetClientRect(&cr);

	// window size
	int		wnd_sizex = cr.Width();
	int		wnd_sizey = cr.Height();

	// get bitmap from DDAQ
	if (!theApp.DDAQ_IRDX_IMAGE_GetBitmap(pDoc->m_hIRDX_Doc, &m_bmp_isize_x, &m_bmp_isize_y, &pBits, &pBitmapInfo))
		return;

	if (pDoc->m_ZoomMode == 7 && (pDoc->m_BaseROI.rx == 0 || pDoc->m_BaseROI.by == 0))
	{
		AfxMessageBox("Base ROI가 존재하지 않습니다.\nCtrl + 드래그 하여 Base ROI를 만든 후 적용 가능합니다.");
		pDoc->m_ZoomMode = 0;
	}
	else if(pDoc->m_ZoomMode == 7)
	{
		m_bmp_isize_x = pDoc->m_BaseROI.rx - pDoc->m_BaseROI.lx;
		m_bmp_isize_y = pDoc->m_BaseROI.by - pDoc->m_BaseROI.ty;
	}

	// calculate additional stretch zoom
	float zoomx = (float)wnd_sizex / (float)m_bmp_isize_x;
	float zoomy = (float)wnd_sizey / (float)m_bmp_isize_y;

	m_bmp_zoom = __min(zoomx, zoomy);

	// destination size
	m_bmp_size_x = (int)(m_bmp_zoom*m_bmp_isize_x);		//실제 뿌려 줄 화면
	m_bmp_size_y = (int)(m_bmp_zoom*m_bmp_isize_y);

	if (m_bmp_zoom < 0.1f)
		m_bmp_zoom = 0.1f;

	// center bitmap to the window
	m_bmp_ofs_x = (int)((wnd_sizex - m_bmp_size_x) / 2.0);
	m_bmp_ofs_y = (int)((wnd_sizey - m_bmp_size_y) / 2.0);

	int local_oldStretchSizeX = 0, local_oldStretchSizeY = 0;
	local_oldStretchSizeX = m_StretchSizeX;
	local_oldStretchSizeY = m_StretchSizeY;

	if (pDoc->m_ZoomMode == 0)   // AutoSize Mode
	{
		m_posX = 0; m_posY = 0;

		m_StretchSizeX = m_bmp_size_x;
		m_StretchSizeY = m_bmp_size_y;
	}
	else
	{
		m_StretchSizeX = m_bmp_size_x * pDoc->m_ZoomRatio;
		m_StretchSizeY = m_bmp_size_y * pDoc->m_ZoomRatio;
	}

	if (m_CurrentRatio != pDoc->m_ZoomRatio)
	{
		m_posX = (m_StretchSizeX - local_oldStretchSizeX) / 2 + m_posX;
		m_posY = (m_StretchSizeY - local_oldStretchSizeY) / 2 + m_posY;

		if (m_posX < 0) m_posX = 0;
		if (m_posY < 0) m_posY = 0;
		if (m_posX >(m_StretchSizeX - m_bmp_size_x))   m_posX = (m_StretchSizeX - m_bmp_size_x);
		if (m_posY >(m_StretchSizeY - m_bmp_size_y))   m_posY = (m_StretchSizeY - m_bmp_size_y);

		m_CurrentRatio = pDoc->m_ZoomRatio;
	}

	if (pDoc->m_ZoomMode != 0)
		m_bmp_zoom *= pDoc->m_ZoomRatio;

	//ux, uy 구하기 위한 포인트 값을 매개변수로 던짐
	CalculatePoint(m_XY);

	// CompatibleDC와 CompatibleBitmap을 생성
	// CompatibleBitmap의 크기는 m_bmp_size_x*m_bmp_size_y으로 설정
	pMemDC->CreateCompatibleDC(pDC);
	pBmp->CreateCompatibleBitmap(pDC, m_bmp_size_x, m_bmp_size_y);

	// CompatibleBitmap을 CompatibleDC에 등록
	pOld = pMemDC->SelectObject(pBmp);

	// SetStretchMode
	::SetStretchBltMode(pMemDC->m_hDC, HALFTONE);

	// stretch bitmap to screen
	::StretchDIBits(pMemDC->m_hDC, -m_posX, -m_posY, m_StretchSizeX, m_StretchSizeY,				// 뿌려줄 화면
				0, 0, m_bmp_isize_x, m_bmp_isize_y, 							// 카메라 이미지 화면 (512 * 384)
					pBits, pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	CFont font;
	font.CreatePointFont(100 + ((pDoc->m_Font_Size - 1) * 10), "Tahoma");	//폰트의 크기와 글꼴설정

	// ROI 텍스트 설정
	if (pDoc->m_ROIShow)	
	{
		CString str_tmp;
		COLORREF color = RGB(255, 0, 0);		// 빨간색
		CPen pen(PS_SOLID, 2, color);

		pMemDC->SelectObject(&font);				//폰트 설정
		CPen* pOldPen = pMemDC->SelectObject(&pen);	//펜 설정
		pMemDC->SelectStockObject(NULL_BRUSH);		//브러시 설정
		pMemDC->SetROP2(R2_COPYPEN);
		//pMemDC->SetBkMode(TRANSPARENT);			//텍스트의 배경을 투명하게 설정
		pMemDC->SetTextColor(color);				//텍스트 칼라 설정

		for (int i = 0; i < pDoc->m_ROICount; i++) {
			if (pDoc->max_x[i] == 0)
				continue;
			str_tmp.Format("#%d", i + 1);
			pMemDC->TextOutA(pDoc->min_x[i] * m_bmp_zoom - m_posX, pDoc->min_y[i] * m_bmp_zoom - m_posY, str_tmp);
		}

		pMemDC->SelectObject(pOldPen);
	}

	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pMemDC->SelectObject(&pen);
	CBrush brush;
	brush.CreateStockObject(NULL_BRUSH);
	CBrush* pOldBrush = pMemDC->SelectObject(&brush);

	// Base ROI 영역
	if (!m_isBaseRectDone && (pDoc->m_BaseROI.rx != 0))		// 처음 켤 시 Base ROI의 값이 존재 할 시
	{
		m_isBaseRectDone = true;

		pMemDC->Rectangle(pDoc->m_BaseROI.lx * m_bmp_zoom, pDoc->m_BaseROI.ty * m_bmp_zoom, pDoc->m_BaseROI.rx * m_bmp_zoom, pDoc->m_BaseROI.by * m_bmp_zoom);
	}
	else if (!m_isBaseRectDone && (pDoc->m_BaseROI.lx != 0))			// 그려지고 있을 때
	{
		pMemDC->Rectangle(
			pDoc->m_BaseROI.lx * m_bmp_zoom - m_posX, pDoc->m_BaseROI.ty * m_bmp_zoom - m_posY,
			(ux - 1) * m_bmp_zoom - m_posX, (uy - 1) * m_bmp_zoom - m_posY);

		InvalidateRect(&m_OBaseRect, 0);
		SelectRect(&m_CBaseRect, pDoc->m_BaseROI, ux - 1, uy - 1);
		m_OBaseRect = m_CBaseRect;
		InvalidateRect(&m_CBaseRect, 0);
	}
	else if (m_isBaseRectDone && (pDoc->m_BaseROI.lx != 0))		// 사각형 그리기 완료 했을 때
	{
		pMemDC->Rectangle(
			pDoc->m_BaseROI.lx * m_bmp_zoom - m_posX, pDoc->m_BaseROI.ty * m_bmp_zoom - m_posY, 
			pDoc->m_BaseROI.rx * m_bmp_zoom - m_posX, pDoc->m_BaseROI.by * m_bmp_zoom - m_posY);
	}

	pMemDC->SelectObject(pOldPen);
	pMemDC->SelectObject(pOldBrush);

	if (1)	// ROI 영역
		RectROIDraw(pMemDC, 1, pDoc->min_x, pDoc->min_y, pDoc->max_x, pDoc->max_y, pDoc->m_ROICount, RGB(255, 255, 255));

	if (pDoc->m_minPointerShow)	// 최소 온도 값
		CrossROIDraw(pMemDC, pDoc->m_ResultData.TMinX, pDoc->m_ResultData.TMinY, pDoc->m_ResultData.TMin, pDoc->m_ROICount, MAXMIN_POINT, RGB(255, 255, 255), RGB(0, 0, 255), RGB(0, 0, 0));

	if (pDoc->m_maxPointerShow)	// 최고 온도 값
		CrossROIDraw(pMemDC, pDoc->m_ResultData.TMaxX, pDoc->m_ResultData.TMaxY, pDoc->m_ResultData.TMax, pDoc->m_ROICount, MAXMIN_POINT, RGB(255, 255, 255), RGB(255, 0, 0), RGB(0, 0, 0));

	// POI 설정
	if (pDoc->m_POI_count > 0)
	{
		int tempX[MAX_POI_NUM] = { 0 }, tempY[MAX_POI_NUM] = { 0 };
		float tempT[MAX_POI_NUM] = { 0 }, tempData;

		for (int i = 0; i < pDoc->m_POI_count; i++)
		{
			tempX[i] = pDoc->m_POIArr[i].x;
			tempY[i] = pDoc->m_POIArr[i].y;
			theApp.DDAQ_IRDX_PIXEL_GetDataPoint(pDoc->m_hIRDX_Doc, pDoc->m_POIArr[i].x, pDoc->m_POIArr[i].y, &tempData);
			if (tempData > 600.0f)
				tempT[i] = tempData;
			else
				tempT[i] = 600.0f;
			pDoc->POI_TemperatureArray[i] = tempT[i];
		}
		if (!m_isPoint_clicked)
			CrossROIDraw(pMemDC, tempX, tempY, tempT, pDoc->m_POI_count, NON_SELECT_POI, RGB(255, 255, 255), RGB(0, 220, 100), RGB(0, 0, 0));
		else
			CrossROIDraw(pMemDC, tempX, tempY, tempT, pDoc->m_POI_count, m_point_idx, RGB(255, 255, 255), RGB(0, 220, 100), RGB(0, 0, 0));
	}

	// tchart의 라인 그리기??
	if (pDoc->m_SelXPixelChart[0] != 0)
	{
		CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* pOldPen = pMemDC->SelectObject(&pen);

		for (int i = 0; i < pDoc->m_SelXPxlChrt_idx; i++)
		{
			pMemDC->MoveTo(pDoc->m_SelXPixelChart[i] * m_bmp_zoom, 0);
			pMemDC->LineTo(pDoc->m_SelXPixelChart[i] * m_bmp_zoom, m_bmp_isize_y * m_bmp_zoom);
		}

		pMemDC->SelectObject(pOldPen);
	}
	if (pDoc->m_SelYPixelChart[0] != 0)
	{
		CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* pOldPen = pMemDC->SelectObject(&pen);

		for (int i = 0; i < pDoc->m_SelYPxlChrt_idx; i++)
		{
			pMemDC->MoveTo(0, pDoc->m_SelYPixelChart[i] * m_bmp_zoom);
			pMemDC->LineTo(m_bmp_isize_x * m_bmp_zoom, pDoc->m_SelYPixelChart[i] * m_bmp_zoom);
		}

		pMemDC->SelectObject(pOldPen);
	}


	//pMemDC 안에 들어왔을 때 좌표값과 온도값 표시
	if (m_XY.x > m_bmp_ofs_x && m_XY.y > m_bmp_ofs_y && (m_XY.x < (m_bmp_ofs_x + m_bmp_size_x)) && (m_XY.y < (m_bmp_ofs_y + m_bmp_size_y)) && pDoc->m_cursorShow)
	{
		CFont font;
		font.CreatePointFont(100 + ((pDoc->m_Font_Size - 1) * 10), "Tahoma");	//사각형 밑 폰트의 크기와 글꼴설정
		pMemDC->SelectObject(&font);				//폰트 설정
		pMemDC->SetTextColor(RGB(0, 220, 100));

		if (ux <= 455 && uy >= 17)
			pMemDC->TextOutA(m_XY.x - m_bmp_ofs_x + 5, m_XY.y - m_bmp_ofs_y - 20, TmperInf);
		else if (ux > 455 && uy >= 17)
			pMemDC->TextOutA(m_XY.x - m_bmp_ofs_x - 50, m_XY.y - m_bmp_ofs_y - 20, TmperInf);
		else if (ux > 455 && uy < 17)
			pMemDC->TextOutA(m_XY.x - m_bmp_ofs_x - 50, m_XY.y - m_bmp_ofs_y + 20, TmperInf);
		else
			pMemDC->TextOutA(m_XY.x - m_bmp_ofs_x + 5, m_XY.y - m_bmp_ofs_y + 20, TmperInf);
	}

	// 커서 설정
	if (pDoc->m_BaseROI.ty - 2 < uy - 1 && pDoc->m_BaseROI.by + 2 > uy - 1 && pDoc->m_BaseROI.lx - 2 < ux - 1 && pDoc->m_BaseROI.lx + 2 > ux - 1 ||
		pDoc->m_BaseROI.ty - 2 < uy - 1 && pDoc->m_BaseROI.by + 2 > uy - 1 && pDoc->m_BaseROI.rx - 2 < ux - 1 && pDoc->m_BaseROI.rx + 2 > ux - 1)
	{
		SetCursor(LoadCursor(0, IDC_SIZEWE));
	}
	else if (pDoc->m_BaseROI.lx - 2 < ux - 1 && pDoc->m_BaseROI.rx + 2 > ux - 1 && pDoc->m_BaseROI.ty - 2 < uy - 1 && pDoc->m_BaseROI.ty + 2 > uy - 1 ||
			pDoc->m_BaseROI.lx - 2 < ux - 1 && pDoc->m_BaseROI.rx + 2 > ux - 1 && pDoc->m_BaseROI.by - 2 < uy - 1 && pDoc->m_BaseROI.by + 2 > uy - 1)
	{
		SetCursor(LoadCursor(0, IDC_SIZENS));
	}
	else if (m_POI_Mode != DRAW_POI && pDoc->m_POI_count > 0)	// POI 선택 시
	{
		if (m_PointMove_flag)
			SetCursor(LoadCursor(0, IDC_SIZEALL));
		else
			SetCursor(LoadCursor(0, IDC_ARROW));
	}
	else if (m_isInsideBaseROI)								// Base ROI 선택 시
		SetCursor(LoadCursor(0, IDC_SIZEALL));
	else if (m_XY.x < wnd_sizex - 5 && m_XY.y < wnd_sizey - 5)
		SetCursor(LoadCursor(0, IDC_ARROW));


	// CompatibleDC에 그린 그림을 실제의 화면으로 복사한다.
	pDC->BitBlt(m_bmp_ofs_x, m_bmp_ofs_y, m_bmp_size_x, m_bmp_size_y, pMemDC, 0, 0, SRCCOPY);

	// 이전 DC로 회복시킨다.
	pMemDC->SelectObject(pOld);

	// 확보한 메모리를 해제한다.
	delete pMemDC;
	delete pBmp;
}

void CGlassFlowView::SelectRect(RECT* CBaseRect, BASE_ROI m_BaseROI, unsigned short ux, unsigned short uy)
{
	if (m_BaseROI.lx < ux && m_BaseROI.ty > uy)
	{
		SetInvaliRect(CBaseRect, m_BaseROI, QUADRANT1, 3, 50, 10, 30);
	}
	else if (m_BaseROI.lx < ux && m_BaseROI.ty < uy)
	{
		SetInvaliRect(CBaseRect, m_BaseROI, QUADRANT2, 3, 3, 10, 30);
	}
	else if (m_BaseROI.lx > ux && m_BaseROI.ty < uy)
	{
		SetInvaliRect(CBaseRect, m_BaseROI, QUADRANT3, 3, 3, 10, 30);
	}
	else if (m_BaseROI.lx > ux && m_BaseROI.ty > uy)
	{
		SetInvaliRect(CBaseRect, m_BaseROI, QUADRANT4, 3, 50, 10, 30);
	}
}

void CGlassFlowView::SetInvaliRect(RECT* CBaseRect, BASE_ROI m_BaseROI, int quadrant, int sx, int sy, int ex, int ey)
{
	CString ss;

	switch (quadrant)
	{	//ux uy고려
	case QUADRANT1:
		CBaseRect->left = m_BaseROI.lx * m_bmp_zoom - sx;
		CBaseRect->top = m_XY.y - ey;
		CBaseRect->right = m_XY.x + ex;
		CBaseRect->bottom = m_BaseROI.ty * m_bmp_zoom + sy;
		break;
	case QUADRANT2:
		CBaseRect->left = m_BaseROI.lx * m_bmp_zoom - sx;
		CBaseRect->top = m_BaseROI.ty * m_bmp_zoom - sy;
		CBaseRect->right = m_XY.x + ex;
		CBaseRect->bottom = m_XY.y + ey;
		break;
	case QUADRANT3:
		CBaseRect->left = m_XY.x - ex;
		CBaseRect->top = m_BaseROI.lx * m_bmp_zoom - sy;
		CBaseRect->right = m_BaseROI.ty * m_bmp_zoom + sx;
		CBaseRect->bottom = m_XY.y + ey;
		break;
	case QUADRANT4:
		CBaseRect->left = m_XY.x - ex;
		CBaseRect->top = m_XY.y - ey;
		CBaseRect->right = m_BaseROI.lx * m_bmp_zoom + sx;
		CBaseRect->bottom = m_BaseROI.ty * m_bmp_zoom + sy;
		break;
	case ELLIPSESTART:
		CBaseRect->left = (ClickedDistance.x - 2) * m_bmp_zoom - sx;
		CBaseRect->top = (ClickedDistance.y - 2) * m_bmp_zoom - sy;
		CBaseRect->right = (ClickedDistance.x + 2) * m_bmp_zoom + ex;
		CBaseRect->bottom = (ClickedDistance.y + 2) * m_bmp_zoom + ey;
		break;
	}
}

void CGlassFlowView::RectROIDraw(CDC* pMemDC, int pWidht, int min_x[], int min_y[], int max_x[], int max_y[], int loop_count, COLORREF rect_color)
{
	CPen pen(PS_SOLID, pWidht, rect_color);
	CPen* pOldPen = pMemDC->SelectObject(&pen);
	pMemDC->SelectStockObject(NULL_BRUSH);
	pMemDC->SetROP2(R2_COPYPEN);

	for (int i = 0; i < loop_count; i++)
		pMemDC->Rectangle(min_x[i] * m_bmp_zoom - m_posX, min_y[i] * m_bmp_zoom + 1 - m_posY, max_x[i] * m_bmp_zoom - m_posX, max_y[i] * m_bmp_zoom + 1 - m_posY);

	pMemDC->SelectObject(pOldPen);
}

void CGlassFlowView::DotROIDraw(CDC* pMemDC, int tempX[], int tempY[], float temp[], int loop_count, int select_idx, COLORREF back_color, COLORREF front_color, COLORREF select_color)
{
	int pWidth = 1, fShort = 1, fLong = 4, xGap = 3, yGap = 5;

	CPen back_pen(PS_SOLID, pWidth, back_color);		// 배경 흰색
	CPen pen(PS_SOLID, pWidth, front_color);			// 기본 표현 색
	CPen select_pen(PS_SOLID, pWidth, select_color);	// 온도점 선택 시 표현 색
	CPen* pOldPen = pMemDC->SelectObject(&back_pen);
	CString strt;

	pMemDC->SelectStockObject(WHITE_BRUSH);
	pMemDC->SetROP2(R2_COPYPEN);
	pMemDC->SetTextColor(front_color);
	
	for (int i = 0; i < loop_count; i++)
	{
		if ((tempX[i] == 0 && tempY[i] == 0) || (pDoc->max_x[i] == 0 && pDoc->max_y[i] == 0))
			continue;

		pMemDC->Rectangle(tempX[i] * m_bmp_zoom, tempY[i] * m_bmp_zoom,
			(tempX[i] + 1) * m_bmp_zoom, (tempY[i] + 1) * m_bmp_zoom);
		if (1)
		{
			strt.Format("%.1f℃", temp[i]);
			pMemDC->TextOutA(tempX[i] * m_bmp_zoom + pDoc->m_Font_Size + 3, tempY[i] * m_bmp_zoom + 5, strt);
		}
	}
	pMemDC->SelectObject(pOldPen);
}

void CGlassFlowView::CrossROIDraw(CDC* pMemDC, int tempX[], int tempY[], float temp[], int loop_count, int select_idx, COLORREF back_color, COLORREF front_color, COLORREF select_color)
{
	if (tempX[0] <= 0 || tempY[0] <= 0) return;

	int pWidth = 1, fShort = 1, fLong = 4, xGap = 3, yGap = 5;

	CFont font;
	font.CreatePointFont(100 + ((pDoc->m_Font_Size - 1) * 10), "Tahoma");	//폰트의 크기와 글꼴설정

	CPen back_pen(PS_SOLID, pWidth, back_color);		// 배경 흰색
	CPen pen(PS_SOLID, pWidth, front_color);			// 기본 표현 색
	CPen select_pen(PS_SOLID, pWidth, select_color);	// 온도점 선택 시 표현 색
	CPen* pOldPen = pMemDC->SelectObject(&back_pen);
	CString strt;

	pMemDC->SelectObject(&font);
	pMemDC->SelectStockObject(WHITE_BRUSH);
	pMemDC->SetROP2(R2_COPYPEN);
	pMemDC->SetTextColor(front_color);

	for (int i = 0; i < loop_count; i++)
	{
		if (tempX[i] == 0 && tempY[i] == 0)
			continue;

		pMemDC->Rectangle(tempX[i] * m_bmp_zoom - (fShort + 1) - m_posX, tempY[i] * m_bmp_zoom - (fLong + 1) - m_posY,
			(tempX[i]) * m_bmp_zoom + (fShort + 1) - m_posX, (tempY[i]) * m_bmp_zoom + (fLong + 1) - m_posY);
		pMemDC->Rectangle(tempX[i] * m_bmp_zoom - (fLong + 1) - m_posX, tempY[i] * m_bmp_zoom - (fShort + 1) - m_posY,
			(tempX[i]) * m_bmp_zoom + (fLong + 1) - m_posX, (tempY[i]) * m_bmp_zoom + (fShort + 1) - m_posY);
	}

	pMemDC->SelectObject(&pen);
	pMemDC->SetROP2(R2_COPYPEN);
	for (int i = 0; i < loop_count; i++)
	{
		if (tempX[i] == 0 && tempY[i] == 0)
			continue;

		if (select_idx != -1 && i == select_idx)
		{
			pMemDC->SelectObject(&select_pen);
			pMemDC->SetTextColor(select_color);
		}
		else
		{
			pMemDC->SelectObject(&pen);
			pMemDC->SetTextColor(front_color);
		}

		pMemDC->Rectangle(tempX[i] * m_bmp_zoom - fShort - m_posX, tempY[i] * m_bmp_zoom - fLong - m_posY,
			(tempX[i]) * m_bmp_zoom + fShort - m_posX, (tempY[i]) * m_bmp_zoom + fLong - m_posY);
		pMemDC->Rectangle(tempX[i] * m_bmp_zoom - fLong - m_posX, tempY[i] * m_bmp_zoom - fShort - m_posY,
			(tempX[i]) * m_bmp_zoom + fLong - m_posX, (tempY[i]) * m_bmp_zoom + fShort - m_posY);

		if (select_idx == MAXMIN_POINT)
		{

		}
		else if (pDoc->m_POIShowNum == TEMPER_ONLY && loop_count != 0)		// 온도값만 표시
		{
			strt.Format("%.1f°C", temp[i]);
			pMemDC->TextOutA(tempX[i] * m_bmp_zoom  + xGap - m_posX, tempY[i] * m_bmp_zoom  + yGap - m_posY, strt);
		}
		else if (pDoc->m_POIShowNum == NUMBER_ONLY && loop_count != 0)		// POI #만 표시
		{
			strt.Format("#%d", i + 1);
			pMemDC->TextOutA(tempX[i] * m_bmp_zoom  + xGap - m_posX, tempY[i] * m_bmp_zoom  + yGap - m_posY, strt);
		}
		else if (pDoc->m_POIShowNum == SHOW_ALL && loop_count != 0)		// 둘다 표시
		{
			strt.Format("%d: %.1f°C", pDoc->RunningIndex[i] + 1, temp[i]);
			pMemDC->TextOutA(tempX[i] * m_bmp_zoom  + xGap - m_posX, tempY[i] * m_bmp_zoom  + yGap - m_posY, strt);
		}
	}

	pMemDC->SelectStockObject(WHITE_BRUSH);
	pMemDC->SelectObject(pOldPen);
}

/////////////////////////////////////////////////////////////////////////////
// CDDAQ_OnlineView message handlers

void CGlassFlowView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	///mdb
	//CPyroSoftMDoc* pDoc = GetDocument();

	if (lHint>2) return;

	CRect	cr;
	GetClientRect(&cr);

	if (cr.Width() == 0 || cr.Height() == 0) return;

	//if(lHint>2 || pDoc->m_OpenMode>1) return;

	// return if document is not ready now
	if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
		return;

	unsigned short new_size_x = 0;
	unsigned short new_size_y = 0;

	// get new bitmap size from DDAQ
	theApp.DDAQ_IRDX_IMAGE_GetSize(pDoc->m_hIRDX_Doc, &new_size_x, &new_size_y);

	// erase background?
	if ((lHint == 0) || (m_bmp_isize_x != new_size_x) || (m_bmp_isize_y != new_size_y))
		Invalidate(1);
	else
		Invalidate(0);

	// save new size
	m_bmp_isize_x = new_size_x;
	m_bmp_isize_y = new_size_y;
}

/////////////////////////////////////////////////////////////////////////////
void CGlassFlowView::OnMouseMove(UINT nFlags, CPoint point)
{
	CString s;

	// offset
	float fx = (float)point.x - m_bmp_ofs_x + m_posX;
	float fy = (float)point.y - m_bmp_ofs_y + m_posY;

	// zoom
	fx /= m_bmp_zoom;
	fy /= m_bmp_zoom;

	// possible internal additonal zoom!
	unsigned short	ZMode = DDAQ_ZMODE_DIRECT;
	float			Zoom = 1.0f;
	theApp.DDAQ_IRDX_IMAGE_GetZoom(pDoc->m_hIRDX_Doc, &ZMode, &Zoom);
	if (ZMode > DDAQ_ZMODE_DIRECT)
	{
		fx /= Zoom;
		fy /= Zoom;
	}

	// in image??
	if ((fx >= 0.0f) && (fy >= 0.0f))
	{
		// pixel coordinate 1..sizex/y
		unsigned short ux = (unsigned short)fx + 1;
		unsigned short uy = (unsigned short)fy + 1;

		// image size
		unsigned short sizex, sizey;
		theApp.DDAQ_IRDX_PIXEL_GetSize(pDoc->m_hIRDX_Doc, &sizex, &sizey);

		// in image??
		if ((ux <= sizex) && (uy <= sizey))
		{
			// get data
			theApp.DDAQ_IRDX_PIXEL_GetDataPoint(pDoc->m_hIRDX_Doc, ux, uy, &DataPoint);

			if (DataPoint < pDoc->m_MinTemp)
			{
				TmperInf.Format("%.1f°C ", pDoc->m_MinTemp);
			}
			else {
				TmperInf.Format("%.1f°C ", DataPoint);
				//하단 상태바에 나타내주는 텍스트 형식 
				s.Format(IDS_FMT_PIXEL_POINT_Cursor, ux - 1, uy - 1, DataPoint);
			}

			if (m_bROI_LLine_flag)
			{
				MoveROILine(BASE_ROI_LEFT);
			}
			else if (m_bROI_RLine_flag)
			{
				MoveROILine(BASE_ROI_RIGHT);
			}
			else if (m_bROI_TLine_flag)
			{
				MoveROILine(BASE_ROI_TOP);
			}
			else if (m_bROI_BLine_flag)
			{
				MoveROILine(BASE_ROI_BOTTOM);
			}
			else if (m_PointMove_flag)				// POI 이동
			{
				int tempX, tempY;

				tempX = pDoc->m_POIArr[m_point_idx].x - (ClickedDistance.x - ux);
				tempY = pDoc->m_POIArr[m_point_idx].y - (ClickedDistance.y - uy);

				if (tempX > MIN_X_PIXEL && tempX <= MAX_X_PIXEL
					&& tempY > MIN_Y_PIXEL && tempY <= MAX_Y_PIXEL)
				{
					pDoc->m_POIArr[m_point_idx].x -= (ClickedDistance.x - ux);
					pDoc->m_POIArr[m_point_idx].y -= (ClickedDistance.y - uy);
				}
				
				ClickedDistance.x = ux;
				ClickedDistance.y = uy;
			}
			else if (m_isInsideBaseROI)			// Base ROI 이동
			{
				int tempLX, tempRX, tempTY, tempBY;

				tempLX = pDoc->m_BaseROI.lx - (ClickedDistance.x - ux);
				tempRX = pDoc->m_BaseROI.rx - (ClickedDistance.x - ux);
				tempTY = pDoc->m_BaseROI.ty - (ClickedDistance.y - uy);
				tempBY = pDoc->m_BaseROI.by - (ClickedDistance.y - uy);

				if (tempLX <= 0 || tempRX > pDoc->sizeX || tempTY <= 0 || tempBY > pDoc->sizeY)
				{
					m_isInsideBaseROI = false;
					MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
				}
				else
				{
					pDoc->m_BaseROI.lx -= (ClickedDistance.x - ux);
					pDoc->m_BaseROI.rx -= (ClickedDistance.x - ux);
					pDoc->m_BaseROI.ty -= (ClickedDistance.y - uy);
					pDoc->m_BaseROI.by -= (ClickedDistance.y - uy);

					ClickedDistance.x = ux;
					ClickedDistance.y = uy;
				}

				this->Invalidate(FALSE);
			}
			else if (m_ViewMoveFlag && pDoc->m_ZoomRatio != 1)	// 화면 이동
			{
				int temPosX, temPosY;

				temPosX = m_posX + (ClickedDistance.x - point.x);
				temPosY = m_posY + (ClickedDistance.y - point.y);

				// 왼쪽 위 방향 이동 제한
				if (temPosX < 0) temPosX = 0;
				if (temPosY < 0) temPosY = 0;

				// 오른쪽 아래 방향 이동 제한
				if ((temPosX)>(m_StretchSizeX - m_bmp_size_x)) { temPosX = (m_StretchSizeX - m_bmp_size_x); }
				if ((temPosY)>(m_StretchSizeY - m_bmp_size_y)) { temPosY = (m_StretchSizeY - m_bmp_size_y); }

				m_posX = temPosX;
				m_posY = temPosY;

				ClickedDistance.x = point.x;		ClickedDistance.y = point.y;
			}

			//마우스 옆 텍스트를 표시해주기위한 Rect Invalidate
			if (ux > 455 && uy < 17)
				InvalidateTextRect(point, -(80 + (pDoc->m_Font_Size - 1) * 8), 20, 20, 40 + ((pDoc->m_Font_Size - 1) * 5));
			else if (ux <= 455 && uy < 17)
				InvalidateTextRect(point, 0, 20, 80 + ((pDoc->m_Font_Size - 1) * 8), 40 + ((pDoc->m_Font_Size - 1) * 5));
			else if (ux > 455 && uy >= 17)
				InvalidateTextRect(point, -(80 + (pDoc->m_Font_Size - 1) * 8), -(20 + (pDoc->m_Font_Size - 1) * 3), 20, 0);
			else
				InvalidateTextRect(point, 0, -(20 + (pDoc->m_Font_Size - 1) * 3), 80 + ((pDoc->m_Font_Size - 1) * 8), 30);

			if (m_isPoint_clicked == true) {	// 마우스 옆 텍스트처럼 사분면별로 위치가 바뀐다면 따로 처리 필요
				InvalidatePOIRect(point, -(10 + pDoc->m_POIArr[m_point_idx].x), -(10 + pDoc->m_POIArr[m_point_idx].y), 150 + ((pDoc->m_Font_Size - 1) * 8), 40 + ((pDoc->m_Font_Size - 1) * 4));
			}
		}
	}
	m_XY = point;
	
	if (!theApp.m_bLoggingRunning)
	{
		CMainFrame* pMWnd = (CMainFrame*)AfxGetMainWnd();
		pMWnd->SetStatusText(s);		//하단 상태바에 좌표값과 온도값 표시
	}

	CView::OnMouseMove(nFlags, point);
}

void CGlassFlowView::MoveROILine(int line_case)
{
	int tempLinePos;

	switch (line_case)
	{
	case BASE_ROI_LEFT:
		tempLinePos = pDoc->m_BaseROI.lx - (ClickedDistance.x - ux);

		if (tempLinePos <= 0)
		{
			m_bROI_LLine_flag = false;
			MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (pDoc->m_BaseROI.lx - (ClickedDistance.x - ux) > pDoc->m_BaseROI.rx)
			{
				m_bROI_LLine_flag = false;
				m_bROI_RLine_flag = true;
			}
			else
				pDoc->m_BaseROI.lx -= (ClickedDistance.x - ux);

			ClickedDistance.x = ux;
		}
		break;
	case BASE_ROI_RIGHT:
		tempLinePos = pDoc->m_BaseROI.rx - (ClickedDistance.x - ux);

		if (tempLinePos <= 0)
		{
			m_bROI_RLine_flag = false;
			MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (pDoc->m_BaseROI.rx - (ClickedDistance.x - ux) < pDoc->m_BaseROI.lx)
			{
				m_bROI_RLine_flag = false;
				m_bROI_LLine_flag = true;
			}
			else
				pDoc->m_BaseROI.rx -= (ClickedDistance.x - ux);

			ClickedDistance.x = ux;
		}
		break;
	case BASE_ROI_TOP:
		tempLinePos = pDoc->m_BaseROI.ty - (ClickedDistance.y - uy);

		if (tempLinePos <= 0)
		{
			m_bROI_TLine_flag = false;
			MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (pDoc->m_BaseROI.ty - (ClickedDistance.y - uy) > pDoc->m_BaseROI.by)
			{
				m_bROI_TLine_flag = false;
				m_bROI_BLine_flag = true;
			}
			else
				pDoc->m_BaseROI.ty -= (ClickedDistance.y - uy);

			ClickedDistance.y = uy;
		}
		break;
	case BASE_ROI_BOTTOM:
		tempLinePos = pDoc->m_BaseROI.by - (ClickedDistance.y - uy);

		if (tempLinePos <= 0)
		{
			m_bROI_BLine_flag = false;
			MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (pDoc->m_BaseROI.by - (ClickedDistance.y - uy) < pDoc->m_BaseROI.ty)
			{
				m_bROI_BLine_flag = false;
				m_bROI_TLine_flag = true;
			}
			else
			pDoc->m_BaseROI.by -= (ClickedDistance.y - uy);

			ClickedDistance.y = uy;
		}
		break;
	}

	this->Invalidate(FALSE);
}

void CGlassFlowView::InvalidateTextRect(CPoint point, int x1, int y1, int x2, int y2)
{
	///마우스 옆 텍스트를 표시해주기위한 Rect Invalidate
	InvalidateRect(&OldRecT, 0);

	CurrentRect.left = point.x + x1;
	CurrentRect.top = point.y + y1;
	CurrentRect.right = point.x + x2;
	CurrentRect.bottom = point.y + y2;

	OldRecT = CurrentRect;

	InvalidateRect(&CurrentRect, 0);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CGlassFlowView::OnEraseBkgnd(CDC* pDC)
{
	return CView::OnEraseBkgnd(pDC);
	//return FALSE;
}


BOOL CGlassFlowView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{

	return TRUE;
	//return CView::OnSetCursor(pWnd, nHitTest, message);
}

///////////////////////////////////////////////////////////////////////////////////////

void CGlassFlowView::OnDataplayerPreviousrecord()
{
	if (pDoc->m_IdxDS > 0) {
		theApp.DDAQ_IRDX_FILE_SetCurDataSet(pDoc->m_hIRDX_Doc, pDoc->m_IdxDS - 1);
		pDoc->m_IdxDS--;

		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_DataPlaySlider.SetPos(pDoc->m_IdxDS);

		IrdxUpdate();

		//pDoc->UpdateAllViews(NULL, 7);
		theApp.m_pPropertyWnd->UpdateDataSet();
	}
}

void CGlassFlowView::OnUpdateDataplayerPreviousrecord(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pDoc->m_IdxDS>0 && !pDoc->m_bDataPlay);
}

void CGlassFlowView::OnDataplayerNextrecord()
{
	if (pDoc->m_IdxDS < pDoc->m_NoDS - 1) {
		theApp.DDAQ_IRDX_FILE_SetCurDataSet(pDoc->m_hIRDX_Doc, pDoc->m_IdxDS + 1);
		pDoc->m_IdxDS++;

		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_DataPlaySlider.SetPos(pDoc->m_IdxDS);

		IrdxUpdate();

		//pDoc->UpdateAllViews(NULL, 7);
		theApp.m_pPropertyWnd->UpdateDataSet();
	}
}

void CGlassFlowView::OnUpdateDataplayerNextrecord(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pDoc->m_IdxDS < pDoc->m_NoDS - 1 && !pDoc->m_bDataPlay);
}

void CGlassFlowView::OnDataplayerPlay()
{
	pDoc->m_bDataPlay = true;

	if (m_TimerID == 0) {
		m_TimerID = SetTimer(theApp.TimerEventID, 50, NULL);
		theApp.TimerEventID++;
	}
}

void CGlassFlowView::OnUpdateDataplayerPlay(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!pDoc->m_bDataPlay && pDoc->m_IdxDS < pDoc->m_NoDS - 1);
}

void CGlassFlowView::OnDataplayerStop()
{
	pDoc->m_bDataPlay = false;

	if (m_TimerID != 0) {
		KillTimer(m_TimerID);
		m_TimerID = 0;
	}
}

void CGlassFlowView::IrdxUpdate()
{
	////////////////////////////////////////////////////////
	pDoc->GetCamNBufferSize();

	// Update Main View //////////////////////////
	pDoc->UpdateAllViews(NULL, 1);


	if (pDoc->pFrameBuffer == NULL)
	{
		AfxMessageBox(IDS_ERROR_ALLOCMEM);
		return;
	}

	// get data
	if (!theApp.DDAQ_IRDX_PIXEL_GetData(pDoc->m_hIRDX_Doc, pDoc->pFrameBuffer, pDoc->BufSize))
	{
		delete[] pDoc->pFrameBuffer;
		return;
	}

	// Extract Spot Area ****************************************************

	irdx_cs.Lock();
	
	pDoc->CalculateResult(pDoc->PPointArr.GetCount(), pDoc->sizeX, pDoc->sizeY, pDoc->pFrameBuffer);

	irdx_cs.Unlock();

	// Update GlassView ///////////////
	pDoc->UpdateAllViews(NULL, 3);

	theApp.m_pPropertyWnd->UpdateDataSet();

	// Enable NextMessage
	theApp.DDAQ_DEVICE_DO_ENABLE_NextMsg(pDoc->m_nDDAQDeviceNo);
	//////////////////////////////////////////////////////////////////////
}

void CGlassFlowView::OnUpdateDataplayerStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pDoc->m_bDataPlay && pDoc->m_NoDS>1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGlassFlowView::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_TimerID)
	{
		// return if document is not ready now
		if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
			return;

		// number of data sets				
		if (pDoc->m_NoDS<2)
			return;

		// actual index		
		IrdxUpdate();

		if (pDoc->m_IdxDS < pDoc->m_NoDS - 1)
		{
			pDoc->m_IdxDS++;
		}
		else {
			pDoc->m_IdxDS = 0;

			if (m_TimerID) {
				KillTimer(m_TimerID);
			}
			m_TimerID = 0;

			pDoc->m_bDataPlay = false;
		}

		// Update index to irdx
		if (!theApp.DDAQ_IRDX_FILE_SetCurDataSet(pDoc->m_hIRDX_Doc, pDoc->m_IdxDS))
		{
			if (m_TimerID) {
				KillTimer(m_TimerID);
			}
			m_TimerID = 0;
		
			pDoc->m_bDataPlay = false;

			return;
		}

		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

		if (pFrame->m_pCurrentDoc == pDoc) {
			pFrame->m_DataPlaySlider.SetPos(pDoc->m_IdxDS);
			theApp.m_pPropertyWnd->UpdateDataSet();
		}

		// update all views
		pDoc->UpdateAllViews(NULL, 1);

		if (pDoc->m_IdxDS == 0)
			return;
	}

	CView::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////

void CGlassFlowView::CalculatePoint(CPoint point)
{
	// offset
	//float fx = (float)point.x - (float)(pDoc->m_BaseROI.rx - pDoc->m_BaseROI.lx);
	//float fy = (float)point.y - (float)(pDoc->m_BaseROI.by - pDoc->m_BaseROI.ty);
	float fx = (float)point.x - m_bmp_ofs_x + m_posX;
	float fy = (float)point.y - m_bmp_ofs_y + m_posY;

	// zoom
	fx /= m_bmp_zoom;
	fy /= m_bmp_zoom;

	// possible internal additonal zoom!
	unsigned short	ZMode = DDAQ_ZMODE_DIRECT;
	float			Zoom = 1.0f;
	theApp.DDAQ_IRDX_IMAGE_GetZoom(pDoc->m_hIRDX_Doc, &ZMode, &Zoom);
	if (ZMode > DDAQ_ZMODE_DIRECT)
	{
		fx /= Zoom;
		fy /= Zoom;
	}

	ux = (unsigned short)fx + 1;
	uy = (unsigned short)fy + 1;
}


int CGlassFlowView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	pDoc = GetDocument();

	m_isLBtnDown = false;
	m_isLBtnUp = false;
	m_PointMove_flag = false;

	// Config로 넘길지 자동 코딩 될지 선택
	m_isBaseRectDone = false;
	m_isBaseRectFirst = true;

	m_isInsideBaseROI = false;

	m_bROI_BLine_flag = false;
	m_bROI_TLine_flag = false;
	m_bROI_RLine_flag = false;
	m_bROI_LLine_flag = false;

	pDoc->m_POI_count = 0;
	m_point_idx = 0;

	ClickedDistance = CPoint(0, 0);

	return 0;
}

void CGlassFlowView::InvalidatePOIRect(CPoint point, int x1, int y1, int x2, int y2) {
	InvalidateRect(&POI_oldRect, 0);

	POI_currentRect.left = point.x + x1;
	POI_currentRect.top = point.y + y1;
	POI_currentRect.right = point.x + x2;
	POI_currentRect.bottom = point.y + y2;

	POI_oldRect = POI_currentRect;

	InvalidateRect(&POI_currentRect, 0);
}


void CGlassFlowView::OnPOIDraw()
{
	if(pDoc->m_POI_count < 10)
		m_POI_Mode = DRAW_POI;		// POI 그리기
}

void CGlassFlowView::OnUpdatePOIDraw(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_POI_Mode == DRAW_POI);
}

void CGlassFlowView::OnPOIDelete()
{
	m_POI_Mode = MOVE_MODE;		// POI 지우기

	if (pDoc->m_POIArr.GetCount() > 0)
	{
		pDoc->m_POI_count--;
		pDoc->columnUpdateFlag = true;

		if (m_isPoint_clicked == true) 
		{
			pDoc->m_POIArr.RemoveAt(m_point_idx);

			pDoc->DeletedIndex.Add(pDoc->RunningIndex[m_point_idx]);
			pDoc->RunningIndex.RemoveAt(m_point_idx);

			// DeletedIndex Element Sorting
			SortCArray(pDoc->DeletedIndex, CompareInt);
		}
		else
		{
			pDoc->m_POIArr.RemoveAt(pDoc->m_POIArr.GetCount()-1);

			pDoc->DeletedIndex.Add(pDoc->RunningIndex[pDoc->RunningIndex.GetCount() - 1]);
			pDoc->RunningIndex.RemoveAt(pDoc->RunningIndex.GetCount()-1);

			// DeletedIndex Element Sorting
			SortCArray(pDoc->DeletedIndex, CompareInt);
		}
	}
	this->Invalidate(FALSE);

	m_isPoint_clicked = false;
}

void CGlassFlowView::OnUpdatePOIDelete(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_POI_Mode == SPARE_MODE);
}

void CGlassFlowView::OnRoiRefMove()
{
	m_POI_Mode = MOVE_MODE;		// POI 이동
}

void CGlassFlowView::OnUpdateRoiRefMove(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_POI_Mode == MOVE_MODE);
}

void CGlassFlowView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

bool CGlassFlowView::InsideBaseROI(unsigned short ux, unsigned short uy, BASE_ROI baseROI)
{
	bool insideBaseROI = false;

	if (baseROI.lx < ux - 1 && baseROI.rx - 1 > ux && baseROI.ty - 1 < uy && baseROI.by - 1 > uy)
	{
		insideBaseROI = true;
	}

	return insideBaseROI;
}

bool CGlassFlowView::CheckLineBaseROI(int line_case, int sLine, int sideOne, int sideTwo, int gap, unsigned short ux, unsigned short uy)
{
	bool result = false;

	switch (line_case)
	{
	case BASE_ROI_LEFT:
		if (sideOne - 2 < uy - 1 && sideTwo + gap > uy && sLine - gap < ux && sLine + gap > ux)
			result = true;
		break;
	case BASE_ROI_RIGHT:
		if (sideOne - 2 < uy - 1 && sideTwo + gap > uy && sLine - gap < ux && sLine + gap > ux)
			result = true;
		break;
	case BASE_ROI_TOP:
		if (sideOne - 2 < ux - 1 && sideTwo + gap > ux && sLine - gap < uy && sLine + gap > uy)
			result = true;
		break;
	case BASE_ROI_BOTTOM:
		if (sideOne - 2 < ux - 1 && sideTwo + gap > ux && sLine - gap < uy && sLine + gap > uy)
			result = true;
		break;
	}

	return result;
}

void CGlassFlowView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_POI_Mode == DRAW_POI && pDoc->m_POI_count < 10 &&
		(point.x > m_bmp_ofs_x && point.y > m_bmp_ofs_y &&
		(point.x < (m_bmp_ofs_x + m_bmp_size_x)) && (point.y < (m_bmp_ofs_y + m_bmp_size_y))))	// 그리기 모드일 때 & POI 개수 10개 미만 & 화면 안쪽 
	{
		POINT tempPoint;
		tempPoint.x = ux;
		tempPoint.y = uy;

		m_point_idx = pDoc->m_POI_count;

		ClickedDistance.x = ux;
		ClickedDistance.y = uy;

		pDoc->m_POI_count++;

		m_isLBtnDown = true;
		m_PointMove_flag = true;
		m_isPoint_clicked = true;

		pDoc->columnUpdateFlag = true;

		pDoc->m_POIArr.Add(tempPoint);

		if (pDoc->DeletedIndex.GetCount() == 0) {
			pDoc->RunningIndex.Add(m_point_idx);
		}
		else if (pDoc->RunningIndex.GetCount() == 0 && pDoc->DeletedIndex.GetCount() != 0) {
			pDoc->DeletedIndex.RemoveAll();
			pDoc->RunningIndex.Add(m_point_idx);
		}
		else {
			pDoc->RunningIndex.Add(pDoc->DeletedIndex[0]);
			pDoc->DeletedIndex.RemoveAt(0);
		}
	}
	else if (CheckLineBaseROI(BASE_ROI_LEFT, pDoc->m_BaseROI.lx, pDoc->m_BaseROI.ty, pDoc->m_BaseROI.by, 2, ux - 1, uy - 1))
	{
		m_bROI_LLine_flag = true;

		ClickedDistance.x = ux - 1;
	}
	else if (CheckLineBaseROI(BASE_ROI_RIGHT, pDoc->m_BaseROI.rx, pDoc->m_BaseROI.ty, pDoc->m_BaseROI.by, 2, ux - 1, uy - 1))
	{
		m_bROI_RLine_flag = true;

		ClickedDistance.x = ux - 1;
	}
	else if (CheckLineBaseROI(BASE_ROI_TOP, pDoc->m_BaseROI.ty, pDoc->m_BaseROI.lx, pDoc->m_BaseROI.rx, 2, ux - 1, uy - 1))
	{
		m_bROI_TLine_flag = true;

		ClickedDistance.y = uy - 1;
	}
	else if (CheckLineBaseROI(BASE_ROI_BOTTOM, pDoc->m_BaseROI.by, pDoc->m_BaseROI.lx, pDoc->m_BaseROI.rx, 2, ux - 1, uy - 1))
	{
		m_bROI_BLine_flag = true;

		ClickedDistance.y = uy - 1;
	}
	else if (pDoc->m_POI_count > 0)			// POI가 있을 시
	{
		bool hit = false;

		for (int i = 0; i < pDoc->m_POI_count; i++)
		{
			if (pDoc->m_POIArr[i].x - 4 < (ux - 1) && pDoc->m_POIArr[i].x + 4 > (ux - 1) &&
				pDoc->m_POIArr[i].y - 4 < (uy - 1) && pDoc->m_POIArr[i].y + 4 > (uy - 1))
			{
				m_PointMove_flag = true;
				m_isPoint_clicked = true;
				hit = true;

				m_point_idx = i;

				ClickedDistance.x = ux;
				ClickedDistance.y = uy;
			}
		}
		if (!hit)
			m_isPoint_clicked = false;
	}


	if (m_isBaseRectFirst && !m_isBaseRectDone && !m_PointMove_flag && (GetKeyState(VK_CONTROL) & 0x8000))
	{
		m_isBaseRectFirst = false;

		pDoc->m_BaseROI.lx = ux - 1;
		pDoc->m_BaseROI.ty = uy - 1;
	}

	this->Invalidate(FALSE);
	//pDoc->UpdateAllViews(4);
	CView::OnLButtonDown(nFlags, point);
}


void CGlassFlowView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (1) // POI 모드가 생성 권한을 가질 때 & L버튼 Flag 활성일 때
	{
		if (point.x > m_bmp_ofs_x && point.y > m_bmp_ofs_y && (point.x < (m_bmp_ofs_x + m_bmp_size_x)) && (point.y < (m_bmp_ofs_y + m_bmp_size_y))) // 화면 안쪽일 때
		{
			m_isLBtnDown = false;

			m_isLBtnDown = false;

			/*if (m_POI_Mode == DRAW_POI) {
				if (pDoc->DeletedIndex.GetCount() == 0) {
					pDoc->RunningIndex.Add(m_point_idx);
				}
				else if (pDoc->RunningIndex.GetCount() == 0 && pDoc->DeletedIndex.GetCount() != 0) {
					pDoc->DeletedIndex.RemoveAll();
					pDoc->RunningIndex.Add(m_point_idx);
				}
				else {
					pDoc->RunningIndex.Add(pDoc->DeletedIndex[0]);
					pDoc->DeletedIndex.RemoveAt(0);
				}
			}*/

			m_POI_Mode = MOVE_MODE;
		}
		else
		{
			//생성되었던 POI 제거
		}
	}

	if (m_bROI_BLine_flag || m_bROI_LLine_flag || m_bROI_RLine_flag || m_bROI_TLine_flag)
	{
		m_bROI_BLine_flag = false;	m_bROI_LLine_flag = false;	m_bROI_RLine_flag = false;	m_bROI_TLine_flag = false;

		ClickedDistance.x = 0;
		ClickedDistance.y = 0;
	}

	if (m_PointMove_flag)
	{
		m_PointMove_flag = false;

		ClickedDistance.x = 0;
		ClickedDistance.y = 0;
	}
	
	// Ctrl 키 유지하면서 마우스 up
	if (!m_isBaseRectFirst && !m_isBaseRectDone && (GetKeyState(VK_CONTROL) & 0x8000))
	{
		m_isBaseRectDone = true;

		pDoc->m_BaseROI.rx = ux - 1;
		pDoc->m_BaseROI.by = uy - 1;

		if (pDoc->m_BaseROI.rx < pDoc->m_BaseROI.lx)
		{
			int tempX = pDoc->m_BaseROI.rx;
			pDoc->m_BaseROI.rx = pDoc->m_BaseROI.lx;
			pDoc->m_BaseROI.lx = tempX;
		}
		if (pDoc->m_BaseROI.by < pDoc->m_BaseROI.ty)
		{
			int tempY = pDoc->m_BaseROI.by;
			pDoc->m_BaseROI.by = pDoc->m_BaseROI.ty;
			pDoc->m_BaseROI.ty = tempY;
		}
	}	
	else if ((GetKeyState(VK_SHIFT) & 0x8000) && m_isBaseRectDone)
	{
		m_isBaseRectDone = false;
		m_isBaseRectFirst = true;

		pDoc->m_BaseROI.lx = 0;
		pDoc->m_BaseROI.ty = 0;

		pDoc->m_BaseROI.rx = 0;
		pDoc->m_BaseROI.by = 0;
	}
	else if (!m_isBaseRectFirst && !m_isBaseRectDone)
	{	// 마우스만 Up 할 시
		m_isBaseRectFirst = true;

		pDoc->m_BaseROI.lx = 0;
		pDoc->m_BaseROI.ty = 0;

		pDoc->m_BaseROI.rx = 0;
		pDoc->m_BaseROI.by = 0;
	}

	this->Invalidate(FALSE);

	CView::OnLButtonUp(nFlags, point);
}

void CGlassFlowView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (pDoc->m_BaseROI.rx != 0 && pDoc->m_BaseROI.by != 0 && InsideBaseROI(ux, uy, pDoc->m_BaseROI))
	{
		m_isInsideBaseROI = true;

		ClickedDistance.x = ux - 1;
		ClickedDistance.y = uy - 1;
	}
	else
	{
		m_ViewMoveFlag = true;

		ClickedDistance.x = point.x;
		ClickedDistance.y = point.y;
	}

	CView::OnRButtonDown(nFlags, point);
}

void CGlassFlowView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_isInsideBaseROI)
	{
		m_isInsideBaseROI = false;

		ClickedDistance.x = 0;
		ClickedDistance.y = 0;
	}
	if (m_ViewMoveFlag)
	{
		m_ViewMoveFlag = false;

		ClickedDistance.x = 0;
		ClickedDistance.y = 0;
	}

	this->Invalidate(FALSE);
	CView::OnRButtonUp(nFlags, point);
}

BOOL CGlassFlowView::PreTranslateMessage(MSG* pMsg) {
	if (m_isPoint_clicked == true && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE) OnPOIDelete();


	return CView::PreTranslateMessage(pMsg);
}