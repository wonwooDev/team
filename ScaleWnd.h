
#pragma once

#include "ScaleDlg.h"

/////////////////////////////////////////////////////////////////////////////
// COutputList 창

class CScaleWnd : public CDockablePane
{
// 생성입니다.
public:
	CScaleWnd();


// 특성입니다.
protected:



// 구현입니다.
public:
	virtual ~CScaleWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:
	CScaleDlg m_ScaleDlg;

	void AdjustLayout();


};

