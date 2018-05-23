
#include "stdafx.h"

#include "ScaleWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "PyroSoftM.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

CScaleWnd::CScaleWnd()
{
}

CScaleWnd::~CScaleWnd()
{
}

BEGIN_MESSAGE_MAP(CScaleWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CScaleWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
		
	if(m_ScaleDlg.GetSafeHwnd() == 0)
	{		
		if(!m_ScaleDlg.Create(IDD_SCALE, this))
			return FALSE;	
		
		m_ScaleDlg.ModifyStyle(0, WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0);
				
		theApp.m_pScaleDlg = &m_ScaleDlg;

		m_ScaleDlg.InvalidateRect(NULL, TRUE);

		AdjustLayout();
	}	

	return 0;
}

void CScaleWnd::AdjustLayout()
{	
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_ScaleDlg.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CScaleWnd::OnSize(UINT nType, int cx, int cy)
{
	if(GetSafeHwnd()==NULL) return;

	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

