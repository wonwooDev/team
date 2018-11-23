
// ChildFrm.cpp : CChildFrame 클래스의 구현
//

#include "stdafx.h"
#include "PyroSoftM.h"

#include "GlassFlowView.h"
#include "ResultView.h"
#include "ROIGridView.h"
#include "ScaleView.h"

#include "MainFrm.h"
#include "ChildFrm.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_WM_CHILDACTIVATE()
	ON_WM_HSCROLL()
	ON_WM_MDIACTIVATE()
	ON_WM_MDIACTIVATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CChildFrame 생성/소멸

CChildFrame::CChildFrame()
{
	bSplitterCreaged = false;

	//////////////////////////////////////
	theApp.nResultDlgWidth = 570;
	theApp.nStatusDlgHeight = 130;
	theApp.nScaleDlgWidth = theApp.nResultDlgWidth + 100;
}

CChildFrame::~CChildFrame()
{

}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서 Window 클래스 또는 스타일을 수정합니다.
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	

	return TRUE;
}

// CChildFrame 진단

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 메시지 처리기


BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if (theApp.IRDX_Mode < 1) {
		return CMDIChildWndEx::OnCreateClient(lpcs, pContext);
	}
	else {
		//if (!theApp.m_wndSplitter.CreateStatic(this, 1, 2)){
		if (!m_wndSplitter.CreateStatic(this, 1, 2)) {
			TRACE("Failed to CreateStaticSplitter\n");
			return FALSE;
		}

		//if (!theApp.m_GFVSplitter.CreateStatic(&theApp.m_wndSplitter, 2, 1)){
		if (!m_GFVSplitter.CreateStatic(&m_wndSplitter, 2, 1)) {
			TRACE("Failed to CreateStaticSplitter\n");
			return FALSE;
		}

		//if (!theApp.m_ScaleSplitter.CreateStatic(&theApp.m_GFVSplitter, 1, 2)){
		if (!m_ScaleSplitter.CreateStatic(&m_GFVSplitter, 1, 2)) {
			TRACE("Failed to CreateStaticSplitter\n");
			return FALSE;
		}

		// pane 1
		//if (!theApp.m_ScaleSplitter.CreateView(0, 0, RUNTIME_CLASS(CGlassFlowView), CSize(100, 100), pContext))
		if (!m_ScaleSplitter.CreateView(0, 0, RUNTIME_CLASS(CGlassFlowView), CSize(100, 100), pContext))
			return FALSE;

		// pane 2
		//if (!theApp.m_ScaleSplitter.CreateView(0, 1, RUNTIME_CLASS(CScaleView), CSize(10, 10), pContext))
		if (!m_ScaleSplitter.CreateView(0, 1, RUNTIME_CLASS(CScaleView), CSize(10, 10), pContext))
			return FALSE;

		// pane 3
		//if (!theApp.m_GFVSplitter.CreateView(1, 0, RUNTIME_CLASS(CROIGridView), CSize(250, 200), pContext))
		if (!m_GFVSplitter.CreateView(1, 0, RUNTIME_CLASS(CROIGridView), CSize(250, 200), pContext))
			return FALSE;

		// pane 4
		//if (!theApp.m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CResultView), CSize(250, 200), pContext))
		if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CResultView), CSize(250, 200), pContext))
			return FALSE;

		bSplitterCreaged = true;
	}

	return TRUE;

	//return CMDIChildWndEx::OnCreateClient(lpcs, pContext);
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bActivate) {
		CPyroSoftMDoc * pDoc = (CPyroSoftMDoc *) GetActiveDocument();
		
		theApp.m_pPropertyWnd->ChangeProperties(pDoc);
		
		/////////////////////////////////////////////////////////////////////
		unsigned long pos;
		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

		pFrame->m_pCurrentDoc = pDoc;

		if(pDoc->m_OpenMode>1 || pDoc->m_NoDS<=1) 
		{
			pFrame->m_DataPlaySlider.EnableWindow(0);
			pFrame->m_DataPlaySlider.SetRange(0, 1, FALSE);
			pFrame->m_DataPlaySlider.SetPos(0);
		}
		else 
		{
			pFrame->m_DataPlaySlider.EnableWindow(1);
		
			pos = pFrame->m_DataPlaySlider.GetPos();

			if(pos > pDoc->m_NoDS-1) {
				pFrame->m_bNoNotify = true;
				pFrame->m_DataPlaySlider.SetPos(pDoc->m_IdxDS);
				pFrame->m_DataPlaySlider.SetRange(0, pDoc->m_NoDS-1, TRUE);
				pFrame->m_bNoNotify = false;
			}
			else {
				pFrame->m_bNoNotify = true;
				pFrame->m_DataPlaySlider.SetRange(0, pDoc->m_NoDS-1, TRUE);
				pFrame->m_DataPlaySlider.SetPos(pDoc->m_IdxDS);
				pFrame->m_bNoNotify = false;
			}
		}
		/////////////////////////////////////////////////////////////////////	
		//CWnd *pWnd = m_GFVSplitter.GetPane(1, 0);
		//CGlassFlowView *pView = DYNAMIC_DOWNCAST(CGlassFlowView, pWnd);

		//SetActiveView(pView);

	}
}


