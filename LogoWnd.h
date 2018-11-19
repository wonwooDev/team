#pragma once

#include "LogoDlg.h"

// CLogoWnd

class CLogoWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CLogoWnd)

public:
	CLogoWnd();

	CLogoDlg m_LogoDlg;

	virtual ~CLogoWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void AdjustLayout();
};


