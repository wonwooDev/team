#pragma once

// Chart header
#include "tchart1.h"	/// 차트 지울거야
#include "tchart.h"
#include "fastlineseries.h"
#include "histogramfunction.h"
#include "histogramseries.h"
#include "horizhistogramseries.h"
#include "TeeChartDefines.h"
#include "series.h"
#include "axis.h"
#include "valuelist.h"
#include "valuelists.h"


// MaxTabDlg 대화 상자입니다.

class CMaxTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMaxTabDlg)

public:
	CMaxTabDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.

	CPyroSoftMDoc* pDoc;
	virtual ~CMaxTabDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAX_TAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CTChart m_Max_Chart;

	DECLARE_EVENTSINK_MAP()
	void OnDblClickMaxTchart();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
