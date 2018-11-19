// StepDiffDlg.cpp : 구현 파일입니다.
//


#include "stdafx.h"
#include "PyroSoftM.h"
#include "StepDiffDlg.h"
#include "afxdialogex.h"


// CStepDiffDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CStepDiffDlg, CDialogEx)

CStepDiffDlg::CStepDiffDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_STEP_DIFF, pParent)
{

}

CStepDiffDlg::~CStepDiffDlg()
{
}

void CStepDiffDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UP_STEP_DIFF, m_upStep_diff);
	DDX_Control(pDX, IDC_MIDDLE_STEP_DIFF, m_middleStep_diff);
	DDX_Control(pDX, IDC_DOWN_STEP_DIFF, m_downStep_diff);
}

BEGIN_MESSAGE_MAP(CStepDiffDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CStepDiffDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CStepDiffDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CStepDiffDlg 메시지 처리기입니다.
BOOL CStepDiffDlg::PreTranslateMessage(MSG* pMsg)
{
	//엔터키를 누르면 UPdate할 수 있도록 
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
		return FALSE;
	}
	else
		return CDialogEx::PreTranslateMessage(pMsg);
}

void CStepDiffDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	float tempUpStemp = 0.0f, tempMiddleStep = 0.0f, tempDownStep = 0.0f;
	int checkApply = 0;

	m_upStep_diff.GetWindowTextA(str);
	tempUpStemp = _ttof(str);
	CheckEmptyValue(str, &checkApply);

	m_middleStep_diff.GetWindowTextA(str);
	tempMiddleStep = _ttof(str);
	CheckEmptyValue(str, &checkApply);

	m_downStep_diff.GetWindowTextA(str);
	tempDownStep = _ttof(str);
	CheckEmptyValue(str, &checkApply);

	if (checkApply)	
	{
		AfxMessageBox("존재하지 않는 값이 존재합니다.");	// -승태임무
	}
	else
	{
		pDoc->m_upStepCoeff = tempUpStemp;
		pDoc->m_middleStepCoeff = tempMiddleStep;
		pDoc->m_downStepCoeff = -tempDownStep;

		AfxMessageBox("값이 적용 되었습니다.");
		CDialogEx::OnOK();
	}
}


void CStepDiffDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
	CDialogEx::OnCancel();
}

void CStepDiffDlg::CheckEmptyValue(CString str, int* checkCount)
{
	if (str == "" || str == "0")
		(*checkCount)++;
}


BOOL CStepDiffDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	PrintStepDiff();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CStepDiffDlg::PrintStepDiff()
{
	CString str;

	str.Format(_T("%.4f"), pDoc->m_upStepCoeff);
	m_upStep_diff.SetWindowText(str);

	str.Format(_T("%.4f"), pDoc->m_middleStepCoeff);
	m_middleStep_diff.SetWindowText(str);

	str.Format(_T("%.4f"), -pDoc->m_downStepCoeff);
	m_downStep_diff.SetWindowText(str);

	//int a = 1, b = 2, c = 0;

	//::CalPlus(a, b, &c);

	//CString ss;
	//ss.Format(_T("%d"), c);

	//AfxMessageBox(ss);
}
