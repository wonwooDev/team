#pragma once

#include "ScaleDlg.h"


// CScaleView 뷰입니다.

class CScaleView : public CView
{
	DECLARE_DYNCREATE(CScaleView)

protected:
	CScaleView();           // 동적 만들기에 사용되는 protected 생성자입니다.

	bool		m_scaleSizeFlag;
	int			m_scaleSizeCount;

	void			AdjustLayout();
	virtual ~CScaleView();

public:
	CPyroSoftMDoc*	pDoc;
	CScaleDlg		m_ScaleDlg;




	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};


