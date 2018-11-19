// DataMsgThread.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PyroSoftM.h"
#include "PyroSoftMDoc.h"
#include "DataMsgThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataMsgThread

IMPLEMENT_DYNCREATE(CDataMsgThread, CWinThread)

CDataMsgThread::CDataMsgThread()
{
}

CDataMsgThread::~CDataMsgThread()
{
}

BOOL CDataMsgThread::InitInstance()
{
	// ZU ERLEDIGEN:  Initialisierungen f? jeden Thread hier durchf?ren
	return TRUE;
}

int CDataMsgThread::ExitInstance()
{
	// ZU ERLEDIGEN:  Bereinigungen f? jeden Thread hier durchf?ren
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CDataMsgThread, CWinThread)
	//{{AFX_MSG_MAP(CDataMsgThread)
		// HINWEIS - Der Klassen-Assistent f?t hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
	//ON_THREAD_MESSAGE(WM_IRDXDataMsgThread, )
	ON_THREAD_MESSAGE(DDAQ_WM_DATAMSG, OnDDAQDataMsg)
	ON_THREAD_MESSAGE(WM_EndDataMsgThread, OnEndDataMsgThread)
	//ON_THREAD_MESSAGE(WM_PropertyUpdateThread, OnPropertyUpdateThread)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen f? Nachrichten CDataMsgThread 

void CDataMsgThread::OnDDAQDataMsg(UINT wParam, LONG lParam)
{
	// beacause a thread message can not access the update function from a window
	// it's necessary to post a windows message here!
	::PostMessage(AfxGetMainWnd()->m_hWnd, DDAQ_WM_DATAMSG, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////

void CDataMsgThread::OnEndDataMsgThread(UINT wParam, LONG lParam)
{
	ExitInstance();
	AfxEndThread(0);
}
/////////////////////////////////////////////////////////////////////////////

//void CDataMsgThread::OnPropertyUpdateThread(UINT wParam, LONG lParam)
//{
//	::PostMessage(AfxGetMainWnd()->m_hWnd, DDAQ_WM_DATAMSG, wParam, lParam);
//}