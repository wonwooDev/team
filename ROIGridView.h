#pragma once

#include "ROIGridDlg.h"
#include "ZoneInfDlg.h"

// CROIGridView 뷰입니다.

class CROIGridView : public CView
{
	DECLARE_DYNCREATE(CROIGridView)

protected:
	CROIGridView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CROIGridView();

	void UpdateStatusData();


public:
	CPyroSoftMDoc* pDoc;
	CROIGridDlg	m_ROIGridDlg;
	CRect m_rectCurHist;


	void AdjustLayout();
	void InitStatusData();
	void SetFont();

	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void UpdateGridControlSet();
};


