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
	DDX_Control(pDX, IDC_TCHART1, m_Min_Chart);
}


BEGIN_MESSAGE_MAP(CMinTapDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// MinTapDlg 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CMinTapDlg, CDialogEx)
	ON_EVENT(CMinTapDlg, IDC_TCHART1, 8, CMinTapDlg::OnDblClickTchart1, VTS_NONE)
END_EVENTSINK_MAP()


void CMinTapDlg::OnDblClickTchart1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_Min_Chart.GetSeriesCount() > 0)
		m_Min_Chart.ShowEditor(0);
}

void CMinTapDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (m_Min_Chart.m_hWnd == NULL) return;

	CRect rect;
	GetClientRect(&rect);

	m_Min_Chart.SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOZORDER);
}