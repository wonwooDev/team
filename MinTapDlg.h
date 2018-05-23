#pragma once
#include "tchart1.h"


// MinTapDlg 대화 상자입니다.

class CMinTapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMinTapDlg)

public:
	CMinTapDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.

	CPyroSoftMDoc* pDoc;
	virtual ~CMinTapDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MIN_TAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTChart m_Min_Chart;
	DECLARE_EVENTSINK_MAP()
	void OnDblClickTchart1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
