// MaxTabDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PyroSoftM.h"
#include "MaxTabDlg.h"
#include "afxdialogex.h"
#include "teeeditor.h"


// MaxTabDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMaxTabDlg, CDialogEx)

CMaxTabDlg::CMaxTabDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAX_TAB_DIALOG, pParent)
{
}

CMaxTabDlg::~CMaxTabDlg()
{
}

void CMaxTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAX_TCHART, m_Max_Chart);
}


BEGIN_MESSAGE_MAP(CMaxTabDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// MaxTabDlg 메시지 처리기입니다.


BOOL CMaxTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
BEGIN_EVENTSINK_MAP(CMaxTabDlg, CDialogEx)
	ON_EVENT(CMaxTabDlg, IDC_MAX_TCHART, 8, CMaxTabDlg::OnDblClickMaxTchart, VTS_NONE)
END_EVENTSINK_MAP()


void CMaxTabDlg::OnDblClickMaxTchart()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_Max_Chart.GetSeriesCount() > 0)
		m_Max_Chart.ShowEditor(0);
}

void CMaxTabDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (m_Max_Chart.m_hWnd == NULL) return;

	CRect rect;
	GetClientRect(&rect);

	m_Max_Chart.SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOZORDER);
}