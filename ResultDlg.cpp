// ResultDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "PyroSoftM.h"
#include "ResultDlg.h"
#include "CoefficientDlg.h"
#include "StepDiffDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WHITE_COLOR		-1

// CResultDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CResultDlg, CDialogEx)

CResultDlg::CResultDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CResultDlg::IDD, pParent)
{
	m_pWndShow = NULL;
}

CResultDlg::~CResultDlg()
{
}


void CResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// [Tab Control]
	DDX_Control(pDX, IDC_TAB1, m_Result_Tap);
	DDX_Control(pDX, IDC_ZONE_BTN, m_ZoneInf_btn);
	DDX_Control(pDX, IDC_COEFFICIENT_BTN, m_Coefficient_btn);
	DDX_Control(pDX, IDC_STEPDIFFERENCE_BTN, m_StepDiff_btn);
	DDX_Control(pDX, IDC_CLEAR_GRAPH, m_ClearGraph_btn);
}

BEGIN_MESSAGE_MAP(CResultDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_ZONE_BTN, &CResultDlg::OnBnClickedZoneBtn)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CResultDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_COEFFICIENT_BTN, &CResultDlg::OnBnClickedCoefficientBtn)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_STEPDIFFERENCE_BTN, &CResultDlg::OnBnClickedStepdifferenceBtn)
	ON_BN_CLICKED(IDC_CLEAR_GRAPH, &CResultDlg::OnBnClickedClearGraph)
END_MESSAGE_MAP()
// CResultDlg 메시지 처리기입니다.

HBRUSH CResultDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	///mdb
	UINT nID = pWnd->GetDlgCtrlID();


	return hbr;
}

BOOL CResultDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// [Tab Init]
	CString tabOne = "Max";
	CString	tabTwo = "Spread";
	//CString tabThree = "Avg";

	m_Result_Tap.InsertItem(1, tabOne);
	m_Result_Tap.InsertItem(2, tabTwo);
	//m_Result_Tap.InsertItem(3, tabThree);

	CRect rect;
	m_Result_Tap.GetClientRect(&rect);


	if (m_MaxTabDlg.GetSafeHwnd() == 0)
	{
		if (!m_MaxTabDlg.Create(IDD_MAX_TAB_DIALOG, &m_Result_Tap))
			return FALSE;

		m_MaxTabDlg.SetWindowPos(NULL, 5, 25, rect.Width() - 12, rect.Height() - 33, SWP_SHOWWINDOW | SWP_NOZORDER);
	}
	///if (m_MinTabDlg.GetSafeHwnd() == 0)
	if(m_SpreadTabDlg.GetSafeHwnd() == 0)
	{
		if (!m_SpreadTabDlg.Create(IDD_MIN_TAB_DIALOG, &m_Result_Tap))
			return FALSE;

		m_SpreadTabDlg.SetWindowPos(NULL, 5, 25, rect.Width() - 12, rect.Height() - 33, SWP_NOZORDER);
	}
	/*if (m_AvgTabDlg.GetSafeHwnd() == 0)
	{
		if (!m_AvgTabDlg.Create(IDD_AVG_TAB_DIALOG, &m_Result_Tap))
			return FALSE;

		m_AvgTabDlg.SetWindowPos(NULL, 5, 25, rect.Width() - 12, rect.Height() - 33, SWP_NOZORDER);
	}*/

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


//********************************************************************************
BOOL CResultDlg::PreTranslateMessage(MSG* pMsg)
{
	//엔터키를 누르면 UPdate할 수 있도록 
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{

	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return FALSE;
	else
		return CDialogEx::PreTranslateMessage(pMsg);
}

void CResultDlg::ChangeProperties(CPyroSoftMDoc* pDoc)
{

}

template <typename T>
T CResultDlg::ModifiedVal(T object, T a, T b)
{
	T result;

	if (object < a)
	{
		result = a;
	}
	else if (object > b)
	{
		result = b;
	}
	return result;
}

void CResultDlg::CheckROICount(int ROI_count, int cROI_count)
{
	CString str;

	if (ROI_count < cROI_count)
	{
		for (int i = ROI_count; i < cROI_count; i++)
		{
			str.Format("#%d", i + 1);
			m_MaxTabDlg.m_Max_Chart.AddSeries(0);
			m_MaxTabDlg.m_Max_Chart.Series(i).SetTitle(str);
			m_MaxTabDlg.m_Max_Chart.Series(i).SetLegendTitle(str);
			m_MaxTabDlg.m_Max_Chart.Series(i).SetColor(ColorRef(i));
		}
		m_SpreadTabDlg.m_Spread_Chart.AddSeries(0);
		m_SpreadTabDlg.m_Spread_Chart.Series(0).SetColor(ColorRef(WHITE_COLOR));
	}

	else if (ROI_count > cROI_count)
	{
		for (int i = ROI_count - 1; i >= cROI_count; i--)
		{
			m_MaxTabDlg.m_Max_Chart.Series(i).Clear();
			//m_AvgTabDlg.m_Avg_Chart.Series(i).Clear();
			m_MaxTabDlg.m_Max_Chart.RemoveSeries(i);
			//m_AvgTabDlg.m_Avg_Chart.RemoveSeries(i);
		}
		m_SpreadTabDlg.m_Spread_Chart.Series(0).Clear();
		m_SpreadTabDlg.m_Spread_Chart.RemoveSeries(0);
	}
}


COLORREF CResultDlg::ColorRef(int series_idx)
{
	BYTE R = 0, G = 0, B = 0;
	switch (series_idx)
	{
	case 0:		// 빨
		R = 255;
		break;
	case 1:		// 주
		R = 255;
		G = 153;
		break;
	case 2:		// 노
		R = 255;
		G = 255;
		break;
	case 3:		// 초
		G = 255;
		break;
	case 4:		// 파
		B = 255;
		break;
	case 5:		// 남
		B = 51;
		break;
	case 6:		// 보
		R = 102;
		B = 255;
		break;
	case 7:		// 빨약
		R = 255;
		G = 102;
		B = 102;
		break;
	case 8:		// 주약
		R = 255;
		G = 204;
		B = 102;
		break;
	case 9:		// 노약
		R = 255;
		G = 225;
		B = 153;
		break;
	case 10:	// 초약
		R = 153;
		G = 225;
		B = 204;
		break;
	default:
		srand((unsigned)time(NULL));
		R = rand() % 255;
		srand((unsigned)time(NULL));
		G = rand() % 255;
		srand((unsigned)time(NULL));
		B = rand() % 255;
		break;
	}
	return RGB(R, G, B);
}

int CResultDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_font_mdf = false;


	return 0;
}

