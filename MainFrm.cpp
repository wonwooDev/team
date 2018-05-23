
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "PyroSoftM.h"

#include "MainFrm.h"
#include "GlassFlowView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_WM_SETTINGCHANGE()
	ON_MESSAGE(DDAQ_WM_DATAMSG, OnDDAQDataMsg)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	m_pCurrentDoc = NULL;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

//	SetMenu(NULL);

	BOOL bNameValid;

	// 모든 사용자 인터페이스 요소를 그리는 데 사용하는 비주얼 관리자를 설정합니다.
	//CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
	
	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 사용할 수 있는 다른 스타일...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bTabIcons = FALSE;    // TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor = TRUE;    // FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu = TRUE; // 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	
	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);
	
	//**********************************************************************************************************************
	CString strToolBarName;

	if (!m_StdToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(1, 1, 1, 1), IDR_MAINFRAME_256) ||
		!m_StdToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_StdToolBar.SetWindowText(strToolBarName);	
	
	///
	if (!m_DataAcqToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(1, 1, 1, 1), IDR_DATAACQ_256) ||
		!m_DataAcqToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_DATAACQ_256 : IDR_DATAACQ_256))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_DATAACQ);
	ASSERT(bNameValid);
	m_DataAcqToolBar.SetWindowText(strToolBarName);

	///
	if (!m_DataPalyToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1, 1, 1, 1), IDR_DATAPLAYER_256) ||	
		!m_DataPalyToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_DATAPLAYER_256 : IDR_DATAPLAYER_256))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_DATAPLAY);
	ASSERT(bNameValid);
	m_DataPalyToolBar.SetWindowText(strToolBarName);
		
	m_DataPalyToolBar.SetButtonInfo(1, ID_DP_SLIDER, TBBS_SEPARATOR, 172);
	CRect itemRect;
	m_DataPalyToolBar.GetItemRect(1,&itemRect);

	itemRect.left+=4;
	itemRect.top+=2;
	itemRect.bottom-=4;

    if(!m_DataPlaySlider.Create(WS_CHILD | WS_VISIBLE | SBS_HORZ,itemRect,&m_DataPalyToolBar,ID_DP_SLIDER)) {
		TRACE0("슬라이더를 만들지 못했습니다.\n");
		return -1;      
	}	
	
	m_DataPlaySlider.EnableWindow(0);	

	/// Focus *******************************************************************
	if (!m_FocusToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1, 1, 1, 1), IDR_TOOLBAR_FOCUS) ||
		!m_FocusToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_TOOLBAR_FOCUS : IDR_TOOLBAR_FOCUS))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_FOCUS);
	ASSERT(bNameValid);
	m_FocusToolBar.SetWindowText(strToolBarName);	


	/// ROI & REF *******************************************************************
	if (!m_ROIREFToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1, 1, 1, 1), IDR_ROI_REF) ||
		!m_ROIREFToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_ROI_REF : IDR_ROI_REF))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_ROIREF);
	ASSERT(bNameValid);
	m_ROIREFToolBar.SetWindowText(strToolBarName);
		
	//**********************************************************************************************************************

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	
	//m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);	
	m_StdToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_DataAcqToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_DataPalyToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_FocusToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_ROIREFToolBar.EnableDocking(CBRS_ALIGN_ANY);
	
	EnableDocking(CBRS_ALIGN_ANY);

	DockPane(&m_wndMenuBar);

	DockPane(&m_StdToolBar);		
	DockPane(&m_DataAcqToolBar);
	DockPane(&m_DataPalyToolBar);
	DockPane(&m_FocusToolBar);
	DockPane(&m_ROIREFToolBar);

	///m_wndMenuBar.ShowWindow(0);
	
	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}
		
	//m_wndScale.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_LEFT);
	m_wndLogo.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_TOP);
	m_wndProperties.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	//m_wndScale.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);

	DockPane(&m_wndProperties);
	//DockPane(&m_wndScale);	
	DockPane(&m_wndLogo);
	
	// 향상된 창 관리 대화 상자를 활성화합니다.
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 사용자 정의 도구 모음 이미지를 로드합니다.
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}
	
	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	//lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);	
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);

	//CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// 창 제목 표시줄에서 문서 이름 및 응용 프로그램 이름의 순서를 전환합니다.
	// 문서 이름이 축소판 그림과 함께 표시되므로 작업 표시줄의 기능성이 개선됩니다.
	//ModifyStyle(0, FWS_PREFIXTITLE);
	this->SetWindowText("RHK Thermal Imaging System");

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//cs.style &= FWS_ADDTOTITLE;
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	
	// 속성 창을 만듭니다.
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 250, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 스케일 창을 만듭니다.
	//CString strOutputWnd;
	//bNameValid = strOutputWnd.LoadString(IDS_SCALE_WND);
	//ASSERT(bNameValid);
	//if (!m_wndScale.Create(strOutputWnd, this, CRect(0, 0, 50, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("출력 창을 만들지 못했습니다.\n");
	//	return FALSE; // 만들지 못했습니다.
	//}
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_LOGO_WND);
	ASSERT(bNameValid);
	if (!m_wndLogo.Create(strOutputWnd, this, CRect(0, 0, 50, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	
	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndScale.SetIcon(hOutputBarIcon, FALSE);
	m_wndLogo.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMDIFrameWndEx::OnShowWindow(bShow, nStatus);

	m_pUserToolbar->ResetAll();

	//m_wndScale.DockToFrameWindow(CBRS_ALIGN_LEFT);
	m_wndLogo.DockToFrameWindow(CBRS_ALIGN_TOP);
	m_wndProperties.DockToFrameWindow(CBRS_ALIGN_LEFT);
}


LONG CMainFrame::OnDDAQDataMsg(UINT wParam, LONG )
{
	CPyroSoftMDoc*	pDoc = theApp.GetDocumentFromDevice(wParam);
	if (pDoc)
		pDoc->OnNewDataReady();

	return 0;
}


void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	m_pUserToolbar = pUserToolbar;

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		
	return lres;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 기본 클래스가 실제 작업을 수행합니다.

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	//
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetStatusText(const char* s)
{
	m_wndStatusBar.SetPaneText(0, s);
}
/////////////////////////////////////////////////////////////////////////////

BOOL CMainFrame::DestroyWindow()
{
	theApp.EndDataMsgThread();

	return CMDIFrameWndEx::DestroyWindow();
}

CPyroSoftMDoc* CMainFrame::GetDocument()
{ 
	//*
	CPyroSoftMDoc *pDoc = NULL;

	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

	if(pFrame->GetSafeHwnd() == NULL) {	
		return (CPyroSoftMDoc*) pDoc;
	}

	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	if(pChild->GetSafeHwnd() == NULL) {	
		return (CPyroSoftMDoc*) pDoc;
	}

	pDoc = (CPyroSoftMDoc *)pChild->GetActiveDocument();

	
	return (CPyroSoftMDoc*) pDoc;	
	//*/ 
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{	
	bool ret;

	if (wParam==ID_DP_SLIDER && !m_bNoNotify)
	{ 
		NMHDR* pnm = (NMHDR*)lParam;		
		
		if(pnm->code==NM_CUSTOMDRAW) 
		{
			CPyroSoftMDoc *pDoc = m_pCurrentDoc;
			

			if(pDoc==NULL) 
				return CMDIFrameWndEx::OnNotify(wParam, lParam, pResult);
						

			if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
				return CMDIFrameWndEx::OnNotify(wParam, lParam, pResult);
			
			unsigned long pos = m_DataPlaySlider.GetPos();

			if(pos!=pDoc->m_IdxDS && pDoc->m_NoDS>1 && pos < pDoc->m_NoDS) 
			{
				ret = theApp.DDAQ_IRDX_FILE_SetCurDataSet(pDoc->m_hIRDX_Doc, pos);

				if(ret) {
					pDoc->UpdateAllViews(NULL, 1);
					theApp.m_pPropertyWnd->UpdateDataSet();		
				}
				else {
					int aaaa = 100;
				}
			}
		}
	}
	
	return CMDIFrameWndEx::OnNotify(wParam, lParam, pResult);
}

LRESULT CMainFrame::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	::PostMessage(m_wndProperties.m_hWnd, AFX_WM_PROPERTY_CHANGED, (WPARAM) wparam, (LPARAM) lparam);

	return 0;
}

void CMainFrame::LocateToolBar(CMainFrame *pFW, CMFCToolBar *pBar1, CMFCToolBar *pBar2)
{ 
	int n=0;
	CRect rect;

	pFW->RecalcLayout();
	pBar1->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	
	/*
	dw=pBar1->GetBarStyle();

	n = (dw & CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw & CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw & CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw & CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	*/

	//pFW->DockControlBar(pBar2, n, &rect);
	pFW->DockPane(pBar2, CBRS_ALIGN_LEFT, &rect);
}




void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (IDNO == AfxMessageBox("프로그램을 종료 하시겠습니까?", MB_YESNO))
		return;
	else
	{
		if (theApp.m_bLoggingRunning)
		{
			CPyroSoftMDoc *pDoc = m_pCurrentDoc;

			pDoc->OnDeviceLoggingStop();

			Sleep(200);
		}
	}


	CMDIFrameWndEx::OnClose();
}
