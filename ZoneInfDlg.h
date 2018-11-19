#pragma once
#include "afxwin.h"


// CZoneInfDlg 대화 상자입니다.
class CZoneInfDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZoneInfDlg)

	//특성입니다.
public:
	CPyroSoftMDoc * pDoc;

	CFont	m_bFont;
	CFont	m_mFont;
	CFont	m_sFont;

	CEdit	m_Zone_Distance[MAX_ZONE];
	CEdit	m_Zone_Speed[MAX_ZONE];
	CEdit	m_Zone_Temp[MAX_ZONE];
	CEdit	m_Start_Position;
	CEdit	m_End_Position;

	bool	m_bIsValuesChanged;
	int		m_updateCount;

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
	void CheckForUpdate();

	CZoneInfDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	
	virtual ~CZoneInfDlg();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedApplyZone();
	afx_msg void OnBnClickedLoadRecipe();
	afx_msg void OnBnClickedSaveZone();
	afx_msg void OnBnClickedCloseZone();
	afx_msg void OnEnUpdateZone1Distance();
	afx_msg void OnEnUpdateZone2Distance();
	afx_msg void OnEnUpdateZone3Distance();
	afx_msg void OnEnUpdateZone4Distance();
	afx_msg void OnEnUpdateZone5Distance();
	afx_msg void OnEnUpdateZone6Distance();
	afx_msg void OnEnUpdateZone7Distance();
	afx_msg void OnEnUpdateZone8Distance();
	afx_msg void OnEnUpdateZone9Distance();
	afx_msg void OnEnUpdateZone10Distance();
	afx_msg void OnEnUpdateZone11Distance();
	afx_msg void OnEnUpdateZone12Distance();
	afx_msg void OnEnUpdateZone13Distance();
	afx_msg void OnEnUpdateZone14Distance();
	afx_msg void OnEnUpdateZone15Distance();
	afx_msg void OnEnUpdateZone16Distance();
	afx_msg void OnEnUpdateZone17Distance();
	afx_msg void OnEnUpdateZone18Distance();
	afx_msg void OnEnUpdateZone19Distance();
	afx_msg void OnEnUpdateZone20Distance();
	afx_msg void OnEnUpdateZone21Distance();
	afx_msg void OnEnUpdateZone22Distance();
	afx_msg void OnEnUpdateZone23Distance();
	afx_msg void OnEnUpdateZone24Distance();
	afx_msg void OnEnUpdateZone25Distance();
	afx_msg void OnEnUpdateZone26Distance();
	afx_msg void OnEnUpdateZone27Distance();
	afx_msg void OnEnUpdateZone28Distance();
	afx_msg void OnEnUpdateZone29Distance();
	afx_msg void OnEnUpdateZone30Distance();
	afx_msg void OnEnUpdateZone31Distance();
	afx_msg void OnEnUpdateZone32Distance();

	afx_msg void OnEnUpdateZone1Speed();
	afx_msg void OnEnUpdateZone2Speed();
	afx_msg void OnEnUpdateZone3Speed();
	afx_msg void OnEnUpdateZone4Speed();
	afx_msg void OnEnUpdateZone5Speed();
	afx_msg void OnEnUpdateZone6Speed();
	afx_msg void OnEnUpdateZone7Speed();
	afx_msg void OnEnUpdateZone8Speed();
	afx_msg void OnEnUpdateZone9Speed();
	afx_msg void OnEnUpdateZone10Speed();
	afx_msg void OnEnUpdateZone11Speed();
	afx_msg void OnEnUpdateZone12Speed();
	afx_msg void OnEnUpdateZone13Speed();
	afx_msg void OnEnUpdateZone14Speed();
	afx_msg void OnEnUpdateZone15Speed();
	afx_msg void OnEnUpdateZone16Speed();
	afx_msg void OnEnUpdateZone17Speed();
	afx_msg void OnEnUpdateZone18Speed();
	afx_msg void OnEnUpdateZone19Speed();
	afx_msg void OnEnUpdateZone20Speed();
	afx_msg void OnEnUpdateZone21Speed();
	afx_msg void OnEnUpdateZone22Speed();
	afx_msg void OnEnUpdateZone23Speed();
	afx_msg void OnEnUpdateZone24Speed();
	afx_msg void OnEnUpdateZone25Speed();
	afx_msg void OnEnUpdateZone26Speed();
	afx_msg void OnEnUpdateZone27Speed();
	afx_msg void OnEnUpdateZone28Speed();
	afx_msg void OnEnUpdateZone29Speed();
	afx_msg void OnEnUpdateZone30Speed();
	afx_msg void OnEnUpdateZone31Speed();
	afx_msg void OnEnUpdateZone32Speed();

	afx_msg void OnEnUpdateZone1Temp();
	afx_msg void OnEnUpdateZone2Temp();
	afx_msg void OnEnUpdateZone3Temp();
	afx_msg void OnEnUpdateZone4Temp();
	afx_msg void OnEnUpdateZone5Temp();
	afx_msg void OnEnUpdateZone6Temp();
	afx_msg void OnEnUpdateZone7Temp();
	afx_msg void OnEnUpdateZone8Temp();
	afx_msg void OnEnUpdateZone9Temp();
	afx_msg void OnEnUpdateZone10Temp();
	afx_msg void OnEnUpdateZone11Temp();
	afx_msg void OnEnUpdateZone12Temp();
	afx_msg void OnEnUpdateZone13Temp();
	afx_msg void OnEnUpdateZone14Temp();
	afx_msg void OnEnUpdateZone15Temp();
	afx_msg void OnEnUpdateZone16Temp();
	afx_msg void OnEnUpdateZone17Temp();
	afx_msg void OnEnUpdateZone18Temp();
	afx_msg void OnEnUpdateZone19Temp();
	afx_msg void OnEnUpdateZone20Temp();
	afx_msg void OnEnUpdateZone21Temp();
	afx_msg void OnEnUpdateZone22Temp();
	afx_msg void OnEnUpdateZone23Temp();
	afx_msg void OnEnUpdateZone24Temp();
	afx_msg void OnEnUpdateZone25Temp();
	afx_msg void OnEnUpdateZone26Temp();
	afx_msg void OnEnUpdateZone27Temp();
	afx_msg void OnEnUpdateZone28Temp();
	afx_msg void OnEnUpdateZone29Temp();
	afx_msg void OnEnUpdateZone30Temp();
	afx_msg void OnEnUpdateZone31Temp();
	afx_msg void OnEnUpdateZone32Temp();

	afx_msg void OnEnUpdateStartPos();
	afx_msg void OnEnUpdateEndPos();
};
