
// GlassFlowView.h : CGlassFlowView 클래스의 인터페이스
//

#pragma once

#include "PyroSoftMDoc.h"

//Polygon
#define MIN(x,y) (x < y ? x : y) 
#define MAX(x,y) (x > y ? x : y) 
#define POLYGONINSIDE	0 
#define POLYGONOUTSIDE	1 
#define	SIN				0
#define	COS				1
#define QUADRANT1		1		
#define QUADRANT2		2
#define QUADRANT3		3
#define QUADRANT4		4
#define	ELLIPSESTART	5

//ROI & REF MENU MODE
#define DRAW_POI		1
#define DELETE_POI		2
#define MOVE_MODE		3
#define	SPARE_MODE		4

//Ellipse
#define ROUND(x) (int)(x+0.5) // 반올림
#define ROUND(y) (int)(y+0.5)

typedef struct {
	double x, y;
} MPoint;

class CGlassFlowView : public CView
{
protected: // serialization에서만 만들어집니다.
	CGlassFlowView();
	DECLARE_DYNCREATE(CGlassFlowView)

	void CalculatePoint(CPoint point);

	UINT				m_TimerID;

	unsigned short		m_bmp_isize_x;	// internal size
	unsigned short		m_bmp_isize_y;	

	int		m_bmp_size_x;	// drawing size
	int		m_bmp_size_y;
	int		m_bmp_ofs_x;	// drawing ofs
	int		m_bmp_ofs_y;

	bool	m_isBaseRectFirst;
	bool	m_isBaseRectDone;

	bool	m_isInsideBaseROI;

	RECT	m_OBaseRect;
	RECT	m_CBaseRect;
	
	float	m_bmp_zoom;		// drawing zoom

	CPoint m_XY;
	CString TmperInf;
	float DataPoint;
	unsigned short ux;
	unsigned short uy;

	bool	isDeleteEllipse;
	bool	m_isLBtnDown;
	bool	m_isLBtnUp;
	bool	m_PointMove_flag;
	bool	m_isPoint_clicked;
	bool	m_bROI_TLine_flag;
	bool	m_bROI_BLine_flag;
	bool	m_bROI_LLine_flag;
	bool	m_bROI_RLine_flag;

	int		m_point_idx;

	int		m_StretchSizeX;
	int		m_StretchSizeY;
	int		m_bforeStretchSizeX;
	int		m_bforeStretchSizeY;

	int		m_posX;
	int		m_posY;

	bool	m_ViewMoveFlag;

	float	m_CurrentRatio;

	POINT ClickedDistance;

	// 메뉴 항목
	int		m_POI_Mode;			// POI 메뉴 항목 모드 선택

	//Mouse Point Text
	RECT	OldRecT, CurrentRect;

	RECT POI_oldRect, POI_currentRect;

	void		IrdxUpdate();
	void		InvalidateTextRect(CPoint point, int x1, int y1, int x2, int y2);
	void		RectROIDraw(CDC* pMemDC, int pWidht, int min_x[], int min_y[], int max_x[], int max_y[], int loop_count,COLORREF rect_color);
	void		DotROIDraw(CDC* pMemDC, int tempX[], int tempY[], float temp[], int loop_count, 
							int select_idx, COLORREF back_color, COLORREF front_color, COLORREF select_color);
	void		CrossROIDraw(CDC* pMemDC, int tempX[], int tempY[], float temp[], int loop_count, 
							int select_idx, COLORREF back_color, COLORREF front_color, COLORREF select_color);
	void		SelectRect(RECT* CBaseRect, BASE_ROI m_BaseROI, unsigned short ux, unsigned short uy);
	void		SetInvaliRect(RECT* CBaseRect, BASE_ROI m_BaseROI, int quadrant, int sx, int sy, int ex, int ey);
	void		MoveROILine(int line_case);

	bool		InsideBaseROI(unsigned short ux, unsigned short uy, BASE_ROI baseROI);
	bool		CheckLineBaseROI(int line_case, int sLine, int sideOne, int sideTwo, int gap, unsigned short ux, unsigned short uy);

// 특성입니다.
public:
	CPyroSoftMDoc* GetDocument() const;
	CPyroSoftMDoc* pDoc;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
protected:
	void		InvalidatePOIRect(CPoint point, int x1, int y1, int x2, int y2);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CGlassFlowView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void OnDataplayerPreviousrecord();
	afx_msg void OnUpdateDataplayerPreviousrecord(CCmdUI *pCmdUI);
	afx_msg void OnDataplayerNextrecord();
	afx_msg void OnUpdateDataplayerNextrecord(CCmdUI *pCmdUI);
	afx_msg void OnDataplayerPlay();
	afx_msg void OnUpdateDataplayerPlay(CCmdUI *pCmdUI);
	afx_msg void OnDataplayerStop();
	afx_msg void OnUpdateDataplayerStop(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPOIDraw();
	afx_msg void OnUpdatePOIDraw(CCmdUI *pCmdUI);
	afx_msg void OnPOIDelete();
	afx_msg void OnUpdatePOIDelete(CCmdUI *pCmdUI);
	afx_msg void OnRoiRefMove();
	afx_msg void OnUpdateRoiRefMove(CCmdUI *pCmdUI);
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG * pMsg);
};

#ifndef _DEBUG  // GlassFlowView.cpp의 디버그 버전
inline CPyroSoftMDoc* CGlassFlowView::GetDocument() const
   { return reinterpret_cast<CPyroSoftMDoc*>(m_pDocument); }
#endif

