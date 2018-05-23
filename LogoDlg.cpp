// LogoDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LogoDlg.h"
#include "afxdialogex.h"


// CLogoDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLogoDlg, CDialogEx)

CLogoDlg::CLogoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LOGO, pParent)
{

}

CLogoDlg::~CLogoDlg()
{
}


void CLogoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_PROGRAM_NAME, m_static_ProgramName);
}


BEGIN_MESSAGE_MAP(CLogoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CLogoDlg 메시지 처리기입니다.

void CLogoDlg::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	Invalidate(lHint == 0);
}

BOOL CLogoDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	HMODULE hInst_samsungLogo = AfxGetInstanceHandle();
	MyLoadFromResource(samsungLogo, (LPCTSTR)MAKEINTRESOURCE(IDB_SAMSUNG_PNG), _T("PNG"), hInst_samsungLogo);
	HMODULE hInst_gtcLogo = AfxGetInstanceHandle();
	MyLoadFromResource(gtcLogo, (LPCTSTR)MAKEINTRESOURCE(IDB_GTC_PNG), _T("PNG"), hInst_gtcLogo);
	HMODULE hInst_programName = AfxGetInstanceHandle();
	MyLoadFromResource(programName, (LPCTSTR)MAKEINTRESOURCE(IDB_PROGRAMNAME_PNG), _T("PNG"), hInst_programName);

	return TRUE;
}


void CLogoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// insert logo =========================================================
	CRect rect;   GetClientRect(&rect);

	float sam_widthRate, sam_heightRate, gtc_widthRate, gtc_heightRate, prog_widthRate, prog_heightRate;

	int samLogoX, samLogoY,GTCLogoX, GTCLogoY, ProgNameX, ProgNameY;
	samLogoX = samsungLogo.GetWidth();
	samLogoY = samsungLogo.GetHeight();
	GTCLogoX = gtcLogo.GetWidth();
	GTCLogoY = gtcLogo.GetHeight();
	ProgNameX = programName.GetWidth();
	ProgNameY = programName.GetHeight();
	
	sam_widthRate = (float)rect.Width() / samLogoX;
	sam_heightRate = (float)rect.Height() / samLogoY;
	gtc_widthRate = (float)rect.Width() / GTCLogoX;;
	gtc_heightRate = (float)rect.Height() / GTCLogoY;
	prog_widthRate = (float)rect.Width() / ProgNameX;
	prog_heightRate = (float)rect.Height() / ProgNameY;

	dlgWidth = rect.Width();
	xPosForStatic = rect.Width() / 3.3;
	heightForStatic = rect.Height();

	if (FAILED(samsungHandle))   TRACE(ssLogoPath);
	samsungLogo.StretchBlt(dc.m_hDC, 15, rect.Height() - rect.Height() * 4 / 5, rect.Width() / sam_widthRate, rect.Height() / sam_heightRate);

	if (FAILED(pnHandle))   TRACE(programNamePath);
	programName.StretchBlt(dc.m_hDC, xPosForStatic - 40, rect.Height() - rect.Height() * 4 / 5, rect.Width() / prog_widthRate, rect.Height() / prog_heightRate);

	if (FAILED(gtcHandle))   TRACE(gtcLogoPath);
	gtcLogo.StretchBlt(dc.m_hDC, rect.Width() - GTCLogoX - 20, rect.Height() - rect.Height() * 9 / 10, rect.Width() / gtc_widthRate, rect.Height() / gtc_heightRate);

}


void CLogoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	Invalidate();
}


BOOL CLogoDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CRect rect;
	GetClientRect(&rect);

	pDC->FillSolidRect(&rect, RGB(255, 255, 255));

	return TRUE;
}



HBRUSH CLogoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


BOOL CLogoDlg::MyLoadFromResource(CImage& img, LPCTSTR pName, LPCTSTR pType, HMODULE hInst) {

	IStream* pStream = NULL;
	HRSRC hResource = ::FindResource(hInst, pName, pType); if (!hResource) return FALSE;
	DWORD nImageSize = ::SizeofResource(hInst, hResource); if (!nImageSize) return FALSE;
	HGLOBAL m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, nImageSize);
	BYTE* pBytes = (BYTE*)::LoadResource(hInst, hResource);

	if (m_hBuffer) {
		void* pSource = ::LockResource(::LoadResource(hInst, hResource));
		if (!pSource) return FALSE;
		void* pDest = ::GlobalLock(m_hBuffer);
		if (pDest) {
			CopyMemory(pDest, pSource, nImageSize);
			if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK) {
				img.Load(pStream); // 여기만 수정 하면
				pStream->Release();
			}
			::GlobalUnlock(m_hBuffer);
		}
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	if (img.IsNull()) return FALSE;
	return TRUE;

}
