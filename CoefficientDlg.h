#pragma once
#include "afxwin.h"


// CCoefficient 대화 상자입니다.

class CCoefficientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCoefficientDlg)

public:
	CCoefficientDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCoefficientDlg();

	int		m_EmptyStringFlag;

	void	CheckEmptyValue(CString str);

	void	PrintZoneInf();

	CPyroSoftMDoc * pDoc;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COEFFICIENT_SET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAccept();
	afx_msg void OnBnClickedCancel();
	CEdit m_Third_Coeff;
	CEdit m_Second_Coeff;
	CEdit m_First_Coeff;
	CEdit m_Constant;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
