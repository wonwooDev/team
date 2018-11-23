// MinTapDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PyroSoftM.h"
#include "MinTapDlg.h"
#include "afxdialogex.h"
#include "teeeditor.h"


// MinTapDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMinTapDlg, CDialogEx)

CMinTapDlg::CMinTapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MIN_TAB_DIALOG, pParent)
{

}

CMinTapDlg::~CMinTapDlg()
{
}

void CMinTapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART1, m_Spread_Chart);
	DDX_Control(pDX, IDC_SPREAD_STATIC, spread_static);
	DDX_Control(pDX, IDC_SPREAD_EDIT, spread_edit);
}


BEGIN_MESSAGE_MAP(CMinTapDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// MinTapDlg 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CMinTapDlg, CDialogEx)
	ON_EVENT(CMinTapDlg, IDC_TCHART1, 8, CMinTapDlg::OnDblClickTchart1, VTS_NONE)
END_EVENTSINK_MAP()


void CMinTapDlg::OnDblClickTchart1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_Spread_Chart.GetSeriesCount() > 0)
		m_Spread_Chart.ShowEditor(0); 
}

void CMinTapDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (m_Spread_Chart.m_hWnd == NULL) return;

	CRect rect;
	GetClientRect(&rect);

	m_Spread_Chart.SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOZORDER);
}

HBRUSH CMinTapDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	switch (nCtlColor)
	{
	case CTLCOLOR_EDIT:
	{
		if ( pWnd->GetDlgCtrlID() == IDC_SPREAD_EDIT && pDoc->m_bSpreadCondition)
			pDC->SetBkColor(RGB(0, 255, 0));
		else
			pDC->SetBkColor(RGB(255, 0, 0));
		break;
	}
	default:
		break;
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


BOOL CMinTapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	spread_static_font.CreateFontA(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "굴림");
	spread_static.SetFont(&spread_static_font);

	spread_edit_font.CreateFontA(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "굴림");
	spread_edit.SetFont(&spread_edit_font);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
