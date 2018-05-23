#pragma once
#include "afxwin.h"


// CZoneInfDlg 대화 상자입니다.
class CZoneInfDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZoneInfDlg)

	//특성입니다.
public:
	CPyroSoftMDoc * pDoc;

	CFont m_bFont;
	CFont m_mFont;
	CFont m_sFont;

	CEdit m_Zone_Distance[MAX_ZONE];
	CEdit m_Zone_Speed[MAX_ZONE];
	CEdit m_Zone_Temp[MAX_ZONE];
	CEdit m_Start_Position;
	CEdit m_End_Position;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REF_INF};
#endif

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	bool m_EmptyStringFlag;

	void LoadRecipeConfig();

	DECLARE_MESSAGE_MAP()

	//작업입니다.
public:

	void InitRefDlg();
	void ZeroInit();
	void PrintZoneInf();
	void CheckEmptyValue(CString str);

	CZoneInfDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	
	virtual ~CZoneInfDlg();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedApplyZone();
	afx_msg void OnBnClickedLoadRecipe();
	afx_msg void OnBnClickedSaveZone();
	afx_msg void OnBnClickedCloseZone();
};