void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWndEx::OnSize(nType, cx, cy);
		
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rect;
	int tempColInf = 0, tempMinInf;
	GetClientRect(&rect);

	if(bSplitterCreaged && theApp.adjust_count < 4)
	{
		theApp.adjust_count++;

		if(rect.Height()==0) return;
		if (rect.Width() == 0) return;

		theApp.m_childFrmWidth = rect.Width();
		theApp.m_childFrmHeight = rect.Height();
		
		int nLeft = rect.Width() - theApp.nResultDlgWidth - 20;

		if (nLeft <= 0) return;

		//theApp.m_wndSplitter.SetColumnInfo(0, nLeft, 100);
		m_wndSplitter.SetColumnInfo(0, nLeft, 100);
		//theApp.m_wndSplitter.SetColumnInfo(1, theApp.nResultDlgWidth, 100);
		m_wndSplitter.SetColumnInfo(1, theApp.nResultDlgWidth, 100);
		//theApp.m_wndSplitter.RecalcLayout();
		m_wndSplitter.RecalcLayout();

		//int nTop = rect.Height() - theApp.nStatusDlgHeight - 20;
		int nTop = rect.Height() - theApp.nStatusDlgHeight + 30;
		if (nTop <= 0) return;

		//theApp.m_GFVSplitter.SetRowInfo(0, nTop, 50);
		m_GFVSplitter.SetRowInfo(0, nTop, 50);
		//theApp.m_GFVSplitter.SetRowInfo(1, theApp.nStatusDlgHeight, 50);
		m_GFVSplitter.SetRowInfo(1, theApp.nStatusDlgHeight, 50);
		//theApp.m_GFVSplitter.RecalcLayout();
		m_GFVSplitter.RecalcLayout();

		int nScaleLeft = rect.Width() - theApp.nScaleDlgWidth - 20;

		if(nScaleLeft <= 0) return;
		//theApp.m_ScaleSplitter.SetColumnInfo(0, nScaleLeft, 20);
		m_ScaleSplitter.SetColumnInfo(0, nScaleLeft, 20);
		//theApp.m_ScaleSplitter.SetColumnInfo(1, theApp.nScaleDlgWidth, 20);
		m_ScaleSplitter.SetColumnInfo(1, theApp.nScaleDlgWidth, 20);
		
		//theApp.m_ScaleSplitter.RecalcLayout();
		m_ScaleSplitter.RecalcLayout();
	}	
}


void CChildFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPyroSoftMDoc * pDoc = (CPyroSoftMDoc *)GetActiveDocument();

	if (MessageBox(_T("Do you want to exit?"), _T("Exitting Application"), MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		if (pDoc->m_OpenMode == 1)
		{
			theApp.m_bFileOpen = false;

			CMDIChildWndEx::OnClose();
		}
		else
		{
			pDoc->OnDeviceDoStop();
			pDoc->OnDeviceLoggingStop();
			CMDIChildWndEx::OnClose();
		}
	}
	else
	{
		return;
	}
}
