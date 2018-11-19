#pragma once
#include "afxwin.h"


// CStepDiffDlg 대화 상자입니다.

class CStepDiffDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStepDiffDlg)

public:
	CPyroSoftMDoc* pDoc;

	CEdit m_upStep_diff;
	CEdit m_middleStep_diff;
	CEdit m_downStep_diff;

	CStepDiffDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CStepDiffDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STEP_DIFF };
#endif

protected:
	void PrintStepDiff();
	void CheckEmptyValue(CString str, int* checkCount);
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