void CResultDlg::OnBnClickedZoneBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CZoneInfDlg m_Refdlg;
	CString str;

	m_Refdlg.pDoc = pDoc;

	m_Refdlg.DoModal();
}

void CResultDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pWndShow != NULL)
	{
		m_pWndShow->ShowWindow(SW_HIDE);
		m_pWndShow = NULL;
	}

	int nIndex = m_Result_Tap.GetCurSel();
	switch (nIndex)
	{
	case 0:
		m_MaxTabDlg.ShowWindow(SW_SHOW);
		m_pWndShow = &m_MaxTabDlg;
		break;
	case 1:
		m_SpreadTabDlg.ShowWindow(SW_SHOW);
		m_pWndShow = &m_SpreadTabDlg;
		break;
	//case 2:
	//	m_AvgTabDlg.ShowWindow(SW_SHOW);
	//	m_pWndShow = &m_AvgTabDlg;
	//	break;
	}
	*pResult = 0;
}

void CResultDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (m_Result_Tap.m_hWnd == NULL) return;

	CRect rect;
	GetClientRect(&rect);

	m_Result_Tap.SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height()*0.93, SWP_NOZORDER);
	m_MaxTabDlg.SetWindowPos(NULL, 5, 25, rect.Width() - 12, rect.Height()*0.9 - 10, SWP_NOZORDER);
	m_SpreadTabDlg.SetWindowPos(NULL, 5, 25, rect.Width() - 12, rect.Height()*0.9 - 10, SWP_NOZORDER);
	//m_AvgTabDlg.SetWindowPos(NULL, 5, 25, rect.Width() - 12, rect.Height()*0.9 - 10, SWP_NOZORDER);

	//m_SpreadTabDlg.spread_static.SetWindowPos(NULL, rect.Width()*0.1, rect.Height()*0.85-20, rect.Width()*0.2, 15, SWP_NOZORDER);
	//m_SpreadTabDlg.spread_edit.SetWindowPos(NULL, rect.Width()*0.1, rect.Height()*0.85 , rect.Width()*0.8, 20, SWP_NOZORDER);
	m_SpreadTabDlg.spread_static.SetWindowPos(NULL, rect.Width()*0.1, rect.Height()*0.85 - 20, rect.Width()*0.6, 15, SWP_NOZORDER);
	m_SpreadTabDlg.spread_edit.SetWindowPos(NULL, rect.Width()*0.35, rect.Height()*0.81, rect.Width()*0.6, 40, SWP_NOZORDER);

	m_ZoneInf_btn.SetWindowPos(NULL, 5, rect.Height() - 35 - 10, 100, 35, SWP_NOZORDER);
	m_StepDiff_btn.SetWindowPos(NULL, 110, rect.Height() - 35 - 10, 100, 35, SWP_NOZORDER);
	m_ClearGraph_btn.SetWindowPos(NULL, 215, rect.Height() - 35 - 10, 100, 35, SWP_NOZORDER);
}

void CResultDlg::OnBnClickedCoefficientBtn()
{
	CCoefficientDlg m_Coeffdlg;

	m_Coeffdlg.pDoc = pDoc;

	m_Coeffdlg.DoModal();
}

void CResultDlg::OnBnClickedStepdifferenceBtn()
{
	CStepDiffDlg m_StepDiffDlg;

	m_StepDiffDlg.pDoc = pDoc;

	m_StepDiffDlg.DoModal();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CResultDlg::OnBnClickedClearGraph()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (int i = 0; i < pDoc->m_ROICount; i++)
	{
		m_MaxTabDlg.m_Max_Chart.Series(i).Clear();
		//m_AvgTabDlg.m_Avg_Chart.Series(i).Clear();
	}
	for (int k = 0; k < 4; k++) {
		m_SpreadTabDlg.m_Spread_Chart.Series(k).Clear();
	}

	pDoc->m_bSpreadCondition = true;
	pDoc->m_ChartFlag = false;
	pDoc->m_max_idxDS = 0;
}
