
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "PyroSoftM.h"
#include "GlassFlowView.h"

#include <atlconv.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define ID_TIMER	6720
#define	MULTISCALE	1

CString strColorBar[9] = { "SYMIICON", "MULTICOLOR", "SPECTRUM", "THERMO",
"GRAY", "GRAYPLUS", "HOTMETAL", "IRON", "LIGHT", };

CString strZoomMode[12] = { "Auto", "25%", "50%", "100%", "200%", "300%", "400%", "500%", "600%", "800%", "1000%", "1500%" };

CString NoScaleColor[7] = { "256", "128", "63", "31", "21", "15", "11" };
unsigned short  NoScaleColorValue[7] = { 256, 128, 63, 31, 21, 15, 11 };

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_NoDR = 0, m_CurrDR = 0;
	//m_nBar	= 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndPropList.SetLeftColumnWidth(180);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	theApp.m_pPropertyWnd = this;

	NameofOOI[UNDISPLAY] = "Undisplay";
	NameofOOI[SHOW_BASE_ROI] = "Show Base ROI";
	NameofOOI[SHOW_EXCLUSIVE_ROI] = "Show Exclusive ROI";
	NameofOOI[SHOW_ALL_ROI] = "Show Base & Exclusive ROI";

	NameOfPOI[UNDISPLAY] = "Undisplay";
	NameOfPOI[TEMPER_ONLY] = "Temper Only";
	NameOfPOI[NUMBER_ONLY] = "Number Only";
	NameOfPOI[SHOW_ALL] = "Show All";

	InitPropList();
	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.	

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

