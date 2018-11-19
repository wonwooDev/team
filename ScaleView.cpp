// ScaleView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ScaleView.h"
#include "PyroSoftM.h"


// CScaleView

IMPLEMENT_DYNCREATE(CScaleView, CView)

CScaleView::CScaleView()
{
	m_scaleSizeFlag = false;
	m_scaleSizeCount = 0;
}

CScaleView::~CScaleView()
{
}

BEGIN_MESSAGE_MAP(CScaleView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CScaleView 그리기입니다.

void CScaleView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.

}

// CScaleView 진단입니다.
#ifdef _DEBUG
void CScaleView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CScaleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CScaleView 메시지 처리기입니다.


int CScaleView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	if (m_ScaleDlg.GetSafeHwnd() == 0)
	{
		if (!m_ScaleDlg.Create(IDD_SCALE, this))
			return FALSE;

		m_ScaleDlg.ModifyStyle(0, WS_VISIBLE, 0);

		theApp.m_pScaleDlg = &m_ScaleDlg;

		AdjustLayout();
	}

	return 0;
}

void CScaleView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int dlgH = 0;

	//if (m_scaleSizeFlag && !theApp.m_resultDlgMove)
	//{
	//	theApp.nScaleDlgWidth = rectClient.Width() + theApp.nResultDlgWidth;
	//	theApp.m_resultDlgMove = true;
	//}

	//if (theApp.adjust_count == 5)
	//{
	//	m_scaleSizeCount++;
	//	if (m_scaleSizeCount == 2)
	//		m_scaleSizeFlag = true;
	//}

	int k = rectClient.left;

	m_ScaleDlg.SetWindowPos(NULL, rectClient.left,
		rectClient.top + dlgH,
		rectClient.Width(),
		rectClient.Height() - dlgH,
		SWP_NOACTIVATE | SWP_NOZORDER);
}


void CScaleView::OnSize(UINT nType, int cx, int cy)
{
	if (GetSafeHwnd() == NULL) return;

	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	AdjustLayout();
}


void CScaleView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}
