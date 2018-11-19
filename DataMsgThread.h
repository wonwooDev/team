#if !defined(AFX_DATAMSGTHREAD_H__A67949D1_2FFB_4109_B5CF_C50BBF3FA2A0__INCLUDED_)
#define AFX_DATAMSGTHREAD_H__A67949D1_2FFB_4109_B5CF_C50BBF3FA2A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataMsgThread.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////

extern CPyroSoftMApp	theApp;

#define WM_EndDataMsgThread	WM_USER + 100
#define WM_IRDXDataMsgThread WM_USER + 150
#define WM_PropertyUpdateThread WM_USER + 200

/////////////////////////////////////////////////////////////////////////////
// Thread CDataMsgThread 

class CDataMsgThread : public CWinThread
{
	DECLARE_DYNCREATE(CDataMsgThread)
protected:
	CDataMsgThread();           // Dynamische Erstellung verwendet gesch?zten Konstruktor

// Attribute
public:

// Operationen
public:

// ?erschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions?erschreibungen
	//{{AFX_VIRTUAL(CDataMsgThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CDataMsgThread();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDataMsgThread)
		// HINWEIS - Der Klassen-Assistent f?t hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG
	afx_msg void OnDDAQDataMsg(UINT wParam, LONG lParam);
	afx_msg void OnEndDataMsgThread(UINT wParam, LONG lParam);
	//afx_msg void OnPropertyUpdateThread(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f?t unmittelbar vor der vorhergehenden Zeile zus?zliche Deklarationen ein.

#endif // AFX_DATAMSGTHREAD_H__A67949D1_2FFB_4109_B5CF_C50BBF3FA2A0__INCLUDED_
