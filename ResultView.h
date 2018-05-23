#pragma once

//#include "PyroSoftMDoc.h"

#include "ResultDlg.h"
#include "MinTapDlg.h"
#include "MaxTabDlg.h"
#include "AvgTabDlg.h"

class CPyroSoftMDoc;

// CResultView 뷰입니다.

class CResultView : public CView
{
	DECLARE_DYNCREATE(CResultView)

protected:
	CResultView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CResultView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

	double				fTime;
	CString				ProcessTime;

	void				CalAllResult();
	
	//특성입니다.
public:
	CDC					memDC;
	CBitmap				bmpBuffer;

	CPyroSoftMDoc*		pDoc;

	CFont				m_bFont;
	CFont				m_mFont;
	CFont				m_sFont;

	CResultDlg			m_ResultDlg;
	CMinTapDlg			m_MinTabDlg;
	CMaxTabDlg			m_MaxTabDlg;
	CAvgTabDlg			m_AvgTabDlg;

	int					m_Dlg_H;
	unsigned long		m_XCount;
	unsigned long		m_XCount_modeThr;

	// For Image 
	unsigned short		m_data_X, m_data_Y;
	float				m_zoomRate;

	int					m_bmp_size_x;	// drawing size
	int					m_bmp_size_y;
	int					m_bmp_ofs_x;	// drawing ofs
	int					m_bmp_ofs_y;
	
	//작업입니다.
public:
	void AdjustLayout();
	void UpdateResult();
	void UpdataSetReadeOnly();
	void InitROIData();
	void setFont();
	void CheckROICount(int ROI_count, int cROI_count);

	COLORREF ColorRef(int series_idx);
	
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);




	int Oldptr;
};


