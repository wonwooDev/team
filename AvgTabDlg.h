#pragma once
#include "tchart1.h"


// AvgTabDlg 대화 상자입니다.

class CAvgTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAvgTabDlg)

public:
	CAvgTabDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CPyroSoftMDoc* pDoc;

	virtual ~CAvgTabDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AVG_TAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTChart m_Avg_Chart;
	DECLARE_EVENTSINK_MAP()
	void OnDblClickTchart1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
