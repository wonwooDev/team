
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

CString strZoomMode[8] = {"100%","100%","125%","150%","200%","300%","400%","ROI Size"};

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

	pDataTriggerFlag = new CCheckBoxPropertyGridProperty(_T("Laser Trigger"), true, (_variant_t)true, _T(""));
	pDataTriggerFlag->Enable(FALSE);
	pDataAcq->AddSubItem(pDataTriggerFlag);

	pDataAcq_Trigger = new CCheckBoxPropertyGridProperty(_T("Near from Dev."), true, (_variant_t)true, _T(""));
	pDataAcq_Trigger->Enable(FALSE);
	pDataAcq->AddSubItem(pDataAcq_Trigger);

	pDataAcq_TriggerDist = new CMFCPropertyGridProperty(_T("Distance"), _T(""));
	pDataAcq_TriggerDist->AllowEdit(FALSE);
	pDataAcq->AddSubItem(pDataAcq_TriggerDist);

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
	pFocusLocation = new CMFCPropertyGridProperty(_T("Location"), (_variant_t)_T(""), _T(""));

	pFocusLocation->AllowEdit(FALSE);
	pFocusLocation->Enable(0);
	pFocus->AddSubItem(pFocusLocation);

	m_wndPropList.AddProperty(pFocus);

	//pFocus->Show(FALSE);


	// Measurement Data ///////////////////////////////////////////////////////////////////////////////////////////////
	pMeasureData = new CMFCPropertyGridProperty(_T("Measurement Info"));

	pMeasureDataAutoETMode = new CCheckBoxPropertyGridProperty(_T("Auto E/T Mode"), true, (_variant_t)true, _T(""));
	pMeasureDataAutoETMode->Enable(FALSE);
	pMeasureData->AddSubItem(pMeasureDataAutoETMode);

	pMeasureDataThreshold = new CFLoatPropertyGridProperty(_T("Threshold (℃)"), (_variant_t) 0.0f);
	pMeasureDataThreshold->AllowEdit(FALSE);
	pMeasureData->AddSubItem(pMeasureDataThreshold);

	pMeasureDataROICount = new CMFCPropertyGridProperty(_T("ROI Count "), _T(""));
	pMeasureDataROICount->AllowEdit(FALSE);
	pMeasureData->AddSubItem(pMeasureDataROICount);

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

	pScreenCursor = new CCheckBoxPropertyGridProperty(_T("Cursor Temperature"), true, (_variant_t)true, _T(""));
	pScreenCursor->Enable(FALSE);
	pScreen->AddSubItem(pScreenCursor);

	pScreenROI = new CCheckBoxPropertyGridProperty(_T("Region of Interest"), true, (_variant_t)true, _T(""));
	pScreenROI->Enable(FALSE);
	pScreen->AddSubItem(pScreenROI);

	pScreenPOI = new CMFCPropertyGridProperty(_T("Point of Interest"), (_variant_t)_T(""), _T(""), 2);
	pScreenPOI->AllowEdit(FALSE);
	pScreenPOI->Enable(FALSE);
	pScreen->AddSubItem(pScreenPOI);

	pScreenMaxPointer = new CCheckBoxPropertyGridProperty(_T("Maximun Pointer"), true, (_variant_t)true, _T(""));
	pScreenMaxPointer->Enable(FALSE);
	pScreen->AddSubItem(pScreenMaxPointer);

	pScreenMinPointer = new CCheckBoxPropertyGridProperty(_T("Minimun Pointer"), true, (_variant_t)true, _T(""));
	pScreenMinPointer->Enable(FALSE);
	pScreen->AddSubItem(pScreenMinPointer);

	m_wndPropList.AddProperty(pScreen);

	pScreen->Show(FALSE);


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

	//pRawDataSaveFileName = new CMFCPropertyGridProperty(_T("File Basename"), (_variant_t)_T(""), _T(""));
	//pRawDataSaveFileName->AllowEdit(FALSE);
	//pDataSave->AddSubItem(pRawDataSaveFileName);

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
	theApp.DDAQ_IRDX_ACQUISITION_GetTimeStampString(pDoc->m_hIRDX_Doc, strDate, strTime);
	pDataAcq_Date->SetValue((_variant_t)(strDate));
	pDataAcq_Time->SetValue((_variant_t)(strTime));
	pDataAcqIndex->SetValue((_variant_t)(pDoc->m_AcqIndex));

	//=============== Measurement Range ==================================
	theApp.DDAQ_IRDX_DEVICE_GetRangeMinMax(pDoc->m_hIRDX_Doc, &pDoc->m_MinTemp, &pDoc->m_MaxTemp);
	s.Format("%.0f...%.0f ℃", pDoc->m_MinTemp, pDoc->m_MaxTemp);
	pDataAcq_MeasRange->SetValue((_variant_t)(s));


	//=================================================================
	if (pDoc->m_OpenMode>1) {
		theApp.DDAQ_IRDX_DEVICE_GetInternalFPS(pDoc->m_hIRDX_Doc, &pDoc->m_FPS);
	}
	else {
		pDoc->m_FPS = 0.0f;
	}

	unsigned short avg = 1;
	float fav;

	pDataAcq_AcqFreq->RemoveAllOptions();
	pDataAcq_AcqFreq->SetValue((_variant_t)(_T("")));


	///// Zoom Mode List Setting /////
	pScreenZoom->RemoveAllOptions();
	pScreenZoom->SetValue((_variant_t)strZoomMode[pDoc->m_ZoomMode]);

	//pScreenZoom->AddOption(strZoomMode[0]);
	pScreenZoom->AddOption(strZoomMode[1]);
	pScreenZoom->AddOption(strZoomMode[2]);
	pScreenZoom->AddOption(strZoomMode[3]);
	pScreenZoom->AddOption(strZoomMode[4]);
	pScreenZoom->AddOption(strZoomMode[5]);
	pScreenZoom->AddOption(strZoomMode[6]);
	//pScreenZoom->AddOption(strZoomMode[7]);
	

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
		for (int i = 1; i<10; i++) {
			fav = pDoc->m_FPS / avg;

			s.Format("%d (%.1f Hz)", avg, fav);
			pDataAcq_AcqFreq->AddOption(s);

			avg *= 2;
		}
		avg = 1;
		theApp.DDAQ_IRDX_ACQUISITION_GetAveraging(pDoc->m_hIRDX_Doc, &avg);

		pDoc->m_Avg = avg;

		fav = pDoc->m_FPS / avg;
		s.Format("%d (%.1f Hz)", avg, fav);

		pDataAcq_AcqFreq->SetValue((_variant_t)(s));
	}
	else {
		theApp.DDAQ_IRDX_ACQUISITION_GetAveraging(pDoc->m_hIRDX_Doc, &avg);

		pDoc->m_Avg = avg;

		s.Format("AV=%d", avg);
		pDataAcq_AcqFreq->SetValue((_variant_t)(s));
	}

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
	s.Format("%d", pDoc->m_FocusingIndex);
	pFocusLocation->SetValue((_variant_t)(s));

	//=====================Measuerment Data ==========================
	pMeasureDataThreshold->SetValue((_variant_t)(pDoc->m_Threshold));
	s.Format("%d", pDoc->m_ROICount);
	pMeasureDataROICount->SetValue((_variant_t)(s));
	s.Format("%d", pDoc->m_Font_Size);
	pMeasureDataFontSize->SetValue((_variant_t)(s));

	pDataTriggerFlag->SetValue(pDoc->m_TriggerOnOff);

	if (pDoc->m_TriggerOnOff)
		s.Format("%.0f", pDoc->m_TriggerDist);
	else
		s = "--";
	pDataAcq_TriggerDist->SetValue((_variant_t)(s));
	pMeasureDataAutoETMode->SetValue(pDoc->m_Auto_ET_Mode);
	pDataAcq_Trigger->SetValue(pDoc->m_NearFromDev);

	//=====================Screen ==================================	
	pScreenZoom->SetValue((_variant_t)strZoomMode[pDoc->m_ZoomMode]);
	pScreenCursor->SetValue(pDoc->m_cursorShow);
	pScreenROI->SetValue(pDoc->m_ROIShow);
	//pScreenPOI->SetValue(pDoc->m_cursorShow);
	pScreenMaxPointer->SetValue(pDoc->m_maxPointerShow);
	pScreenMinPointer->SetValue(pDoc->m_minPointerShow);

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
	pMeasureDataFontSize->AllowEdit(TRUE);
	pMeasureDataAutoETMode->Enable(true);

	pScreenCursor->Enable(true);
	pScreenROI->Enable(true);
	pScreenZoom->Enable(true);
	pScreenPOI->Enable(true);
	pScreenMaxPointer->Enable(true);
	pScreenMinPointer->Enable(true);

	pDataTriggerFlag->Enable(1);
	
	if (pDoc->m_TriggerOnOff)
	{
		pDataAcq_Trigger->Enable(1);
		pDataAcq_TriggerDist->AllowEdit(TRUE);
	}
	else
	{
		pDataAcq_Trigger->Enable(0);
		pDataAcq_TriggerDist->AllowEdit(FALSE);
	}
	
	pRawDataSaveEnable->Show(FALSE);

	//보여줄 것이냐 or 보여주지 않을 것이냐
	if (pDoc->m_OpenMode>1) {
		pDataSave->Show(TRUE);
		pMeasureData->Show(TRUE);
		pRawDataSaveEnable->Show(FALSE);
		pScreen->Show(TRUE);
	}
	else {
		pDataSave->Show(FALSE);
		pMeasureData->Show(TRUE);
		pScreen->Show(TRUE);
		pMeasureDataAutoETMode->Enable(FALSE);
	}

	//활성화 or 비활성화
	if (pDoc->m_OpenMode>1 && !theApp.m_bLoggingRunning)
	{
		//Data Logging
		pRawDataSaveEnable->Enable(TRUE);
		pRawDataSaveFilePath->Enable(TRUE);
		pResultDataSaveFilePath->Enable(TRUE);
	}
	else {
		//Data Logging
		pRawDataSaveEnable->Enable(FALSE);
		pRawDataSaveFilePath->Enable(FALSE);
		pResultDataSaveFilePath->Enable(FALSE);

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
	unsigned short Status;

	CString s1, s2;
	CString	strDate, strTime, strPos;

	theApp.DDAQ_IRDX_FILE_GetCurDataSet(pDoc->m_hIRDX_Doc, &(pDoc->m_IdxDS));
	theApp.DDAQ_IRDX_DEVICE_GetDetectorTemp(pDoc->m_hIRDX_Doc, &temp1, &ok);
	theApp.DDAQ_IRDX_DEVICE_GetCameraTemp(pDoc->m_hIRDX_Doc, &temp2, &ok);
	theApp.DDAQ_IRDX_ACQUISITION_GetTimeStampString(pDoc->m_hIRDX_Doc, strDate, strTime);
	if (theApp.m_FocusFlag && pDoc->m_OpenMode == 3)
		theApp.DDAQ_IRDX_ACQUISITION_GetMotorFocusPos(pDoc->m_hIRDX_Doc, &Pos, &Status);

	//pDoc->m_IdxDS = m_CurrDR; 
	s1.Format("%.1f ℃", temp1);
	s2.Format("%.1f ℃", temp2);
	strPos.Format("%d", Pos);

	pFile_CurDR->SetValue((_variant_t)(pDoc->m_IdxDS + 1));
	pMeasureera_DTemp->SetValue((_variant_t)(s1));
	pMeasureera_CTemp->SetValue((_variant_t)(s2));
	pDataAcq_Date->SetValue((_variant_t)(strDate));
	pDataAcq_Time->SetValue((_variant_t)(strTime));
	pDataAcqIndex->SetValue((_variant_t)(pDoc->m_AcqIndex));
	pFocusLocation->SetValue((_variant_t)strPos);
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

unsigned short CPropertiesWnd::valueOfAverage(CString str, float FPS)
{
	float fav;
	unsigned short avg = 1;
	CString s;

	for (int i = 1; i<10; i++) {
		fav = FPS / avg;
		s.Format("%d (%.1f Hz)", avg, fav);

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
		else if (iVal < 0)
			pDoc->m_ROICount = 0;
		else
			pDoc->m_ROICount = iVal;

		str.Format("%d", pDoc->m_ROICount);
		pMeasureDataROICount->SetValue(str);

		pDoc->m_ROI_check_flag = true;
		pDoc->columnUpdateFlag = true;

		pDoc->UpdateAllViews(NULL, 2);
	}
	else if (pProp == pDataAcq_TriggerDist)
	{
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		float fVal = _ttof(CString(pbstr));
		CString str;

		if (fVal > 5000)
			pDoc->m_TriggerDist = 5000;
		else if (fVal < 0)
			pDoc->m_TriggerDist = 0;
		else
			pDoc->m_TriggerDist = fVal;

		str.Format("%.0f", pDoc->m_TriggerDist);
		pDataAcq_TriggerDist->SetValue(str);

		pDoc->UpdateAllViews(NULL, 2);
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
	else if (pProp == pMeasureDataAutoETMode)
	{
		pDoc->m_Auto_ET_Mode = pMeasureDataAutoETMode->GetValue();

		pDoc->UpdateAllViews(NULL, 8);	// update 8번이 무슨 기능인지 보자
	}
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

		if (!compName.Compare(strZoomMode[0]))			//Auto size
		{
			pDoc->m_ZoomMode = 0;
			pDoc->m_ZoomRatio = 1;
		}
		else if (!compName.Compare(strZoomMode[1]))		//100%
		{
			pDoc->m_ZoomMode = 1;
			pDoc->m_ZoomRatio = 1;
		}
		else if (!compName.Compare(strZoomMode[2]))		//125%
		{
			pDoc->m_ZoomMode = 2;
			pDoc->m_ZoomRatio = 1.25;
		}
		else if (!compName.Compare(strZoomMode[3]))		//150%
		{
			pDoc->m_ZoomMode = 3;
			pDoc->m_ZoomRatio = 1.5;
		}
		else if (!compName.Compare(strZoomMode[4]))		//200%
		{
			pDoc->m_ZoomMode = 4;
			pDoc->m_ZoomRatio = 2;
		}
		else if (!compName.Compare(strZoomMode[5]))		//300%
		{
			pDoc->m_ZoomMode = 5;
			pDoc->m_ZoomRatio = 3;
		}
		else if (!compName.Compare(strZoomMode[6]))		//400%
		{
			pDoc->m_ZoomMode = 6;
			pDoc->m_ZoomRatio = 4;
		}
		//else if (!compName.Compare(strZoomMode[7]))		//ROI size
		//	pDoc->m_ZoomMode = 7;
		else
			pDoc->m_ZoomMode = 0;

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
		pDoc->m_TriggerOnOff = pDataTriggerFlag->GetValue();

		if (pDoc->m_TriggerOnOff)
		{
			CString s;
			s.Format("%.0f", pDoc->m_TriggerDist);

			pDataAcq_TriggerDist->SetValue((_variant_t)(s));

			pDataAcq_Trigger->Enable(1);
			pDataAcq_TriggerDist->AllowEdit(TRUE);
		}
		else
		{
			pDataAcq_TriggerDist->SetValue((_variant_t)("--"));
			
			pDataAcq_Trigger->Enable(0);
			pDataAcq_TriggerDist->AllowEdit(FALSE);
		}

		pDoc->UpdateAllViews(NULL, 8);
	}
	else if (pProp == pDataAcq_Trigger)
	{
		pDoc->m_NearFromDev = pDataAcq_Trigger->GetValue();

		pDoc->UpdateAllViews(NULL, 8);
	}
	// Acquistion **************************************************************
	else if (pProp == pDataAcq_AcqFreq) {
		USES_CONVERSION;
		pbstr = OLE2A(var.bstrVal);
		pDoc->m_Avg = valueOfAverage(CString(pbstr), pDoc->m_FPS);

		if (!theApp.DDAQ_DEVICE_DO_Stop(pDoc->m_nDDAQDeviceNo))
			return 0;

		theApp.DDAQ_IRDX_ACQUISITION_SetAveraging(pDoc->m_hIRDX_DEVICE, pDoc->m_Avg);

		if (!theApp.DDAQ_DEVICE_DO_Start(pDoc->m_nDDAQDeviceNo))
			return 0;
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
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////


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