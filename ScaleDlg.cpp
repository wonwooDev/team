// ScaleDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PyroSoftM.h"
#include "ScaleDlg.h"
#include "afxdialogex.h"

#include "GlassFlowView.h"


// CScaleDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CScaleDlg, CDialogEx)

CScaleDlg::CScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScaleDlg::IDD, pParent)
{

}

CScaleDlg::~CScaleDlg()
{
}

CPyroSoftMDoc* CScaleDlg::GetDocument()
{ 
	CPyroSoftMDoc *pDoc = NULL;

	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

	if(pFrame->GetSafeHwnd() == NULL) {	
		return (CPyroSoftMDoc*) pDoc;
	}

	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	
	if(pChild->GetSafeHwnd() != NULL) {	
		pDoc = (CPyroSoftMDoc *)pChild->GetActiveDocument();
	}

	return (CPyroSoftMDoc*) pDoc;	
}

void CScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);	
}


BEGIN_MESSAGE_MAP(CScaleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CScaleDlg 메시지 처리기입니다.
/////////////////////////////////////////////////////////////////////////////

void CScaleDlg::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{	
	Invalidate(lHint == 0);
}

void CScaleDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
		
	CPyroSoftMDoc* pDoc = GetDocument();

	if(pDoc==NULL) return;

	// return if document is not ready now
	if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
		return;

	void*		pBits;
	BITMAPINFO* pBitmapInfo;

	CRect cr;
	GetClientRect(cr);
		
	if (!theApp.DDAQ_IRDX_PALLET_GetBitmapScale(pDoc->m_hIRDX_Doc, cr.Width(), cr.Height(), &pBits, &pBitmapInfo))
		return;
	
	
	//호환성 있는 메모리 DC 생성
    CDC BufferDC;
    BufferDC.CreateCompatibleDC(&dc);    
	
    // 호환성 있는 메모리 비트맵 생성
    CBitmap bmpBuffer;
    bmpBuffer.CreateCompatibleBitmap(&dc, cr.Width(), cr.Height());
	
    // 메모리 비트맵 선택
    CBitmap *pOB1 = (CBitmap*)BufferDC.SelectObject(&bmpBuffer);
	
	::SetDIBitsToDevice(BufferDC.m_hDC, 
						0, 0, cr.Width(), cr.Height(), 
						0, 0, 0, cr.Height(), 
						pBits, pBitmapInfo, 0); 

 
    // 메모리 비트맵에 그려진 내용을 화면으로 전송
    dc.BitBlt(0, 0, cr.Width(), cr.Height(), &BufferDC, 0, 0, SRCCOPY);
	 
    BufferDC.SelectObject(pOB1);    
}


void CScaleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	Invalidate();
}


BOOL CScaleDlg::OnEraseBkgnd(CDC* pDC)
{
	CPyroSoftMDoc* pDoc = GetDocument();

	if(pDoc==NULL) 
		return CDialogEx::OnEraseBkgnd(pDC);

	// return if document is not ready now
	if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
		return CDialogEx::OnEraseBkgnd(pDC);
	
	return FALSE;
}
