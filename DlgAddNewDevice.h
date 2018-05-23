////////////////////////////////////////////////////////////////////////////
/** @file DlgAddNewDevice.h interface of the CDlgAddNewDevice class
*/
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGADDNEWDEVICE_H__A17732E2_00C0_459B_92DB_DBE191B9D50C__INCLUDED_)
#define AFX_DLGADDNEWDEVICE_H__A17732E2_00C0_459B_92DB_DBE191B9D50C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddNewDevice.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgAddNewDevice 

class CDlgAddNewDevice : public CDialog
{
// Konstruktion
public:
	CDlgAddNewDevice(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDlgAddNewDevice)
	enum { IDD = IDD_ADD_NEW_DEVICE };
	int				m_SelDevice;
	int				m_SelInterface;
	CString	m_TemplateFile;
	BOOL	m_UseTemplate;
	//}}AFX_DATA
	CStringArray	m_DeviceStrings;
	CStringArray	m_InterfaceStrings;

// ?erschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions?erschreibungen
	//{{AFX_VIRTUAL(CDlgAddNewDevice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst?zung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgAddNewDevice)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTempl();
	afx_msg void OnCheckTempl();
	afx_msg void OnSelchangeComboDevices();
	afx_msg void OnSelchangeComboInterface();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f?t unmittelbar vor der vorhergehenden Zeile zus?zliche Deklarationen ein.

#endif // AFX_DLGADDNEWDEVICE_H__A17732E2_00C0_459B_92DB_DBE191B9D50C__INCLUDED_
