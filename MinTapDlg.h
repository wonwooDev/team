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
	CStatic spread_static;
	CEdit spread_edit;
	CTChart m_Spread_Chart;
	CFont spread_static_font;
	CFont spread_edit_font;

	DECLARE_EVENTSINK_MAP()
	void OnDblClickTchart1();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
};