CPyroSoftMDoc* CPropertiesWnd::GetDocument()
{
	CPyroSoftMDoc *pDoc = NULL;

	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

	if (pFrame->GetSafeHwnd() == NULL) {
		return (CPyroSoftMDoc*)pDoc;
	}

	CMDIChildWnd *pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	if (pChild->GetSafeHwnd() == NULL) {
		return (CPyroSoftMDoc*)pDoc;
	}

	pDoc = (CPyroSoftMDoc *)pChild->GetActiveDocument();

	return (CPyroSoftMDoc*)pDoc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitPropList()
{
	float	fVal = 0;
	unsigned long ULVal = 0;

	m_wndPropList.RemoveAll();

	m_NoDR = 0, m_CurrDR = 0;

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();

	// File ///////////////////////////////////////////////////////////////////////////////////////////////		
	pFile = new CMFCPropertyGridProperty(_T("File"));

	pFile_NoDR = new CMFCPropertyGridProperty(_T("Number of Data Record"), (_variant_t)m_NoDR, _T(""));
	pFile_NoDR->AllowEdit(FALSE);
	pFile->AddSubItem(pFile_NoDR);

	pFile_CurDR = new CMFCPropertyGridProperty(_T("Current Data Record"), (_variant_t)m_CurrDR, _T(""));
	pFile_CurDR->AllowEdit(FALSE);
	pFile->AddSubItem(pFile_CurDR);

	m_wndPropList.AddProperty(pFile);

	// Camera ///////////////////////////////////////////////////////////////////////////////////////////////
	CString str = _T("");

	pMeasureera = new CMFCPropertyGridProperty(_T("Camera"));

	pMeasureera_CType = new CMFCPropertyGridProperty(_T("Camera Type"), (_variant_t)str, _T(""));
	pMeasureera_CType->AllowEdit(FALSE);
	pMeasureera->AddSubItem(pMeasureera_CType);

	pMeasureera_SN = new CMFCPropertyGridProperty(_T("Serial Number"), (_variant_t)str, _T(""));
	pMeasureera_SN->AllowEdit(FALSE);
	pMeasureera->AddSubItem(pMeasureera_SN);

	pMeasureera_DTemp = new CMFCPropertyGridProperty(_T("Detector Temperature"), (_variant_t)str, _T(""));
	pMeasureera_DTemp->AllowEdit(FALSE);
	pMeasureera->AddSubItem(pMeasureera_DTemp);

	pMeasureera_CTemp = new CMFCPropertyGridProperty(_T("Camera Temperature"), (_variant_t)str, _T(""));
	pMeasureera_CTemp->AllowEdit(FALSE);
	pMeasureera->AddSubItem(pMeasureera_CTemp);

	m_wndPropList.AddProperty(pMeasureera);

	// Document Size ///////////////////////////////////////////////////////////////////////////////////////////////
	pDocSize = new CMFCPropertyGridProperty(_T("Document Size"));

	pDocSize_Pixel = new CMFCPropertyGridProperty(_T("Pixel"), (_variant_t)_T(""), _T(""));
	pDocSize_Pixel->AllowEdit(FALSE);
	pDocSize->AddSubItem(pDocSize_Pixel);

	m_wndPropList.AddProperty(pDocSize);

	// Data Acquisition ///////////////////////////////////////////////////////////////////////////////////////////////
	pDataAcq = new CMFCPropertyGridProperty(_T("Data Acquisition"));

	pDataAcq_Date = new CMFCPropertyGridProperty(_T("Date"), (_variant_t)_T(""), _T(""));
	pDataAcq_Date->AllowEdit(FALSE);
	pDataAcq->AddSubItem(pDataAcq_Date);

	pDataAcq_Time = new CMFCPropertyGridProperty(_T("Time"), (_variant_t)_T(""), _T(""));
	pDataAcq_Time->AllowEdit(FALSE);
	pDataAcq->AddSubItem(pDataAcq_Time);

	pDataAcqIndex = new CMFCPropertyGridProperty(_T("Index"), (_variant_t)ULVal, _T(""));
	pDataAcqIndex->AllowEdit(FALSE);
	pDataAcq->AddSubItem(pDataAcqIndex);

	pDataAcq_MeasRange = new CMFCPropertyGridProperty(_T("Measurement Range"), (_variant_t)_T(""), _T(""), 1);
	pDataAcq_MeasRange->AllowEdit(FALSE);
	pDataAcq_MeasRange->Enable(0);
	pDataAcq->AddSubItem(pDataAcq_MeasRange);

	pDataAcq_AcqFreq = new CMFCPropertyGridProperty(_T("Acquisition Frequency"), (_variant_t)_T(""), _T(""), 2);
	pDataAcq_AcqFreq->AllowEdit(FALSE);
	pDataAcq_AcqFreq->Enable(1);
	pDataAcq->AddSubItem(pDataAcq_AcqFreq);

	pDataAcq_LoggingInterval = new CMFCPropertyGridProperty(_T("Logging Interval (msec)"), _T(""));
	pDataAcq_LoggingInterval->AllowEdit(FALSE);
	pDataAcq_LoggingInterval->Enable(1);
	pDataAcq->AddSubItem(pDataAcq_LoggingInterval);

	pDataTriggerFlag = new CCheckBoxPropertyGridProperty(_T("Laser Trigger"), true, (_variant_t)true, _T(""));
	pDataTriggerFlag->Enable(FALSE);
	pDataAcq->AddSubItem(pDataTriggerFlag);
	pDataTriggerFlag->Show(FALSE);

	pDataAcq_Trigger = new CCheckBoxPropertyGridProperty(_T("Near from Dev."), true, (_variant_t)true, _T(""));
	pDataAcq_Trigger->Enable(FALSE);
	pDataAcq->AddSubItem(pDataAcq_Trigger);
	pDataAcq_Trigger->Show(FALSE);

	pDataAcq_TriggerDist = new CMFCPropertyGridProperty(_T("Distance(cm)"), _T(""));
	pDataAcq_TriggerDist->AllowEdit(FALSE);
	pDataAcq->AddSubItem(pDataAcq_TriggerDist);
	pDataAcq_TriggerDist->Show(FALSE);

	m_wndPropList.AddProperty(pDataAcq);

	// Measurement Object ///////////////////////////////////////////////////////////////////////////////////////////////
	pMeasObj = new CMFCPropertyGridProperty(_T("Measurement Object"));

	pMeasObj_Emissivity = new CFLoatPropertyGridProperty(_T("Emissivity"), (_variant_t)fVal);
	pMeasObj_Emissivity->AllowEdit(FALSE);
	pMeasObj->AddSubItem(pMeasObj_Emissivity);

	pMeasObj_Trans = new CFLoatPropertyGridProperty(_T("Transmittance"), (_variant_t)fVal);
	pMeasObj_Trans->AllowEdit(FALSE);
	pMeasObj->AddSubItem(pMeasObj_Trans);

	pMeasObj_AmbTemp = new CFLoatPropertyGridProperty(_T("Ambient Temperature (℃)"), (_variant_t)fVal);
	pMeasObj_AmbTemp->AllowEdit(FALSE);
	pMeasObj->AddSubItem(pMeasObj_AmbTemp);

	m_wndPropList.AddProperty(pMeasObj);

	// Scaling ///////////////////////////////////////////////////////////////////////////////////////////////
	pScale = new CMFCPropertyGridProperty(_T("Scaling"));

	// ColorBar type
	pColorBar = new CMFCPropertyGridProperty(_T("Color Bar"), (_variant_t)strColorBar[MULTISCALE], _T(""));

	for (int i = 0; i<9; i++) {
		pColorBar->AddOption(strColorBar[i]);
	}
	pColorBar->AllowEdit(FALSE);
	pColorBar->Enable(0);
	pScale->AddSubItem(pColorBar);

	// Number of Color
	pNoColor = new CMFCPropertyGridProperty(_T("Number of Colors"), (_variant_t)NoScaleColor[0], _T(""));

	for (int i = 0; i<7; i++) {
		pNoColor->AddOption(NoScaleColor[i]);
	}

	pNoColor->AllowEdit(FALSE);
	pNoColor->Enable(0);
	pScale->AddSubItem(pNoColor);

	pScale_MaxTemp = new CFLoatPropertyGridProperty(_T("Maximum (℃)"), (_variant_t) 0.0f);
	pScale_MaxTemp->AllowEdit(FALSE);
	pScale->AddSubItem(pScale_MaxTemp);

	pScale_MinTemp = new CFLoatPropertyGridProperty(_T("Minimum (℃)"), (_variant_t) 100.f);
	pScale_MinTemp->AllowEdit(FALSE);
	pScale->AddSubItem(pScale_MinTemp);

	pScale_IsoTherm = new CFLoatPropertyGridProperty(_T("Iso Therm (℃)"), (_variant_t) 100.f);
	pScale_IsoTherm->AllowEdit(FALSE);
	pScale->AddSubItem(pScale_IsoTherm);

	m_wndPropList.AddProperty(pScale);

	// Focus ///////////////////////////////////////////////////////////////////////////////////////////////
	pFocus = new CMFCPropertyGridProperty(_T("Focus"));

	pFocusPosition = new CMFCPropertyGridProperty(_T("Position"), (_variant_t)_T(""), _T(""));
	pFocusPosition->AllowEdit(FALSE);
	pFocusPosition->Enable(0);
	pFocus->AddSubItem(pFocusPosition);

	m_wndPropList.AddProperty(pFocus);

	// Measurement Data ///////////////////////////////////////////////////////////////////////////////////////////////
	pMeasureData = new CMFCPropertyGridProperty(_T("Measurement Info"));

	pMeasureDataZoneInfoEnable = new CCheckBoxPropertyGridProperty(_T("Zone Info Enable"), true, (_variant_t)true, _T(""));
	pMeasureDataZoneInfoEnable->Enable(FALSE);
	pMeasureData->AddSubItem(pMeasureDataZoneInfoEnable);

	pMeasureDataThreshold = new CFLoatPropertyGridProperty(_T("Threshold (℃)"), (_variant_t) 0.0f);
	pMeasureDataThreshold->AllowEdit(FALSE);
	pMeasureData->AddSubItem(pMeasureDataThreshold);

	pMeasureROIBuffer = new CMFCPropertyGridProperty(_T("ROI Buffer "), _T(""));
	pMeasureROIBuffer->AllowEdit(FALSE);
	pMeasureData->AddSubItem(pMeasureROIBuffer);

	pMeasureROIBufferCnt = new CMFCPropertyGridProperty(_T("Catch ROI Num "), _T(""));
	pMeasureROIBufferCnt->AllowEdit(FALSE);
	pMeasureData->AddSubItem(pMeasureROIBufferCnt);

	pMeasureDataROICount = new CMFCPropertyGridProperty(_T("Total ROI Count "), _T(""));
	pMeasureDataROICount->AllowEdit(FALSE);
	pMeasureData->AddSubItem(pMeasureDataROICount);

	pMeasureDataBaseROISize = new CMFCPropertyGridProperty(_T("Object ROI Minimun Size (pixel) "), _T(""));
	pMeasureDataBaseROISize->AllowEdit(FALSE);
	pMeasureData->AddSubItem(pMeasureDataBaseROISize);

	pMeasureDataFontSize = new CMFCPropertyGridProperty(_T("Font Size ()"), _T(""));
	pMeasureDataFontSize->AllowEdit(FALSE);
	pMeasureData->AddSubItem(pMeasureDataFontSize);

	m_wndPropList.AddProperty(pMeasureData);

	pMeasureData->Show(FALSE);

	// Screen ///////////////////////////////////////////////////////////////////////////////////////////////
	pScreen = new CMFCPropertyGridProperty(_T("Screen"));

	pScreenZoom = new CMFCPropertyGridProperty(_T("Zoom In/Out"), (_variant_t)_T("100%"), _T(""));
	pScreenZoom->AllowEdit(FALSE);
	pScreenZoom->Enable(FALSE);
	pScreen->AddSubItem(pScreenZoom);

	pScreenBROI = new CMFCPropertyGridProperty(_T("Object ROI"), (_variant_t)_T(""), _T(""), 2);
	pScreenBROI->AllowEdit(FALSE);
	pScreenBROI->Enable(FALSE);
	pScreen->AddSubItem(pScreenBROI);

	pScreenPOI = new CMFCPropertyGridProperty(_T("Point of Interest"), (_variant_t)_T(""), _T(""), 2);
	pScreenPOI->AllowEdit(FALSE);
	pScreenPOI->Enable(FALSE);
	pScreen->AddSubItem(pScreenPOI);

	pScreenCursor = new CCheckBoxPropertyGridProperty(_T("Cursor Temperature"), true, (_variant_t)true, _T(""));
	pScreenCursor->Enable(FALSE);
	pScreen->AddSubItem(pScreenCursor);

	pScreenROI = new CCheckBoxPropertyGridProperty(_T("ROI Number"), true, (_variant_t)true, _T(""));
	pScreenROI->Enable(FALSE);
	pScreen->AddSubItem(pScreenROI);

	pScreenMaxPointer = new CCheckBoxPropertyGridProperty(_T("Maximun Pointer"), true, (_variant_t)true, _T(""));
	pScreenMaxPointer->Enable(FALSE);
	pScreen->AddSubItem(pScreenMaxPointer);

	pScreenMinPointer = new CCheckBoxPropertyGridProperty(_T("Minimun Pointer"), true, (_variant_t)true, _T(""));
	pScreenMinPointer->Enable(FALSE);
	pScreen->AddSubItem(pScreenMinPointer);

	m_wndPropList.AddProperty(pScreen);

	pScreen->Show(FALSE);

	// Spread Alarm ///////////////////////////////////////////////////////////////////////////////////////////////
	pSpread = new CMFCPropertyGridProperty("Spread Alarm");

	pSpreadDetectRange = new CFLoatPropertyGridProperty(_T("Detect Range (℃)"), (_variant_t) 0.0f);
	pSpreadDetectRange->AllowEdit(FALSE);
	pSpread->AddSubItem(pSpreadDetectRange);

	pSpreadLimitTempr = new CFLoatPropertyGridProperty(_T("Limit Temperature (℃)"), (_variant_t) 0.0f);
	pSpreadLimitTempr->AllowEdit(FALSE);
	pSpread->AddSubItem(pSpreadLimitTempr);

	m_wndPropList.AddProperty(pSpread);

	pSpread->Show(FALSE);


	// Online Data Saving ///////////////////////////////////////////////////////////////////////////////////////////////
	pDataSave = new CMFCPropertyGridProperty(_T("Data Logging"));

	pRawDataSaveEnable = new CCheckBoxPropertyGridProperty(_T("Enable"), true, (_variant_t)true, _T(""));
	pRawDataSaveEnable->Enable(FALSE);
	pDataSave->AddSubItem(pRawDataSaveEnable);

	pRawDataSaveFilePath = new CMFCPropertyGridFileProperty(_T("RawData Location"), _T(""));
	pRawDataSaveFilePath->AllowEdit(FALSE);
	pRawDataSaveFilePath->Enable(FALSE);
	pDataSave->AddSubItem(pRawDataSaveFilePath);

	pResultDataSaveFilePath = new CMFCPropertyGridFileProperty(_T("ResultData Location"), _T(""));
	pResultDataSaveFilePath->AllowEdit(FALSE);
	pResultDataSaveFilePath->Enable(FALSE);
	pDataSave->AddSubItem(pResultDataSaveFilePath);

	m_wndPropList.AddProperty(pDataSave);

	pDataSave->Show(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::ChangeProperties(CPyroSoftMDoc* pDoc)
{
	//CPyroSoftMDoc* pDoc = GetDocument();

	if (pDoc == NULL) return;

	// return if document is not ready now
	if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
		return;

	//=================================================================
	m_NoDR = m_CurrDR = 0;

	theApp.DDAQ_IRDX_FILE_GetNumDataSets(pDoc->m_hIRDX_Doc, &(pDoc->m_NoDS));
	pFile_NoDR->SetValue((_variant_t)(pDoc->m_NoDS));

	theApp.DDAQ_IRDX_FILE_GetCurDataSet(pDoc->m_hIRDX_Doc, &(pDoc->m_IdxDS));
	pFile_CurDR->SetValue((_variant_t)(pDoc->m_IdxDS + 1));

	//=================================================================
	CString strType;
	unsigned long id = 0;
	unsigned long type = 0;

	theApp.DDAQ_IRDX_DEVICE_GetIDString(pDoc->m_hIRDX_Doc, strType);
	theApp.DDAQ_IRDX_DEVICE_GetID(pDoc->m_hIRDX_Doc, &id, &type);

	CString s, s2;
	switch (type)
	{
	case DDAQ_DEVICE_TYPE_VIEW_100:		s2.LoadString(IDS_IDTYPE_VIEW_100);
		break;
	case DDAQ_DEVICE_TYPE_VIEW_256:		s2.LoadString(IDS_IDTYPE_VIEW_256);
		break;
	case DDAQ_DEVICE_TYPE_VIEW_320:		s2.LoadString(IDS_IDTYPE_VIEW_320);
		break;
	case DDAQ_DEVICE_TYPE_MIDAS:		s2.LoadString(IDS_IDTYPE_MIDAS);
		break;
	case DDAQ_DEVICE_TYPE_MODULE_128:	s2.LoadString(IDS_IDTYPE_MODULE_128);
		break;
	case DDAQ_DEVICE_TYPE_LINE_128:		s2.LoadString(IDS_IDTYPE_LINE_128);
		break;
	case DDAQ_DEVICE_TYPE_LINE_256:		s2.LoadString(IDS_IDTYPE_LINE_256);
		break;
	case DDAQ_DEVICE_TYPE_HZK_160:		s2.LoadString(IDS_IDTYPE_HZK_160);
		break;
	case DDAQ_DEVICE_TYPE_HZK_256:		s2.LoadString(IDS_IDTYPE_HZK_256);
		break;
	default: s2.LoadString(IDS_IDTYPE_INVALID);
	}
	s.Format("PYRO%s", s2);
	pMeasureera_CType->SetValue((_variant_t)(s));

	s.Format("%d", id);
	pMeasureera_SN->SetValue((_variant_t)(strType));

	bool ok = false;
	float temp = 0.0f;

	theApp.DDAQ_IRDX_DEVICE_GetDetectorTemp(pDoc->m_hIRDX_Doc, &temp, &ok);
	s.Format("%.1f ℃", temp);
	pMeasureera_DTemp->SetValue((_variant_t)(s));

	theApp.DDAQ_IRDX_DEVICE_GetCameraTemp(pDoc->m_hIRDX_Doc, &temp, &ok);
	s.Format("%.1f ℃", temp);
	pMeasureera_CTemp->SetValue((_variant_t)(s));

	//=============== Document Size(Pixel) ===========================
	unsigned short SizeX = 0;
	unsigned short SizeY = 0;

	theApp.DDAQ_IRDX_PIXEL_GetSize(pDoc->m_hIRDX_Doc, &SizeX, &SizeY);
	s.Format("%d × %d", SizeX, SizeY);
	pDocSize_Pixel->SetValue((_variant_t)(s));

	//=============== Data Acquisition ===============================
	CString	strDate, strTime;
	theApp.DDAQ_IRDX_ACQUISITION_GetTimeStampString(pDoc->m_hIRDX_Doc, theApp.m_systemDate, theApp.m_systemTime);
	pDataAcq_Date->SetValue((_variant_t)(theApp.m_systemDate));
	pDataAcq_Time->SetValue((_variant_t)(theApp.m_systemTime));
	pDataAcqIndex->SetValue((_variant_t)(pDoc->m_AcqIndex));

	//=============== Measurement Range ==================================
	theApp.DDAQ_IRDX_DEVICE_GetRangeMinMax(pDoc->m_hIRDX_Doc, &pDoc->m_MinTemp, &pDoc->m_MaxTemp);
	s.Format("%.0f...%.0f ℃", pDoc->m_MinTemp, pDoc->m_MaxTemp);
	pDataAcq_MeasRange->SetValue((_variant_t)(s));


	//=================================================================
	theApp.DDAQ_IRDX_DEVICE_GetInternalFPS(pDoc->m_hIRDX_Doc, &pDoc->m_FPS);		// mody 2018-08-10

	unsigned short avg = 1;
	float fav;

	pDataAcq_AcqFreq->RemoveAllOptions();
	pDataAcq_AcqFreq->SetValue((_variant_t)(_T("")));


	///// Zoom Mode List Setting /////
	pScreenZoom->RemoveAllOptions();
	pScreenZoom->SetValue((_variant_t)strZoomMode[pDoc->m_ZoomMode]);

	pScreenZoom->AddOption(strZoomMode[0]);
	pScreenZoom->AddOption(strZoomMode[1]);
	pScreenZoom->AddOption(strZoomMode[2]);
	pScreenZoom->AddOption(strZoomMode[3]);
	pScreenZoom->AddOption(strZoomMode[4]);
	pScreenZoom->AddOption(strZoomMode[5]);
	pScreenZoom->AddOption(strZoomMode[6]);
	pScreenZoom->AddOption(strZoomMode[7]);
	pScreenZoom->AddOption(strZoomMode[8]);
	pScreenZoom->AddOption(strZoomMode[9]);
	pScreenZoom->AddOption(strZoomMode[10]);
	pScreenZoom->AddOption(strZoomMode[11]);

	pScreenBROI->RemoveAllOptions();
	pScreenBROI->SetValue((_variant_t)(_T("")));

	pScreenBROI->AddOption(NameofOOI[UNDISPLAY]);
	pScreenBROI->AddOption(NameofOOI[SHOW_BASE_ROI]);
	pScreenBROI->AddOption(NameofOOI[SHOW_EXCLUSIVE_ROI]);
	pScreenBROI->AddOption(NameofOOI[SHOW_ALL_ROI]);

	pScreenBROI->SetValue(NameofOOI[pDoc->m_OOIShowNum]);
	
	///// Screen POI List Setting ////
	pScreenPOI->RemoveAllOptions();
	pScreenPOI->SetValue((_variant_t)(_T("")));

	pScreenPOI->AddOption(NameOfPOI[UNDISPLAY]);
	pScreenPOI->AddOption(NameOfPOI[TEMPER_ONLY]);
	pScreenPOI->AddOption(NameOfPOI[NUMBER_ONLY]);
	pScreenPOI->AddOption(NameOfPOI[SHOW_ALL]);

	pScreenPOI->SetValue(NameOfPOI[pDoc->m_POIShowNum]);

	///// Frequency List Setting /////
	if (pDoc->m_FPS>0) {
		int dist_fav;

		for (int i = 1; i<12; i++) 
		{
			fav = pDoc->m_FPS / avg;

			dist_fav = fav * 100;			// Freq 판별

			if(dist_fav % 100 == 0)
				s.Format("%d (%.0f Hz)", avg, fav);
			else if(dist_fav % 10 == 0)
				s.Format("%d (%.1f Hz)", avg, fav);
			else
				s.Format("%d (%.2f Hz)", avg, fav);

			pDataAcq_AcqFreq->AddOption(s);

			avg *= 2;
		}

		if (pDoc->m_OpenMode == 3)
		{
			avg = 1;
			theApp.DDAQ_IRDX_ACQUISITION_GetAveraging(pDoc->m_hIRDX_Doc, &avg);

			pDoc->m_Avg = avg;

			fav = pDoc->m_FPS / avg;
			s.Format("%d (%.1f Hz)", avg, fav);

			pDataAcq_AcqFreq->SetValue((_variant_t)(s));
		}
		else if (pDoc->m_OpenMode == 1)
		{
			avg = theApp.m_acqFreq;

			pDoc->m_Avg = avg;

			fav = pDoc->m_FPS / avg;
			s.Format("%d (%.1f Hz)", avg, fav);

			pDataAcq_AcqFreq->SetValue((_variant_t)(s));
		}

	}
	else {
		theApp.DDAQ_IRDX_ACQUISITION_GetAveraging(pDoc->m_hIRDX_Doc, &avg);

		pDoc->m_Avg = avg;

		s.Format("AV=%d", avg);
		pDataAcq_AcqFreq->SetValue((_variant_t)(s));
	}
	if (pDoc->m_OpenMode > 1)
		s.Format("%d", theApp.m_loggingInterval);
	else
		s = "--";
	pDataAcq_LoggingInterval->SetValue((_variant_t)s);

	//================= Measurement Object ===============================
	theApp.DDAQ_IRDX_OBJECT_GetEmissivity(pDoc->m_hIRDX_Doc, &pDoc->m_Emissivity);
	pMeasObj_Emissivity->SetValue((_variant_t)pDoc->m_Emissivity);

	theApp.DDAQ_IRDX_OBJECT_GetTransmission(pDoc->m_hIRDX_Doc, &pDoc->m_Transmission);
	pMeasObj_Trans->SetValue((_variant_t)pDoc->m_Transmission);

	theApp.DDAQ_IRDX_OBJECT_GetAmbientTemp(pDoc->m_hIRDX_Doc, &pDoc->m_AmbTemp);
	pMeasObj_AmbTemp->SetValue((_variant_t)pDoc->m_AmbTemp);

	//===================== Scaling ====================================	
	theApp.DDAQ_IRDX_PALLET_GetBar(pDoc->m_hIRDX_Doc, &pDoc->m_nBar, &pDoc->m_NoCol);
	s.Format("%d", pDoc->m_NoCol);

	// MULTISCALE BAR Set
	pColorBar->SetValue((_variant_t)strColorBar[MULTISCALE]);
	theApp.DDAQ_IRDX_PALLET_SetBar(pDoc->m_hIRDX_Doc, MULTISCALE, pDoc->m_NoCol);

	pNoColor->SetValue((_variant_t)(s));

	theApp.DDAQ_IRDX_SCALE_GetMinMax(pDoc->m_hIRDX_Doc, &pDoc->m_ScaleMin, &pDoc->m_ScaleMax);

	pScale_MaxTemp->SetValue((_variant_t)pDoc->m_ScaleMax);
	pScale_MinTemp->SetValue((_variant_t)pDoc->m_ScaleMin);
	pScale_IsoTherm->SetValue((_variant_t)pDoc->m_ScaleMin);

	//======================= Focus =================================	
	s.Format("%d", pDoc->m_FocusPosition);
	pFocusPosition->SetValue((_variant_t)(s));

	//=====================Measuerment Data ==========================
	pMeasureDataThreshold->SetValue((_variant_t)(pDoc->m_Threshold));
	
	s.Format("%d", pDoc->m_ROICount);
	pMeasureDataROICount->SetValue((_variant_t)(s));
	
	s.Format("%d", pDoc->m_ROIBufferLimit);
	pMeasureROIBuffer->SetValue((_variant_t)(s));
	
	s.Format("%d", pDoc->m_ROICatchCnt);
	pMeasureROIBufferCnt->SetValue((_variant_t)(s));
	
	s.Format("%d", pDoc->m_Font_Size);
	pMeasureDataFontSize->SetValue((_variant_t)(s));

	s.Format("%d", pDoc->BROI->GetMinSize());
	pMeasureDataBaseROISize->SetValue((_variant_t)(s));

	pDataAcq_TriggerDist->SetValue((_variant_t)(s));
	pMeasureDataZoneInfoEnable->SetValue(pDoc->m_ZoneInfoEnable);
	//pMeasureDataAutoETMode->SetValue(pDoc->m_Auto_ET_Mode);

	//=====================Screen ==================================	
	pScreenZoom->SetValue((_variant_t)strZoomMode[pDoc->m_ZoomMode]);
	pScreenCursor->SetValue(pDoc->m_cursorShow);
	pScreenROI->SetValue(pDoc->m_ROIShow);
	//pScreenPOI->SetValue(pDoc->m_cursorShow);
	pScreenMaxPointer->SetValue(pDoc->m_maxPointerShow);
	pScreenMinPointer->SetValue(pDoc->m_minPointerShow);


	//=====================Spread Alarm ==================================
	pSpreadDetectRange->SetValue((_variant_t)(pDoc->m_spreadDetctRange));

	pSpreadLimitTempr->SetValue((_variant_t)(pDoc->m_spreadlimitTempr));

	//=====================Data Logging ==================================	
	pRawDataSaveEnable->SetValue(pDoc->m_bRawDataSave);
	pRawDataSaveFilePath->SetValue((_variant_t)(pDoc->m_strRawDataFilePath));
	pResultDataSaveFilePath->SetValue((_variant_t)(pDoc->m_strResultDataFilePath));
	////////////////////////////////////////////////////////////////////////////////	

	// Enabling Controls ///////////////////////////////////////////////////////////
	pMeasObj_Emissivity->AllowEdit(TRUE);
	pMeasObj_Trans->AllowEdit(TRUE);
	pMeasObj_AmbTemp->AllowEdit(TRUE);

	pColorBar->Enable(true);
	pNoColor->Enable(true);
	pScale_MaxTemp->AllowEdit(TRUE);
	pScale_MinTemp->AllowEdit(TRUE);
	pScale_IsoTherm->AllowEdit(TRUE);

	pMeasureDataThreshold->AllowEdit(TRUE);
	pMeasureDataROICount->AllowEdit(TRUE);
	pMeasureDataBaseROISize->AllowEdit(TRUE);
	pMeasureROIBuffer->AllowEdit(TRUE);
	pMeasureROIBufferCnt->AllowEdit(TRUE);
	pMeasureDataFontSize->AllowEdit(TRUE);
	pMeasureDataZoneInfoEnable->Enable(TRUE);
	//pMeasureDataAutoETMode->Enable(TRUE);

	pScreenCursor->Enable(TRUE);
	pScreenROI->Enable(TRUE);
	pScreenZoom->Enable(TRUE);
	pScreenPOI->Enable(TRUE);
	pScreenBROI->Enable(TRUE);
	pScreenMaxPointer->Enable(TRUE);
	pScreenMinPointer->Enable(TRUE);

	pSpreadDetectRange->AllowEdit(TRUE);
	pSpreadLimitTempr->AllowEdit(TRUE);

	pDataTriggerFlag->Enable(1);

	pRawDataSaveEnable->Show(FALSE);

	pSpread->Show(TRUE);
	pDataSave->Show(TRUE);
	pMeasureData->Show(TRUE);
	pScreen->Show(TRUE);
	pRawDataSaveEnable->Show(FALSE);

	// Simulation mode & Online mode
	if (pDoc->m_OpenMode>1) {	
		pDataAcq_LoggingInterval->AllowEdit(TRUE);
	}
	// IRDX mode
	else {
		pMeasureDataZoneInfoEnable->Enable(FALSE);
		pDataAcq_LoggingInterval->AllowEdit(FALSE);
	}

	//활성화 or 비활성화
	if (pDoc->m_OpenMode>1 && !theApp.m_bLoggingRunning)
	{
		//
		pRawDataSaveEnable->Enable(TRUE);
		pRawDataSaveFilePath->Enable(TRUE);
		pResultDataSaveFilePath->Enable(TRUE);
	}
	else if(pDoc->m_OpenMode>1 && theApp.m_bLoggingRunning){
		//
		pRawDataSaveEnable->Enable(FALSE);
		pRawDataSaveFilePath->Enable(FALSE);
		pResultDataSaveFilePath->Enable(FALSE);
		pDataTriggerFlag->Enable(FALSE);
		pDataAcq_TriggerDist->Enable(FALSE);
	}
	else
	{
		pRawDataSaveEnable->Enable(FALSE);
		pRawDataSaveFilePath->Enable(FALSE);
		pResultDataSaveFilePath->Enable(TRUE);
	}
	////////////////////////////////////////////////////////////////////////////////

	pDoc->UpdateAllViews(NULL, 2);
	if(theApp.m_bFileOpen)
		theApp.m_pScaleDlg->Invalidate();
	//theApp.m_pScaleDlg->Invalidate(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::UpdateDataSet()
{
	CPyroSoftMDoc* pDoc = GetDocument();

	if (pDoc == NULL) return;

	// return if document is not ready now
	if (pDoc->m_hIRDX_Doc == INVALID_HANDLE_VALUE)
		return;

	bool ok = false;
	float temp1 = 0.0f, temp2 = 0.0f;
	float IRDX_transmisstion = 0.0f, IRDX_emissivity = 0.0f;
	unsigned short Pos = 0;

	CString s1, s2;
	//CString	strDate, strTime, strPos;	//mody 05-24
	CString	strPos;

	theApp.DDAQ_IRDX_FILE_GetCurDataSet(pDoc->m_hIRDX_Doc, &(pDoc->m_IdxDS));
	theApp.DDAQ_IRDX_DEVICE_GetDetectorTemp(pDoc->m_hIRDX_Doc, &temp1, &ok);
	theApp.DDAQ_IRDX_DEVICE_GetCameraTemp(pDoc->m_hIRDX_Doc, &temp2, &ok);
	theApp.DDAQ_IRDX_ACQUISITION_GetTimeStampString(pDoc->m_hIRDX_Doc, theApp.m_systemDate, theApp.m_systemTime);

	if (theApp.m_FocusFlag && pDoc->m_OpenMode == 3)
		theApp.DDAQ_IRDX_ACQUISITION_GetMotorFocusPos(pDoc->m_hIRDX_Doc, &Pos, &pDoc->m_camStatus);

	pDoc->m_FocusPosition = Pos;

	//pDoc->m_IdxDS = m_CurrDR; 
	s1.Format("%.1f ℃", temp1);
	s2.Format("%.1f ℃", temp2);
	strPos.Format("%d", Pos);

	pFile_CurDR->SetValue((_variant_t)(pDoc->m_IdxDS + 1));
	pMeasureera_DTemp->SetValue((_variant_t)(s1));
	pMeasureera_CTemp->SetValue((_variant_t)(s2));
	pDataAcq_Date->SetValue((_variant_t)(theApp.m_systemDate));
	pDataAcq_Time->SetValue((_variant_t)(theApp.m_systemTime));
	pDataAcqIndex->SetValue((_variant_t)(pDoc->m_AcqIndex));
	pFocusPosition->SetValue((_variant_t)strPos);
	pRawDataSaveFilePath->SetValue((_variant_t)pDoc->m_strRawDataFilePath);
	pResultDataSaveFilePath->SetValue((_variant_t)pDoc->m_strResultDataFilePath);
	

	if (pDoc->m_OpenMode == 1)
	{
		theApp.DDAQ_IRDX_OBJECT_GetTransmission(pDoc->m_hIRDX_Doc, &IRDX_transmisstion);
		theApp.DDAQ_IRDX_OBJECT_GetEmissivity(pDoc->m_hIRDX_Doc, &IRDX_emissivity);

		theApp.DDAQ_IRDX_OBJECT_SetTransmission(pDoc->m_hIRDX_Doc, IRDX_transmisstion);
		theApp.DDAQ_IRDX_OBJECT_SetEmissivity(pDoc->m_hIRDX_Doc, IRDX_emissivity);

		pMeasObj_Trans->SetValue((_variant_t)IRDX_transmisstion);
		pMeasObj_Emissivity->SetValue((_variant_t)IRDX_emissivity);
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int CPropertiesWnd::IndexOfColorBar(CString s)
{
	int index = 0;
	for (int i = 0; i<9; i++) {
		if (s == strColorBar[i]) {
			return i;
		}
	}
	return index;
}

int CPropertiesWnd::IndexOfNoColor(CString s)
{
	int index = 0;
	for (int i = 0; i<7; i++) {
		if (s == NoScaleColor[i]) {
			return NoScaleColorValue[i];
		}
	}

	return NoScaleColorValue[index];
}

int CPropertiesWnd::valudeOfFrequency(unsigned short avg)
{
	int result = 1;		// 1Hz

	switch (avg)
	{
	case 1:
		result = 1000 / 60;		// 60Hz
		break;
	case 2:
		result = 1000 / 30;		// 30Hz
		break;
	case 4:
		result = 1000 / 16;		// 16Hz
		break;
	case 8:
		result = 1000 / 8;		// 8Hz
		break;
	case 16:
		result = 1000 / 4;		// 4Hz
		break;
	case 32:
		result = 1000 / 2;		// 2Hz
		break;
	case 64:
		result = 1000 / 1;		// 1Hz
		break;
	case 128:
		result = 1000 / 0.5;	// 0.50Hz
		break;
	case 256:
		result = 1000 / 0.25;	// 0.25Hz
		break;
	}
	return result;
}

unsigned short CPropertiesWnd::valueOfAverage(CString str, float FPS)
{
	float fav;
	unsigned short avg = 1;
	int dist_fav;
	CString s;

	for (int i = 1; i<12; i++) {
		fav = FPS / avg;
		dist_fav = fav * 100;			// Freq 판별

		//s.Format("%d (%.1f Hz)", avg, fav);
		if (dist_fav % 100 == 0)
			s.Format("%d (%.0f Hz)", avg, fav);
		else if (dist_fav % 10 == 0)
			s.Format("%d (%.1f Hz)", avg, fav);
		else
			s.Format("%d (%.2f Hz)", avg, fav);

		if (s == str)
			return avg;

		avg *= 2;
	}
	return avg;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty *pProp = (CMFCPropertyGridProperty*)lparam;

	if (!pProp) return 0;

	char* pbstr;

	CPyroSoftMDoc* pDoc = GetDocument();

	pDoc->m_bUpdateProperty = true;

	COleVariant rVariant = pProp->GetValue();
	VARIANT var = rVariant.Detach();

	if (pProp == pMeasObj_Emissivity) {
		float fVal = var.fltVal;

		if (fVal>1.0 || fVal<0.1f) {
			pMeasObj_Emissivity->SetValue((_variant_t)pDoc->m_Emissivity);

			return 0;
		}
		pDoc->m_Emissivity = fVal;

		theApp.DDAQ_IRDX_OBJECT_SetEmissivity(pDoc->m_hIRDX_Doc, pDoc->m_Emissivity);

		if (!theApp.DDAQ_IRDX_PIXEL_GetData(pDoc->m_hIRDX_Doc, pDoc->pFrameBuffer, pDoc->BufSize))
		{
			delete[] pDoc->pFrameBuffer;
			return 0;
		}

		pDoc->columnUpdateFlag = true;
		pDoc->UpdateAllViews(NULL, 7);
		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pMeasObj_Trans) {
		float fVal = var.fltVal;

		if (fVal>1.0 || fVal<0.1f) {
			pMeasObj_Trans->SetValue((_variant_t)pDoc->m_Transmission);

			return 0;
		}
		pDoc->m_Transmission = fVal;

		theApp.DDAQ_IRDX_OBJECT_SetTransmission(pDoc->m_hIRDX_Doc, pDoc->m_Transmission);

		if (!theApp.DDAQ_IRDX_PIXEL_GetData(pDoc->m_hIRDX_Doc, pDoc->pFrameBuffer, pDoc->BufSize))
		{
			delete[] pDoc->pFrameBuffer;
			return 0;

		}

		pDoc->columnUpdateFlag = true;
		pDoc->UpdateAllViews(NULL, 7);
		pDoc->UpdateAllViews(NULL, 2);

	}
	else if (pProp == pMeasObj_AmbTemp) {
		float fVal = var.fltVal;

		if (fVal>pDoc->m_MaxTemp || fVal<pDoc->m_MinTemp) {
			pMeasObj_AmbTemp->SetValue((_variant_t)pDoc->m_AmbTemp);

			return 0;
		}
		pDoc->m_AmbTemp = fVal;

		theApp.DDAQ_IRDX_OBJECT_SetAmbientTemp(pDoc->m_hIRDX_Doc, pDoc->m_AmbTemp);

		if (!theApp.DDAQ_IRDX_PIXEL_GetData(pDoc->m_hIRDX_Doc, pDoc->pFrameBuffer, pDoc->BufSize))
		{
			delete[] pDoc->pFrameBuffer;
			return 0;
		}

		pDoc->columnUpdateFlag = true;
		pDoc->UpdateAllViews(NULL, 7);
		pDoc->UpdateAllViews(NULL, 2);
	}

	// Color Scale //**************************************************************
	else if (pProp == pColorBar) {
		USES_CONVERSION;					//convert를 위한 매크로
		pbstr = OLE2A(var.bstrVal);			//bstr을 char로 convert
		pDoc->m_nBar = IndexOfColorBar(CString(pbstr));

		theApp.DDAQ_IRDX_PALLET_SetBar(pDoc->m_hIRDX_Doc, pDoc->m_nBar, pDoc->m_NoCol);

		pDoc->UpdateAllViews(NULL, 2);
		theApp.m_pScaleDlg->Invalidate();
	}
	else if (pProp == pNoColor) {
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		pDoc->m_NoCol = IndexOfNoColor(CString(pbstr));

		theApp.DDAQ_IRDX_PALLET_SetBar(pDoc->m_hIRDX_Doc, pDoc->m_nBar, pDoc->m_NoCol);

		pDoc->UpdateAllViews(NULL, 2);
		theApp.m_pScaleDlg->Invalidate();
	}
	else if (pProp == pScale_MaxTemp) 
	{
		float fVal = var.fltVal;

		if (fVal>pDoc->m_MaxTemp) {
			pDoc->m_ScaleMax = pDoc->m_MaxTemp;
		}
		else if (fVal <= pDoc->m_ScaleMin) {
			pDoc->m_ScaleMax = pDoc->m_ScaleMin + 1.0f;
		}
		else {
			pDoc->m_ScaleMax = fVal;
		}

		// set to irdx
		pScale_MaxTemp->SetValue((_variant_t)pDoc->m_ScaleMax);
		theApp.DDAQ_IRDX_SCALE_SetMinMax(pDoc->m_hIRDX_Doc, pDoc->m_ScaleMin, pDoc->m_ScaleMax);

		pDoc->UpdateAllViews(NULL, 2);
		theApp.m_pScaleDlg->Invalidate();
	}
	else if (pProp == pScale_MinTemp) 
	{
		float fVal = var.fltVal;

		if (fVal<pDoc->m_MinTemp) {
			pDoc->m_ScaleMin = pDoc->m_MinTemp;
		}
		else if (fVal >= pDoc->m_ScaleMax) {
			pDoc->m_ScaleMin = pDoc->m_ScaleMax - 1.0f;
		}
		else {
			pDoc->m_ScaleMin = fVal;
		}
		// set to irdx
		pScale_MinTemp->SetValue((_variant_t)pDoc->m_ScaleMin);
		theApp.DDAQ_IRDX_SCALE_SetMinMax(pDoc->m_hIRDX_Doc, pDoc->m_ScaleMin, pDoc->m_ScaleMax);

		pDoc->UpdateAllViews(NULL, 2);
		theApp.m_pScaleDlg->Invalidate(FALSE);
	}
	// Measuerment Data *******************************************************
	else if (pProp == pMeasureDataThreshold)
	{
		float fVal = var.fltVal;

		if (fVal > 2000)
			pDoc->m_Threshold = 2000.0f;
		else if (fVal < 600)
			pDoc->m_Threshold = 600.0f;
		else
			pDoc->m_Threshold = fVal;

		// set to irdx
		pMeasureDataThreshold->SetValue((_variant_t)pDoc->m_Threshold);
		
		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pMeasureDataROICount)
	{
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		int iVal = _ttoi(CString(pbstr));
		CString str;

		pDoc->m_comp_ROICount = pDoc->m_ROICount;

		if (iVal > 50)
			pDoc->m_ROICount = 50;
		else if (iVal < 1)
			pDoc->m_ROICount = 1;
		else
			pDoc->m_ROICount = iVal;

		str.Format("%d", pDoc->m_ROICount);
		pMeasureDataROICount->SetValue(str);

		pDoc->m_ROI_check_flag = true;
		pDoc->columnUpdateFlag = true;

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pMeasureDataBaseROISize)
	{
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		int iVal = _ttoi(CString(pbstr));

		CString str;
		int tmp_BROIMinSize = 0;

		if (iVal > 10)
			tmp_BROIMinSize = 10;
		else if (iVal < 1)
			tmp_BROIMinSize = 1;
		else
			tmp_BROIMinSize = iVal;

		str.Format("%d", tmp_BROIMinSize);
		pMeasureDataBaseROISize->SetValue(str);

		pDoc->BROI->SetMinSize(tmp_BROIMinSize);

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pMeasureROIBuffer)
	{
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		int iVal = _ttoi(CString(pbstr));
		CString str;

		if (iVal >= MAX_ROI_CNT)
			pDoc->m_ROIBufferLimit = MAX_ROI_CNT-1;
		else if (iVal < 1)
			pDoc->m_ROIBufferLimit = 1;
		else
			pDoc->m_ROIBufferLimit = iVal;

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pMeasureROIBufferCnt)
	{
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		int iVal = _ttoi(CString(pbstr));
		CString str;

		if (iVal >= MAX_ROI_CNT)
			pDoc->m_ROICatchCnt = MAX_ROI_CNT-1;
		else if (iVal < 1)
			pDoc->m_ROICatchCnt = 1;
		else
			pDoc->m_ROICatchCnt = iVal;

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pDataAcq_TriggerDist)
	{
		// pDataAcq_TriggerDist 삭제 예정 
	}
	else if (pProp == pMeasureDataFontSize)
	{
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		int iVal = _ttoi(CString(pbstr));
		CString str;

		if (iVal > 30)
			pDoc->m_Font_Size = 30;
		else if (iVal < 1)
			pDoc->m_Font_Size = 1;
		else
			pDoc->m_Font_Size = iVal;

		str.Format("%d", pDoc->m_Font_Size);
		pMeasureDataFontSize->SetValue(str);

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pMeasureDataZoneInfoEnable)
	{
		pDoc->m_ZoneInfoEnable = pMeasureDataZoneInfoEnable->GetValue();

		
		//if (!(pDoc->m_ZoneInfoEnable))
		//{
		//	pDoc->m_Auto_ET_Mode = false;
		//	pMeasureDataAutoETMode->SetValue(pDoc->m_Auto_ET_Mode);
		//	pMeasureDataAutoETMode->Enable(FALSE);
		//}
		//else
		//	pMeasureDataAutoETMode->Enable(TRUE);

		pDoc->UpdateAllViews(NULL, 2);
	}
	//else if (pProp == pMeasureDataAutoETMode)
	//{
	//	pDoc->m_Auto_ET_Mode = pMeasureDataAutoETMode->GetValue();

	//	pDoc->UpdateAllViews(NULL, 8);	// update 8번이 무슨 기능인지 보자
	//}
	else if (pProp == pScreenCursor)
	{
		pDoc->m_cursorShow = pScreenCursor->GetValue();
	}
	else if (pProp == pScreenROI)
	{
		pDoc->m_ROIShow = pScreenROI->GetValue();
	}
	else if (pProp == pScreenZoom)
	{
		CString compName;

		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		compName = (CString)pbstr;

		if (!compName.Compare(strZoomMode[0])) {		// Auto
			pDoc->m_ZoomMode = ZOOM_AUTO;
			//pDoc->m_ZoomRatio = 1;
		}
		else if (!compName.Compare(strZoomMode[1])) {		// 25%
			pDoc->m_ZoomMode = ZOOM_QUATER;
			pDoc->m_ZoomRatio = 0.25;
		}
		else if (!compName.Compare(strZoomMode[2]))			// 50%
		{
			pDoc->m_ZoomMode = ZOOM_HALF;
			pDoc->m_ZoomRatio = 0.5;
		}
		else if (!compName.Compare(strZoomMode[3]))		// 100%
		{
			pDoc->m_ZoomMode = ZOOM_ONE;
			pDoc->m_ZoomRatio = 1;
		}
		else if (!compName.Compare(strZoomMode[4]))		// 200%
		{
			pDoc->m_ZoomMode = ZOOM_TWOTIMES;
			pDoc->m_ZoomRatio = 2;
		}
		else if (!compName.Compare(strZoomMode[5]))		// 300%
		{
			pDoc->m_ZoomMode = ZOOM_THREETIMES;
			pDoc->m_ZoomRatio = 3;
		}
		else if (!compName.Compare(strZoomMode[6]))		// 400%
		{
			pDoc->m_ZoomMode = ZOOM_FOURTIMES;
			pDoc->m_ZoomRatio = 4;
		}
		else if (!compName.Compare(strZoomMode[7]))		// 500%
		{
			pDoc->m_ZoomMode = ZOOM_FIVETIMES;
			pDoc->m_ZoomRatio = 5;
		}
		else if (!compName.Compare(strZoomMode[8]))		// 600%
		{
			pDoc->m_ZoomMode = ZOOM_SIXTIMES;
			pDoc->m_ZoomRatio = 6;
		}
		else if (!compName.Compare(strZoomMode[9])) {		// 800%
			pDoc->m_ZoomMode = ZOOM_EIGHTTIMES;
			pDoc->m_ZoomRatio = 8;
		}
		else if (!compName.Compare(strZoomMode[10])) {		// 1000%
			pDoc->m_ZoomMode = ZOOM_TENTIMES;
			pDoc->m_ZoomRatio = 10;
		}
		else if (!compName.Compare(strZoomMode[11])) {		// 1500%
			pDoc->m_ZoomMode = ZOOM_FIFTEENTIMES;
			pDoc->m_ZoomRatio = 15;
		}
		//else if (!compName.Compare(strZoomMode[7]))		//ROI size
		//	pDoc->m_ZoomMode = 7;
		else {
			pDoc->m_ZoomMode = ZOOM_AUTO;
			//pDoc->m_ZoomRatio = 1;
		}

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pScreenBROI)
	{
		CString compName;

		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		compName = (CString)pbstr;

		if (!compName.Compare(NameofOOI[UNDISPLAY]))
			pDoc->m_OOIShowNum = UNDISPLAY;
		else if (!compName.Compare(NameofOOI[SHOW_BASE_ROI]))
			pDoc->m_OOIShowNum = SHOW_BASE_ROI;
		else if (!compName.Compare(NameofOOI[SHOW_EXCLUSIVE_ROI]))
			pDoc->m_OOIShowNum = SHOW_EXCLUSIVE_ROI;
		else if (!compName.Compare(NameofOOI[SHOW_ALL_ROI]))
			pDoc->m_OOIShowNum = SHOW_ALL_ROI;
		else
			pDoc->m_OOIShowNum = 0;

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pScreenPOI)
	{
		CString compName;

		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		compName = (CString)pbstr;

		if (!compName.Compare(NameOfPOI[UNDISPLAY]))
			pDoc->m_POIShowNum = 0;
		else if (!compName.Compare(NameOfPOI[TEMPER_ONLY]))
			pDoc->m_POIShowNum = 1;
		else if (!compName.Compare(NameOfPOI[NUMBER_ONLY]))
			pDoc->m_POIShowNum = 2;
		else if (!compName.Compare(NameOfPOI[SHOW_ALL]))
			pDoc->m_POIShowNum = 3;
		else
			pDoc->m_POIShowNum = 0;

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pScreenMaxPointer)
	{
		pDoc->m_maxPointerShow = pScreenMaxPointer->GetValue();

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pScreenMinPointer)
	{
		pDoc->m_minPointerShow = pScreenMinPointer->GetValue();

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pDataTriggerFlag)
	{
		// pDataTriggerFlag 삭제 해야해 
	}
	else if (pProp == pDataAcq_Trigger)
	{
		// pDataAcq_Trigger 삭제 해야해
	}
	// Acquistion **************************************************************
	else if (pProp == pDataAcq_AcqFreq) {
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		pDoc->m_Avg = valueOfAverage(CString(pbstr), pDoc->m_FPS);

		if (pDoc->m_OpenMode == 3)
		{
			if (!theApp.DDAQ_DEVICE_DO_Stop(pDoc->m_nDDAQDeviceNo))
				return 0;

			theApp.DDAQ_IRDX_ACQUISITION_SetAveraging(pDoc->m_hIRDX_DEVICE, pDoc->m_Avg);

			if (!theApp.DDAQ_DEVICE_DO_Start(pDoc->m_nDDAQDeviceNo))
				return 0;
		}
		else if (pDoc->m_OpenMode == 1)
		{
			//theApp.m_acqFreq = valudeOfFrequency(pDoc->m_Avg);
			theApp.m_acqFreq = pDoc->m_Avg;
		}

	}
	else if (pProp == pDataAcq_LoggingInterval) {
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		int iVal = _ttoi(CString(pbstr));
		CString str;

		if (iVal > 100000)
			theApp.m_loggingInterval = 100000;
		else if (iVal < 100)
			theApp.m_loggingInterval = 100;
		else
			theApp.m_loggingInterval = iVal;

		str.Format("%d", theApp.m_loggingInterval);

		pDataAcq_LoggingInterval->SetValue(str);

	}
	else if (pProp == pSpreadDetectRange)
	{
		float fVal = var.fltVal;

		if (fVal < pDoc->m_MinTemp)
			pDoc->m_spreadDetctRange = pDoc->m_MinTemp;
		else if (fVal >= pDoc->m_MaxTemp)
			pDoc->m_spreadDetctRange = pDoc->m_MaxTemp - 1.0f;
		else
			pDoc->m_spreadDetctRange = fVal;

		pSpreadDetectRange->SetValue((_variant_t)pDoc->m_spreadDetctRange);

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pSpreadLimitTempr)
	{
		float fVal = var.fltVal;

		if (fVal < 1)
			pDoc->m_spreadlimitTempr = 1;
		else if (fVal >= 10000)
			pDoc->m_spreadlimitTempr = 10000 - 1.0f;
		else
			pDoc->m_spreadlimitTempr = fVal;

		pSpreadLimitTempr->SetValue((_variant_t)pDoc->m_spreadlimitTempr);

		pDoc->UpdateAllViews(NULL, 2);
	}
	// Raw Data Saving **************************************************************
	else if (pProp == pRawDataSaveEnable) {
		pDoc->m_bRawDataSave = pRawDataSaveEnable->GetValue();

		pDoc->UpdateAllViews(NULL, 8);
	}
	else if (pProp == pRawDataSaveFilePath) {
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		pDoc->m_strRawDataFilePath = CString(pbstr);
	}
	else if (pProp == pResultDataSaveFilePath)
	{
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		pDoc->m_strResultDataFilePath = CString(pbstr);
	}
	// =========================== 여기서 부터 수정 코드 ===========================
	else if (pProp == pScale_IsoTherm) {
		int fVal = var.fltVal;

		if (fVal < pDoc->m_MinTemp) {
			pDoc->m_ScaleIsoTherm = pDoc->m_MinTemp;
		}
		else if (fVal >= pDoc->m_ScaleMax) {
			pDoc->m_ScaleIsoTherm = pDoc->m_ScaleMax - 1.0f;
		}
		else {
			pDoc->m_ScaleIsoTherm = fVal;
		}

		///////////////////////////////////////////////////
		unsigned short   nNoIsoTherm;

		theApp.DDAQ_IRDX_PALLET_GetNumIsotherms(pDoc->m_hIRDX_Doc, &nNoIsoTherm);

		if (nNoIsoTherm < 3) {
			bool bRet = theApp.DDAQ_IRDX_PALLET_SetNumIsotherms(pDoc->m_hIRDX_Doc, 1);

			bRet = theApp.DDAQ_IRDX_PALLET_SetIsotherm(pDoc->m_hIRDX_Doc, 1, pDoc->m_MinTemp, pDoc->m_ScaleIsoTherm,
				RGB(0, 0, 0), 0, false);
		}
		else {
			unsigned long Col;
			unsigned short Sat;
			bool bTransp;

			theApp.DDAQ_IRDX_PALLET_GetIsotherm(pDoc->m_hIRDX_Doc, 1, &pDoc->m_IsoThermMin, &pDoc->m_ScaleIsoTherm,
				&Col, &Sat, &bTransp);
		}

		pScale_IsoTherm->SetValue((_variant_t)pDoc->m_ScaleIsoTherm);
		////////////////////////////////////////////////////////////////////

		pDoc->UpdateAllViews(NULL, 2);
		theApp.m_pScaleDlg->Invalidate();
	}

	pDoc->m_bUpdateProperty = false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void CPropertiesWnd::InitFreqeuncy(CPyroSoftMDoc *pDoc, unsigned short avg)
{
	if (!theApp.DDAQ_DEVICE_DO_Stop(pDoc->m_nDDAQDeviceNo))
		return;

	theApp.DDAQ_IRDX_ACQUISITION_SetAveraging(pDoc->m_hIRDX_DEVICE, avg);
}

void CPropertiesWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case ID_TIMER:
		AfxMessageBox("OnTimer안에 1초 예상값을 위하여");
		break;
	}

	CDockablePane::OnTimer(nIDEvent);
}

void CPropertiesWnd::OnChangedZoomOnly()
{
	CPyroSoftMDoc* pDoc = (CPyroSoftMDoc*)GetDocument();
	pScreenZoom->SetValue((_variant_t)strZoomMode[pDoc->m_ZoomMode]);
}