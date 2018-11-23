#if !defined(AFX_DLGSCREENPERIOD_H__58C6DF5B_58E7_4958_AA63_A5999182589B__INCLUDED_)
#define AFX_DLGSCREENPERIOD_H__58C6DF5B_58E7_4958_AA63_A5999182589B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgscreenperiod.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgScreenPeriod dialog

class CDlgScreenPeriod : public CDialog
{
// Construction
public:
	CDlgScreenPeriod(CWnd* pParent = NULL);   // standard constructor

protected:
// Dialog Data
	//{{AFX_DATA(CDlgScreenPeriod)
	enum { IDD = IDD_SCREEN_PERIOD };
	long	m_lScreenPeriod;
	long	m_lPoolingPeriod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScreenPeriod)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
  UINT GetScreenPeriod();
  void SetScreenPeriod(UINT uScreenPeriod);

  UINT GetPoolingPeriod();
  void SetPoolingPeriod(UINT uPoolingPeriod);

  void SetPeriods(UINT uScreenPeriod,UINT uPoolingPeriod);
  void GetPeriods(UINT &uScreenPeriod,UINT &uPoolingPeriod);

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScreenPeriod)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCREENPERIOD_H__58C6DF5B_58E7_4958_AA63_A5999182589B__INCLUDED_)
