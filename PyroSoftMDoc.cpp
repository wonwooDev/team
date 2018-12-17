
// PyroSoftMDoc.cpp : CPyroSoftMDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "PyroSoftM.h"
#endif

#include "PyroSoftMDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "GlassFlowView.h"
#include "ResultView.h"
#include "ResultDlg.h"	
#include "ROIGridView.h"
#include "ScaleView.h"
#include "windows.h"
#include "MMSystem.h" 
#include <afxmt.h>
#include <propkey.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT		nSendResultTimerID;
RESULT_T	*pResultData;
CTime		R_DateTime;

#define BOXCAR				1
#define	DISCRIMINATION_NUM	4
#define INCLINATION_NUM		4

CCriticalSection g_cs;

// CPyroSoftMDoc

IMPLEMENT_DYNCREATE(CPyroSoftMDoc, CDocument)

BEGIN_MESSAGE_MAP(CPyroSoftMDoc, CDocument)
	ON_COMMAND(ID_DEVICE_DO_START, &CPyroSoftMDoc::OnDeviceLoggingStart)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_DO_START, &CPyroSoftMDoc::OnUpdateDeviceDoStart)
	ON_COMMAND(ID_DEVICE_DO_STOP, &CPyroSoftMDoc::OnDeviceLoggingStop)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_DO_STOP, &CPyroSoftMDoc::OnUpdateDeviceDoStop)
	ON_COMMAND(ID_DATAACQUISITION_SHOWMIN, &CPyroSoftMDoc::OnDataacquisitionShowmin)
	ON_UPDATE_COMMAND_UI(ID_DATAACQUISITION_SHOWMIN, &CPyroSoftMDoc::OnUpdateDataacquisitionShowmin)

	ON_COMMAND(ID_FOCUS_N, &CPyroSoftMDoc::OnButtonFocusN)
	ON_UPDATE_COMMAND_UI(ID_FOCUS_N, &CPyroSoftMDoc::OnUpdateFocusN)
	ON_COMMAND(ID_FOCUS_F, &CPyroSoftMDoc::OnButtonFocusF)
	ON_UPDATE_COMMAND_UI(ID_FOCUS_F, &CPyroSoftMDoc::OnUpdateFocusF)
	ON_COMMAND(ID_FOCUS_N_STEP, &CPyroSoftMDoc::OnButtonFocusNStep)
	ON_UPDATE_COMMAND_UI(ID_FOCUS_N_STEP, &CPyroSoftMDoc::OnUpdateFocusNStep)
	ON_COMMAND(ID_FOCUS_F_STEP, &CPyroSoftMDoc::OnButtonFocusFStep)
	ON_UPDATE_COMMAND_UI(ID_FOCUS_F_STEP, &CPyroSoftMDoc::OnUpdateFocusFStep)

	ON_COMMAND(ID_DATAPLAYER_PREVIOUSRECORD, &CPyroSoftMDoc::OnDataplayerPreviousrecord)
	ON_UPDATE_COMMAND_UI(ID_DATAPLAYER_PREVIOUSRECORD, &CPyroSoftMDoc::OnUpdateDataplayerPreviousrecord)
	ON_COMMAND(ID_DATAPLAYER_NEXTRECORD, &CPyroSoftMDoc::OnDataplayerNextrecord)
	ON_UPDATE_COMMAND_UI(ID_DATAPLAYER_NEXTRECORD, &CPyroSoftMDoc::OnUpdateDataplayerNextrecord)
	ON_COMMAND(ID_DATAPLAYER_PLAY, &CPyroSoftMDoc::OnDataplayerPlay)
	ON_UPDATE_COMMAND_UI(ID_DATAPLAYER_PLAY, &CPyroSoftMDoc::OnUpdateDataplayerPlay)
	ON_COMMAND(ID_DATAPLAYER_STOP, &CPyroSoftMDoc::OnDataplayerStop)
	ON_UPDATE_COMMAND_UI(ID_DATAPLAYER_STOP, &CPyroSoftMDoc::OnUpdateDataplayerStop)

	ON_COMMAND(ID_LEFT_SIDE_BTN, &CPyroSoftMDoc::OnSwitchLeftSide)
	ON_UPDATE_COMMAND_UI(ID_LEFT_SIDE_BTN, &CPyroSoftMDoc::OnUpdateSwitchLeftSide)
	ON_COMMAND(ID_RIGHT_SIDE_BTN, &CPyroSoftMDoc::OnSwitchRightSide)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_SIDE_BTN, &CPyroSoftMDoc::OnUpdateSwitchRightSide)

	//ON_COMMAND(ID_ROI_DRAW, &CPyroSoftMDoc::OnPOIDraw)
	//ON_UPDATE_COMMAND_UI(ID_ROI_DRAW, &CPyroSoftMDoc::OnUpdatePOIDraw)
	//ON_COMMAND(ID_ROI_DELETE, &CPyroSoftMDoc::OnPOIDelete)
	//ON_UPDATE_COMMAND_UI(ID_ROI_DELETE, &CPyroSoftMDoc::OnUpdatePOIDelete)
	//ON_COMMAND(ID_ROI_MOVE, &CPyroSoftMDoc::OnRoiRefMove)
	//ON_UPDATE_COMMAND_UI(ID_ROI_MOVE, &CPyroSoftMDoc::OnUpdateRoiRefMove)

	ON_COMMAND(ID_ZOOM_IN, &CPyroSoftMDoc::OnButtonZoomin)
	ON_COMMAND(ID_ZOOM_OUT, &CPyroSoftMDoc::OnButtonZoomout)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CPyroSoftMDoc::OnUpdateButtonZoomin)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CPyroSoftMDoc::OnUpdateButtonZoomout)
END_MESSAGE_MAP()


// CPyroSoftMDoc 생성/소멸

CPyroSoftMDoc::CPyroSoftMDoc()
{
	BROI = new CBROI();
	//POI = new CPROI();
	//LOI = new CLROI();
	//ROI = new CTROI();
	for (int i = 0; i < MAX_ROI_CNT; i++)
		TROI[i] = new CTROI();
	
	for (int i = 0; i < MAX_EROI_CNT; i++)
		EROI[i] = new CEROI();

	m_GFV_Hwnd = NULL;

	m_RHKStream = NULL;

	m_OpenMode = 1;					// 1:IRDX, 2:Simulation, 3:Device

	m_nDDAQDeviceNo = 0;			///< invalid or offline
	m_hIRDX_DEVICE = INVALID_HANDLE_VALUE;

	m_bAcqIsRunning = false;

	m_bSpreadCondition = true;		// Spread Condition "OK" == true, "NG" == false

	m_bStretchDIB = true;
	m_bUpdateProperty = false;

	m_AppendIRDXFile = true;
	m_SaveEveryRecord = false;

	m_bDataPlay = false;
	m_bRawDataSave = false;
	m_bMaxPointEnable = false;
	m_RefAreaFlag = false;
	m_ChartFlag = false;
	m_RHK_logging = false;
	m_bShowPointer = true;
	m_RHK_log_done = false;
	columnUpdateFlag = false;
	m_ROI_check_flag = false;
	m_bPreviousBtnClick = false;		// IRDX 화면 이전 버튼

	//// Property Flags ////
	m_ZoneInfoEnable = false;			// Result Dlg의 Zone Info 버튼의 활성화 여부
	m_Auto_ET_Mode = false;				// 자동 방사율,투과율 모드
	indexMoveFlag = false;
	m_cursorShow = false;				// 커서 온도값 show
	m_ROIShow = false;					// 관심영역 show
	m_maxPointerShow = false;			// 최대 온도값 show
	m_minPointerShow = false;			// 최소 온도값 show
	m_triggerLoggingFirst = false;
	m_isStopMotion = false;

	m_hIRDX_Doc = INVALID_HANDLE_VALUE;
	m_hIRDX_READ = INVALID_HANDLE_VALUE;
	m_hIRDX_WRITE = INVALID_HANDLE_VALUE;

	m_hEventData = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_NoDS = m_IdxDS = 0;
	
	m_MaxTemp = m_MinTemp = 0.0f;
	m_FPS = 0.0f;
	m_Emissivity = m_Transmission = m_AmbTemp = 0.0f;

	m_nBar = 0;
	m_NoCol = 256;
	m_ScaleMin = 0.0f;
	m_ScaleMax = 100.0f;
	m_ScaleIsoTherm = 0.0f;

	m_AcqIndex = 0;

	m_PixelX = m_PixelY = 0;

	m_RHKDistance = 0;
	m_RHKPreDistance = 0;
	m_RHKTotalDistance = 0;
	m_RHKZoneCount = 0;
	m_FirstCeoff = 0.0f;
	m_SecondCeoff = 0.0f;
	m_ThirdCeoff = 0.0f;
	m_Intercept = 0;

	m_Thermal_Gap = 0;
	m_ROICount = 0;
	m_Font_Size = 1;	// 폰트 최소 사이즈

	m_ET_dataCount = 0;
	m_ET_idx = 0;
	m_POI_count = 0;

	m_bFrameNum = 0;
	m_RcpStartDist = 0;

	m_OOIShowNum = 0;
	m_POIShowNum = 0;
	m_max_idxDS = 0;

	m_comp_ROICount = 0;

	pFrameBuffer = NULL;

	m_BaseROI.lx = 0;
	m_BaseROI.ty = 0;
	m_BaseROI.rx = 0;
	m_BaseROI.by = 0;

	m_ExROICnt =0;

	m_SelXPxlChrt_idx = 0;
	m_SelYPxlChrt_idx = 0;

	m_row_switch_count = 0;

	m_low_temp_idx = 0;
	m_lx_pxl_avg_idx = 0;

	m_ROI_loop_count = 0;

	m_camStatus = 0;

	m_ROIBufferCnt = 0;
	m_ROIBufferLimit = 0;
	m_ROICatchCnt = 0;

	limitOfTemperOver = 0;
	gapOfPosition = 0;
	limitOfXMaintain = 0;

	m_bCheckSpread = false;
	m_spreadTempr = 0;
	m_spreadMaxTempr = 0;
	m_spreadDetctRange = 0;
	m_spreadlimitTempr = 0;

	// varables in the calROI function 
	//totalIndexCount = 0;

	// Recipe
	m_NumOfZone = 0;
	m_RowNum = 0;
	m_ColNum = 0;

	// Zoom
	m_ZoomMode = 1;			//100%
	m_ZoomRatio = 0.0f;

	m_BROI_minSize = 10;		// Base ROI init size

	// Step Diff
	m_upStepCoeff = 0.0f;
	m_middleStepCoeff = 0.0f;
	m_downStepCoeff = 0.0f;

	m_POIArr.SetSize(0);
	RunningIndex.SetSize(0);
	DeletedIndex.SetSize(0);

	for (int i = 0; i < MAX_POI_NUM; i++)
		POI_TemperatureArray[i] = 0;

	for (int i = 0; i<MAX_ROI_CNT; i++)
	{
		TROI[i]->SetPosXY(X_LEFT, 0);
		TROI[i]->SetPosXY(X_RIGHT, 0);
		TROI[i]->SetPosXY(Y_TOP, 0);
		TROI[i]->SetPosXY(Y_BOTTOM, 0);

		temptt[i] = 0;

		m_ROIBuffer[i] = 0;
	}

	for (int i = 0; i < MAX_ET_PER_DISTANCE; i++)
	{
		m_ET_Distance[i] = 0;
		m_ET_Emissivity[i] = 0;
		m_ET_Transmission[i] = 0;
	}
	for (int i = 0; i < MAX_ROI_CNT * 2; i++)
	{
		m_SelXPixelChart[i] = 0;
		m_SelYPixelChart[i] = 0;
	}
	for (int i = 0; i < MAX_X_PIXEL; i++)
	{
		m_row_projection[i] = 0;
		m_lRow_projection[i] = 0;
	}

	for (size_t i = 0; i < MAX_Y_PIXEL; i++)
		m_col_projection[i] = 0;

	for (int i = 0; i < MAX_DISCRIMINANT_NUM; i++)
	{
		m_discrim_arr[i] = 0;
		m_inclin_arr[i] = 0;
	}
	for (int i = 0; i < MAX_ROI_CNT; i++)
	{
		m_baseROILXTemp[i] = 0;
		m_baseROIRXTemp[i] = 0;

		for (int j = 0; j < MAX_PIXEL_AVG_NUM; j++)
		{
			m_lx_pxl_avg[i][j] = 0;
			m_rx_pxl_avg[i][j] = 0;
			m_ty_pxl_avg[i][j] = 0;
			m_by_pxl_avg[i][j] = 0;
		}
	}
	
	for (int i = 0; i < MAX_ZONE; i++)
	{
		m_ZoneDistance[i] = 0;
		m_ZoneSpeed[i] = 0;
		m_ZoneEmissivity[i] = 0;
	}

	m_ResultData.Min = m_ResultData.Max = m_ResultData.Avg = 600.0f;

	pResultData = m_Result;

	// 승태 작업부분
	for (int k = 0; k < MAX_ROI_CNT; k++) {
		bShouldWrite[k] = true;
	}
	for (int k = 0; k < MAX_EROI_CNT; k++) {
		exclusived[k].lx = 0;
		exclusived[k].rx = 0;
		exclusived[k].ty = 0;
		exclusived[k].by = 0;
	}
	zoom_inAuto = 0.0f;
}

void CALLBACK ClientTimer(UINT m_nTimerID, UINT uiMsg, DWORD dwUser, DWORD dw1, DWORD d2) {
	theApp.m_StatusInfFlag = true;
	theApp.m_TchartFlag = true;
}

void CALLBACK LoggingIntervalTimer(UINT m_nTimerID, UINT uiMsg, DWORD dwUser, DWORD dw1, DWORD d2) {
	theApp.m_TimerFlag = true;
}


CPyroSoftMDoc::~CPyroSoftMDoc()
{
	if (pFrameBuffer != NULL)
		delete[] pFrameBuffer;
	
	CloseHandle(m_hEventData);
}

void CPyroSoftMDoc::GetCamNBufferSize()
{
	//unsigned short sizex=0, sizey=0;
	if (!theApp.DDAQ_IRDX_PIXEL_GetSize(m_hIRDX_Doc, &sizeX, &sizeY))
		return;

	// alloc buffer
	unsigned long NumPix = sizeX*sizeY;
	BufSize = NumPix * sizeof(float);

	if (pFrameBuffer != NULL)
		delete[] pFrameBuffer;

	pFrameBuffer = new float[NumPix];
}

// for online device
void CPyroSoftMDoc::CalMeasurement(int index, int sizeX, int sizeY, float* pBuffer)
{
	CalROI(index, sizeX, sizeY, pBuffer);
}

/////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::SetDDAQDeviveNo(unsigned long nDevNo)
{
	m_nDDAQDeviceNo = nDevNo;

	// get IRDX Handle! 
	if (!theApp.DDAQ_DEVICE_GET_IRDXHandle(m_nDDAQDeviceNo, &m_hIRDX_DEVICE))
		return;

	m_hIRDX_Doc = m_hIRDX_DEVICE;
	m_DeviceString.Format(IDS_DEVICE_NO, m_nDDAQDeviceNo);
	m_DeviceString += " / ";

	// get device id string
	CString s;
	if (!theApp.DDAQ_DEVICE_GET_IDString(m_nDDAQDeviceNo, s))
		return;
	m_DeviceString += s;
	m_DeviceString += " / ";

	// get interface string
	if (!theApp.DDAQ_DEVICE_GET_InterfaceString(m_nDDAQDeviceNo, s))
		return;
	m_DeviceString += s;
}

/////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::GetAverageString(unsigned short av, CString& s)
{
	s.Empty();

	// get internal fps without averaging
	float intern_fps;
	if (!theApp.DDAQ_IRDX_DEVICE_GetInternalFPS(m_hIRDX_Doc, &intern_fps))
	{
		// without "Hz"
		s.Format(IDS_FMT_AVERAGE0, av);
		return;
	}

	float fav = intern_fps / av;
	if (fav >= 1.0f)
	{
		s.Format(IDS_FMT_AVERAGE1, av, fav);
	}
	else
	{
		if (fav > 0.0f)
			fav = 1.0f / fav;
		s.Format(IDS_FMT_AVERAGE2, av, fav);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPyroSoftMDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}

#ifdef SHARED_HANDLERS
#endif // SHARED_HANDLERS

// CPyroSoftMDoc 진단

#ifdef _DEBUG
void CPyroSoftMDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPyroSoftMDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

// CPyroSoftMDoc 명령

/////////////////////////////////////////////////////////////////////////////
BOOL CPyroSoftMDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if (!theApp.DDAQ_IRDX_FILE_OpenRead((char*)lpszPathName, &m_hIRDX_READ))
		return FALSE;
	//if (!theApp.DDAQ_IRDX_FILE_OpenReadWrite((char*)lpszPathName, true, &m_hIRDX_READ))
	//	return FALSE;

	m_hIRDX_Doc = m_hIRDX_READ;

	// get device id string from irdx!
	theApp.DDAQ_IRDX_DEVICE_GetIDString(m_hIRDX_Doc, m_DeviceString);

	m_OpenMode = 1;
	theApp.IRDX_Mode = 1;

	/////////////////////////////////////////////////
	//if (!theApp.m_bFileOpen)
	//{
		if (!theApp.DDAQ_IRDX_PIXEL_GetSize(m_hIRDX_Doc, &sizeX, &sizeY))
			return FALSE;

		POSITION pos = GetFirstViewPosition();
		CView* pView;
		CView* pPView;

		pView = GetNextView(pos);
		pView = GetNextView(pos);
		((CScaleView *)pView)->pDoc = this;
		((CScaleView *)pView)->m_ScaleDlg.pDoc = this;

		pView = GetNextView(pos);
		pPView = pView;
		((CROIGridView *)pView)->pDoc = this;
		((CROIGridView *)pView)->m_ROIGridDlg.pDoc = this;

		pView = GetNextView(pos);
		((CResultView *)pView)->pDoc = this;

		LoadConfig();
		LoadRecipeConfig();
		LoadETPerDistance();

		((CResultView *)pView)->m_ResultDlg.pDoc = this;

		((CResultView *)pView)->m_MaxTabDlg.pDoc = this;
		((CResultView *)pView)->m_SpreadTabDlg.pDoc = this;
		((CResultView *)pView)->m_ResultDlg.m_SpreadTabDlg.pDoc = this;
		//((CResultView *)pView)->m_AvgTabDlg.pDoc = this;
		((CResultView *)pView)->InitROIData();
		((CROIGridView *)pPView)->InitStatusData();
	//}
	///////////////////////////////////////////////////
		theApp.pDocTemp = this;
		
	theApp.m_bFileOpen = TRUE;		// IRDX 파일 열때 한번 TRUE 변환 (초기값 false)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::OnIdle()
{
	// if messaging no idle time procedure!
	if (theApp.GetMessagingType() == Msg_No)
	{
		if (!m_nDDAQDeviceNo || !m_bAcqIsRunning)
			return;

		// get new data ready flag
		bool NewDataReady = false;
		if (!theApp.DDAQ_DEVICE_GET_NewDataReady(m_nDDAQDeviceNo, &NewDataReady))
		{
			//OnDeviceLoggingStop();
			OnDeviceDoStop();
			return;
		}

		// if not ready, do nothing
		if (!NewDataReady)
			return;

		// update new data!
		//if(m_bLoggingRunning)
		OnNewDataReady();

		return;
	}

	// if notification via event ==> call WaitForSingleObject()
	// not the really right place here, but for demonstartion a simply way!
	if (theApp.GetMessagingType() == Msg_Event)
	{
		DWORD dwResult = WaitForSingleObject(m_hEventData, 0);
		if (dwResult == WAIT_OBJECT_0)
		{
			// update new data!
			//if (m_bLoggingRunning)
			OnNewDataReady();
		}
	}
}

void CPyroSoftMDoc::IRDXUpdate()
{
	GetCamNBufferSize();

	// Update Main View //////////////////////////
	UpdateAllViews(NULL, 1);
	
	if (pFrameBuffer == NULL)
	{
		AfxMessageBox(IDS_ERROR_ALLOCMEM);
		return;
	}

	// get data
	if (!theApp.DDAQ_IRDX_PIXEL_GetData(m_hIRDX_Doc, pFrameBuffer, BufSize))
	{
		delete[] pFrameBuffer;
		return;
	}
	
	// Extract Spot Area ****************************************************

	CalculateResult(PPointArr.GetCount(), sizeX, sizeY, pFrameBuffer);

	if(!m_bDataPlay)
		theApp.m_pPropertyWnd->UpdateDataSet();

	// Update GlassView ///////////////
	UpdateAllViews(NULL, 3);

	theApp.DDAQ_DEVICE_DO_ENABLE_NextMsg(m_nDDAQDeviceNo);
}

double CPyroSoftMDoc::getLineAvgTemp(float *pBuf, unsigned short size)
{
	double sum = 0.0;

	if (size <= 0) return 0;

	for (unsigned short i = 0; i<size; i++) {
		sum += pBuf[i];
	}

	return sum / size;
}

// for irdx file
void CPyroSoftMDoc::CalculateResult(int index, int sizeX, int sizeY, float* pBuffer)
{
	/// pBuffer의 주소값이 아닌 안의 배열에 값이 존재 하지 않는 조건문으로 변경해야한다.
	//if(pBuffer != NULL)
	CalROI(index, sizeX, sizeY, pBuffer);
}
//
///////  ROI 계산 ////////  
//void CPyroSoftMDoc::CalROI(int index, int sizeX, int sizeY, float* pBuffer)
//{
//	/// pBuffer의 주소값이 아닌 안의 배열에 값이 존재 하지 않는 조건문으로 변경해야한다.
//	bool isNext_LxPos = TRUE;			// bool 배열로 함수인자를 2개에서 1개로 줄임
//	bool isNext_RxPos = FALSE;
//	bool isNext_TyPos = TRUE;
//	bool isNext_ByPos = FALSE;
//	float temp_val = 0;
//	int temp_posLeftX, temp_posRightX, temp_posTopY, temp_posBottomY;	// 임시 온도값 저장
//	
//	// ROI 좌표 정보를 넘겨줄 배열
//	int arr_PosLeftX[MAX_ROI_CNT] = { 0 };
//	int arr_PosRightX[MAX_ROI_CNT] = { 0 };
//	int arr_PosTopY[MAX_ROI_CNT] = { 0 };
//	int arr_PosBottomY[MAX_ROI_CNT] = { 0 };
//
//	// 임시적으로 ROI 좌표 정보를 가지고 있을 배열
//	int arr_tempPosLeftX[MAX_ROI_CNT] = { 0 };
//	int arr_tempPosRightX[MAX_ROI_CNT] = { 0 };
//	int arr_tempPosTopY[MAX_ROI_CNT] = { 0 };
//	int arr_tempPosBottomY[MAX_ROI_CNT] = { 0 };
//
//	// Gap 차이 -> countOfRXMaintain 위해
//	int arr_distctLeftX[MAX_ROI_CNT] = { 0 };
//	int arr_distctRightX[MAX_ROI_CNT] = { 0 };
//	int arr_distctTopY[MAX_ROI_CNT] = { 0 };
//	int arr_distctBottomY[MAX_ROI_CNT] = { 0 };
//
//	int idx_tempPosX = 0,pre_idx_tempPosX = 0, idx_tempPosY = 0;
//
//	// 추후 변수 위치 & 변수 값 수정 필요
//	int totalIndexCount = 0;						// x축을 쭉 스캔 했을 시 저장 된 총 ROI 횟수
//	int pre_totalIndexCnt = 0;
//	int countOfTemperOver = 0;						// x축을 스캔 할 시 임계온도를 넘은 횟수
//	int countOfLXMaintain[MAX_ROI_CNT] = { 0 };		// ROI의 left_x축이 몇번 y축 방향으로 유지 된 횟수
//	int countOfRXMaintain[MAX_ROI_CNT] = { 0 };		// ROI의 right_x축이 몇번 y축 방향으로 유지 된 횟수
//
//	bool isArrFirst[MAX_ROI_CNT] = { 0 };
//	int	sumTemp[MAX_ROI_CNT] = { 0 };
//	int	pixelCount[MAX_ROI_CNT] = { 0 };
//
//	int pre_min_x[MAX_ROI_CNT] = { 0 };
//	int pre_max_x[MAX_ROI_CNT] = { 0 };
//	int pre_min_y[MAX_ROI_CNT] = { 0 };
//	int pre_max_y[MAX_ROI_CNT] = { 0 };
//
//	float pre_TMin[MAX_ROI_CNT] = { 0.0 };
//	float pre_TMax[MAX_ROI_CNT] = { 0.0 };
//
//	for (int i = 0; i < MAX_ROI_CNT; i++)			// 함수로 뺄것
//	{
//		arr_PosLeftX[i] = 0;
//		arr_PosRightX[i] = 0;
//		arr_PosTopY[i] = 0;
//		arr_PosBottomY[i] = 0;
//
//		arr_tempPosLeftX[i] = 0;
//		arr_tempPosRightX[i] = 0;
//		arr_tempPosTopY[i] = 0;
//		arr_tempPosBottomY[i] = 0;
//
//		isArrFirst[i] = TRUE;
//		sumTemp[i] = 0;
//		pixelCount[i] = 0;
//
//		pre_min_x[i] = 0;
//		pre_max_x[i] = 0;
//		pre_min_y[i] = 0;
//		pre_max_y[i] = 0;
//
//		m_ResultData.TMin[i] = 0;
//		m_ResultData.TMinX[i] = 0;
//		m_ResultData.TMinY[i] = 0;
//
//		m_ResultData.TMax[i] = 0;
//		m_ResultData.TMaxX[i] = 0;
//		m_ResultData.TMaxY[i] = 0;
//	}
//
//	for (int y = 0; y < sizeY; y++)
//	{
//		for (int x = 0; x < sizeX; x++)
//		{
//			temp_val = pBuffer[sizeX*y + x];
//
//			// Threashold 보다 높은 값만 계산
//			if (temp_val < m_Threshold)
//			{
//				// 설정한 임계온도 보다 높은 점을 일정 이상 반복하였을 시 오른쪽 X 좌표를 저장 하기 위한 조건
//				if (isNext_RxPos && countOfTemperOver > limitOfTemperOver)
//				{
//					arr_tempPosRightX[idx_tempPosX++] = x;
//					isNext_LxPos = TRUE;
//					isNext_RxPos = FALSE;
//
//					countOfTemperOver = 0;
//				}
//				else
//				{
//					arr_tempPosLeftX[idx_tempPosX] = 0;
//					isNext_LxPos = TRUE;
//					isNext_RxPos = FALSE;
//				}
//				
//				continue;
//			}
//			
//			// 임계 온도 보다 높은 기준을 충족 시
//			countOfTemperOver++;		
//			
//			// temporary n번째 ROI left_x값을 계산 하는 조건이 만족 하느냐?
//			if (isNext_LxPos)		
//			{
//				if (x != 0 ) {
//					arr_tempPosLeftX[idx_tempPosX] = x;
//					isNext_LxPos = FALSE;
//					isNext_RxPos = TRUE;
//				}
//			}
//		}
//
//		if (idx_tempPosX > 2)	// 맨 가에 2개의 불필요한 ROI 제거
//		{
//			bool firstComp = TRUE;
//			int max, min;
//			int compArr[MAX_ROI_CNT] = { 0 };
//
//			for (int i = 1; i < idx_tempPosX - 1; i++)
//			{
//				compArr[i] = arr_tempPosRightX[i] - arr_tempPosLeftX[i];
//
//				if (firstComp)
//				{
//					firstComp = FALSE;
//
//					max = compArr[i];
//					min = compArr[i];
//				}
//				else
//				{
//					if (compArr[i] > max)
//						max = compArr[i];
//
//					if (compArr[i] < min)
//						min = compArr[i];
//				}
//			}
//
//			//if (max - min < 3 && max != min)
//			if (max - min < 3 && idx_tempPosX > 3)
//			{
//				if (totalIndexCount == 0)
//				{
//					totalIndexCount = idx_tempPosX;
//
//					arr_tempPosTopY[idx_tempPosY] = y;
//
//					for (int i = 0; i < totalIndexCount; i++)
//					{
//						arr_distctLeftX[i] = arr_tempPosLeftX[i];
//						arr_distctRightX[i] = arr_tempPosRightX[i];
//
//						arr_PosLeftX[i] = arr_tempPosLeftX[i];
//						arr_PosRightX[i] = arr_tempPosRightX[i];
//					}
//				}
//				else if (pre_idx_tempPosX == idx_tempPosX)
//				{
//					for (int i = 1; i < idx_tempPosX-1; i++)
//					{
//						if (abs(arr_tempPosLeftX[i] - arr_distctLeftX[i]) < gapOfPosition)		// +-gapPosition
//						{
//							if (arr_tempPosLeftX[i] - arr_distctLeftX[i] >= 0)
//							{
//								arr_distctLeftX[i] = arr_tempPosLeftX[i];
//								//arr_PosLeftX[i] = arr_distctLeftX[i];
//							}
//
//							countOfLXMaintain[i]++;
//						}
//						else
//						{
//							if (countOfLXMaintain[i] > limitOfXMaintain)
//								arr_PosLeftX[i] = arr_distctLeftX[i];
//
//							arr_distctLeftX[i] = arr_tempPosLeftX[i];
//						}
//
//
//						if (abs(arr_tempPosRightX[i] - arr_distctRightX[i]) < gapOfPosition)		// +-gapPosition
//						{
//							if (arr_tempPosRightX[i] - arr_distctRightX[i] <= 0)
//							{
//								arr_distctRightX[i] = arr_tempPosRightX[i];
//								//arr_PosRightX[i] = arr_distctRightX[i];
//							}
//
//							countOfRXMaintain[i]++;
//						}
//						else
//						{
//							if (countOfRXMaintain[i] > limitOfXMaintain)
//								arr_PosRightX[i] = arr_distctRightX[i];
//
//							arr_distctRightX[i] = arr_tempPosRightX[i];
//						}
//					}
//				}
//				else
//				{
//					int maintainCnt = 0;
//
//					for (int i = 0; i < idx_tempPosX; i++)
//					{
//						if (countOfLXMaintain[i] > limitOfXMaintain && countOfRXMaintain[i] > limitOfXMaintain)
//							maintainCnt++;
//
//					}
//
//					if (maintainCnt == (idx_tempPosX - 2))
//					{
//						totalIndexCount = idx_tempPosX;
//						arr_tempPosTopY[0] = y - (countOfLXMaintain[1] + countOfRXMaintain[1]) / 2;
//
//						for (int i = 0; i < totalIndexCount; i++)
//						{
//							//arr_PosLeftX[i] = arr_tempPosLeftX[i];
//							//arr_PosRightX[i] = arr_tempPosRightX[i];
//						}
//					}
//
//				}
//			}// End of "if (max - min < 3 && max != min)"
//		}// End of "if (idx_tempPosX > 2)"
//
//		for (int i = 0; i < idx_tempPosX; i++) {
//			arr_tempPosLeftX[i] = 0;
//			arr_tempPosRightX[i] = 0;
//		}
//
//		pre_idx_tempPosX = idx_tempPosX;	
//		idx_tempPosX = 0;
//		idx_tempPosY = 0;
//
//		countOfTemperOver = 0;
//
//		isNext_LxPos = TRUE;
//		isNext_RxPos = FALSE;
//	}
//
//	for (int i = 1; i < totalIndexCount; i++)
//	{
//		//if (isNext_ByPos && countOfLXMaintain[i] > limitOfXMaintain && countOfRXMaintain[i] > limitOfXMaintain)
//			arr_tempPosBottomY[idx_tempPosY++] = arr_tempPosTopY[0] + (countOfLXMaintain[1] + countOfRXMaintain[1]) / 2;;
//	}
//
//	// y값 데이터 저장
//	for (int i = 0; i < totalIndexCount; i++) {
//		arr_PosTopY[i] = arr_tempPosTopY[0];
//		arr_PosBottomY[i] = arr_tempPosBottomY[i];
//	}
//
//	for (int i = 0; i < totalIndexCount - 2; i++)
//	{
//		pre_min_x[i] = TROI_lx[i];
//		pre_max_x[i] = TROI_rx[i];
//		pre_min_y[i] = TROI_ty[i];
//		pre_max_y[i] = TROI_by[i];
//
//		TROI_lx[i] = arr_PosLeftX[i+1];
//		TROI_rx[i] = arr_PosRightX[i+1];
//		TROI_ty[i] = arr_PosTopY[i+1];
//		//if(arr_PosBottomY[i - 1] != 0)
//			//TROI_by[i] = arr_PosBottomY[i-1];
//		if(arr_PosBottomY[i] != 0)
//			TROI_by[i] = arr_PosBottomY[i];
//	}
//
//	// totalIndexCount 값 보다 큰 인덱스 초기화
//	if (totalIndexCount != 0)
//	{
//		for (int i = totalIndexCount - 2; i < MAX_ROI_CNT; i++) {
//			TROI_lx[i] = 0;
//			TROI_rx[i] = 0;
//			TROI_ty[i] = 0;
//			TROI_by[i] = 0;
//		}
//	}
//	else
//	{
//		for (int i = 0; i < MAX_ROI_CNT; i++)
//		{
//			TROI_lx[i] = 0;
//			TROI_rx[i] = 0;
//			TROI_ty[i] = 0;
//			TROI_by[i] = 0;
//		}
//	}
//
//
//	// 최대, 최소, 평균 온도값 찾기
//	for (int k = 0; k < totalIndexCount - 2; k++)
//	{
//		pre_TMin[k] = m_ResultData.TMin[k];
//		pre_TMax[k] = m_ResultData.TMax[k];
//
//		SearchTemperature(k, pBuffer, pixelCount, sumTemp, isArrFirst);
//
//		if (m_ResultData.TMin[k] < pre_TMin[k] - (pre_TMin[k] * 0.05))
//		{
//			TROI_lx[k] = pre_min_x[k];
//			TROI_rx[k] = pre_max_x[k];
//			TROI_ty[k] = pre_min_y[k];
//			TROI_by[k] = pre_max_y[k];
//
//			SearchTemperature(k, pBuffer, pixelCount, sumTemp, isArrFirst);
//		}
//
//		if(pixelCount[k] != 0)
//			m_ResultData.TAvg[k] = sumTemp[k] / pixelCount[k];
//	}
//
//	for (int i = 0; i < m_ROICount; i++)
//	{
//		if (m_ResultData.TMin[i] <= 0 || (TROI_lx[i] == 0 && TROI_ty[i] == 0 && TROI_rx[i] == 0 && TROI_by[i] == 0))
//			m_ResultData.TMin[i] = 0.0f;
//		else if (m_ResultData.TMin[i] < 600)
//			m_ResultData.TMin[i] = 600.0f;
//		if (m_ResultData.TMax[i] <= 0 || (TROI_lx[i] == 0 && TROI_ty[i] == 0 && TROI_rx[i] == 0 && TROI_by[i] == 0))
//			m_ResultData.TMax[i] = 0.0f;
//		if (m_ResultData.TAvg[i] <= 0 || (TROI_lx[i] == 0 && TROI_ty[i] == 0 && TROI_rx[i] == 0 && TROI_by[i] == 0))
//			m_ResultData.TAvg[i] = 0.0f;
//
//		if (TROI_ty[i] == TROI_by[i])
//		{
//			m_ResultData.TMin[i] = 0;
//			m_ResultData.TMinX[i] = 0;
//			m_ResultData.TMinY[i] = 0;
//
//			m_ResultData.TMax[i] = 0;
//			m_ResultData.TMaxX[i] = 0;
//			m_ResultData.TMaxY[i] = 0;
//			
//			m_ResultData.TAvg[i] = 0;
//		}
//	}
//
//	//m_ROI_loop_count = m_SelXPxlChrt_idx;
//	m_ROI_loop_count = totalIndexCount-2;
//
//}
//
//inline void CPyroSoftMDoc::SearchTemperature(int k, float *pBuffer, int *pixelCount, int *sumTemp, bool *isArrFirst)
//{
//	for (int j = TROI_ty[k]; j <= TROI_by[k]; j++)
//	{
//		for (int i = TROI_lx[k]; i <= TROI_rx[k]; i++)
//		{
//			float val = pBuffer[sizeX*j + i];
//
//			pixelCount[k]++;
//
//			sumTemp[k] += val;
//
//			if (isArrFirst[k]) {
//				m_ResultData.TMin[k] = m_ResultData.TMax[k] = val;
//				isArrFirst[k] = FALSE;
//			}
//			else {
//				if (val < m_ResultData.TMin[k])
//				{
//					m_ResultData.TMin[k] = val;
//					m_ResultData.TMinX[k] = i;
//					m_ResultData.TMinY[k] = j;
//				}
//				else if (val > m_ResultData.TMax[k])
//				{
//					m_ResultData.TMax[k] = val;
//					m_ResultData.TMaxX[k] = i;
//					m_ResultData.TMaxY[k] = j;
//				}
//			}
//		}
//	}
//}


/////  ROI 계산 ////////		
void CPyroSoftMDoc::CalROI(int index, int sizeX, int sizeY, float* pBuffer)
{
	/// pBuffer의 주소값이 아닌 안의 배열에 값이 존재 하지 않는 조건문으로 변경해야한다.

	int BROI_lx, BROI_rx, BROI_ty, BROI_by;
	int EROI_lx[MAX_EROI_CNT], EROI_rx[MAX_EROI_CNT], EROI_ty[MAX_EROI_CNT], EROI_by[MAX_EROI_CNT];
	int TROI_lx[MAX_ROI_CNT], TROI_rx[MAX_ROI_CNT], TROI_ty[MAX_ROI_CNT], TROI_by[MAX_ROI_CNT];
	int arr_count = 0, max_arr = 0, hit_count = 0;	
	int	m_baseROILXTmp_idx = 0;
	int m_baseROIRXTmp_idx = 0;
	int	m_discrim_idx = 0;
	int inclin_down = 0;
	int inclin_zero = 0;
	int inclin_up = 0;
	int m_col_max = 0;		
	int m_col_min = 0;		

	float sumTemp[MAX_ROI_CNT] = { 0.0f };
	int pixelCount[MAX_ROI_CNT] = { 0 };
	bool isArrFirst[MAX_ROI_CNT];
	bool isColFirst = false;
	bool m_inclin_up_flag = false;
	bool m_inclin_zero_flag = false;
	bool m_inclin_down_flag = false;
	int gapX = 0, gapY = 0;

	for (int i = 0; i < m_ROICount; i++)	// ROI XY 좌표의 min, max 값 초기화
	{
		m_ResultData.TMinX[i] = 0; m_ResultData.TMinY[i] = 0;
		m_ResultData.TMaxX[i] = 0; m_ResultData.TMaxY[i] = 0;
	}

	for (int i = 0; i < MAX_ROI_CNT; i++)
		isArrFirst[i] = true;

	for (int i = 0; i < 512; i++)
		m_row_projection[i] = 0;

	for (int i = 0; i < 384; i++)
		m_col_projection[i] = 0;

	for (int i = 0; i < m_SelXPxlChrt_idx; i++)
		m_SelXPixelChart[i] = 0;

	m_SelXPxlChrt_idx = 0;

	for (int i = 0; i < m_SelYPxlChrt_idx; i++)
		m_SelYPixelChart[i] = 0;

	m_SelYPxlChrt_idx = 0;

	m_ROI_loop_count = 0;

	InitROI(BROI_lx, BROI_rx, BROI_ty, BROI_by, BROI->GetPosXY(X_LEFT), BROI->GetPosXY(X_RIGHT), BROI->GetPosXY(Y_TOP), BROI->GetPosXY(Y_BOTTOM));

	for (int i = 0; i < MAX_ROI_CNT; i++)
		InitROI(TROI_lx[i], TROI_rx[i], TROI_ty[i], TROI_by[i], TROI[i]->GetPosXY(X_LEFT), TROI[i]->GetPosXY(X_RIGHT), TROI[i]->GetPosXY(Y_TOP), TROI[i]->GetPosXY(Y_BOTTOM));

	for (int i = 0; i < MAX_EROI_CNT; i++)
		InitROI(EROI_lx[i], EROI_rx[i], EROI_ty[i], EROI_by[i], EROI[i]->GetPosXY(X_LEFT), EROI[i]->GetPosXY(X_RIGHT), EROI[i]->GetPosXY(Y_TOP), EROI[i]->GetPosXY(Y_BOTTOM));

	for (int k = 0; k < MAX_EROI_CNT; k++) {
		exclusived[k].lx = 0;
		exclusived[k].rx = 0;
		exclusived[k].ty = 0;
		exclusived[k].by = 0;
	}

	/////////////////////////// 계산 /////////////////////////////////

	if (BROI_rx != 0 && BROI_by != 0)
	{
		for (int i = BROI_ty; i < BROI_by; i++)
		{
			for (int j = BROI_lx; j < BROI_rx; j++)
			{
				float val = pBuffer[sizeX*i + j];

				::CalArr(m_row_projection, val, j, (int)BROI_lx);

				if (val < m_Threshold)
				{
					if (!isColFirst)
					{
						isColFirst = true;
						m_col_max = i;
						m_col_min = i;
					}
					else if (i > m_col_max)
					{
						m_col_max = i;
					}
					else if (i < m_col_min)
					{
						m_col_min = i;
					}
					//m_col_projection[i - m_BaseROI.ty] += val;
				}
			}
		}

		//for (int i = BOXCAR; i < sizeof(m_row_projection) / sizeof(m_row_projection[0]) - BOXCAR; i++)	// x축의 projection 된 수 만큼
		//{

		//}

		CRect m_baseROI;
		m_baseROI.left = BROI_lx;
		m_baseROI.right = BROI_rx;
		m_baseROI.top = BROI_ty;
		m_baseROI.bottom = BROI_by;

		::CalMeasure(pBuffer, &m_baseROI, &m_baseROIRXTmp_idx, m_upStepCoeff, m_middleStepCoeff, m_downStepCoeff, m_baseROILXTemp, m_baseROIRXTemp);

		for (int i = 0; i < m_baseROIRXTmp_idx; i++)
		{
			if (abs(m_baseROILXTemp[i] - TROI_lx[m_SelXPxlChrt_idx]) < 2 || abs(m_baseROILXTemp[i] - TROI_lx[m_SelXPxlChrt_idx]) > 5)
				TROI[i]->SetPosXY(X_LEFT, m_baseROILXTemp[i]);
			if (abs(m_baseROIRXTemp[i] - TROI_rx[m_SelXPxlChrt_idx]) < 2 || abs(m_baseROIRXTemp[i] - TROI_rx[m_SelXPxlChrt_idx]) > 5)
				TROI[i]->SetPosXY(X_RIGHT, m_baseROIRXTemp[i]);

			TROI[m_SelXPxlChrt_idx]->SetPosXY(Y_TOP, m_col_min);
			TROI[m_SelXPxlChrt_idx]->SetPosXY(Y_BOTTOM, m_col_max);

			m_SelXPxlChrt_idx++;
		}

		for (int i = 0; i < MAX_EROI_CNT; i++)
		{
			TROI_lx[i] = TROI[i]->GetPosXY(X_LEFT);		TROI_rx[i] = TROI[i]->GetPosXY(X_RIGHT);
			TROI_ty[i] = TROI[i]->GetPosXY(Y_TOP);	TROI_by[i] = TROI[i]->GetPosXY(Y_BOTTOM);
		}
	
		for (int j = 0; j < MAX_EROI_CNT; j++)
		{
			for (int i = 0; i < m_SelXPxlChrt_idx; i++)
			{
				if (EROI[j]->GetDrawDone() == false)
					break;

				if (EROI_lx[j] > TROI_lx[i] && EROI_lx[j] < TROI_rx[i])
				{
					if (EROI_ty[j] < TROI_ty[i] && EROI_by[j] > TROI_by[i] || EROI_ty[j] > TROI_ty[i] && EROI_by[j] < TROI_by[i])
						TROI[i]->SetPosXY(X_RIGHT, EROI_lx[j]);
					else if (EROI_ty[j] < TROI_ty[i] && EROI_by[j] > TROI_ty[i])
					{
						TROI[i]->SetPosXY(X_RIGHT, EROI_lx[j]);
						TROI[i]->SetPosXY(Y_BOTTOM, EROI_by[j]);
					}
					else if (EROI_ty[j] < TROI_by[i] && EROI_by[j] > TROI_by[i])
					{
						TROI[i]->SetPosXY(X_RIGHT, EROI_lx[j]);
						TROI[i]->SetPosXY(Y_TOP, EROI_ty[j]);
					}
				}

				if (EROI_rx[j] > TROI_lx[i] && EROI_rx[j] < TROI_rx[i])
				{
					if (EROI_ty[j] < TROI_ty[i] && EROI_by[j] > TROI_by[i] || EROI_ty[j] > TROI_ty[i] && EROI_by[j] < TROI_by[i])
						TROI[i]->SetPosXY(X_LEFT, EROI_rx[j]);
					else if (EROI_ty[j] < TROI_ty[i] && EROI_by[j] > TROI_ty[i])
					{
						TROI[i]->SetPosXY(X_LEFT, EROI_rx[j]);
						TROI[i]->SetPosXY(Y_BOTTOM, EROI_by[j]);
					}
					else if (EROI_ty[j] < TROI_by[i] && EROI_by[j] > TROI_by[i])
					{
						TROI[i]->SetPosXY(X_LEFT, EROI_rx[j]);
						TROI[i]->SetPosXY(Y_TOP, EROI_ty[j]);
					}
				}

				if (EROI_lx[j] < TROI_lx[i] && EROI_rx[j] > TROI_rx[i] && (EROI_ty[j] < TROI_by[i] && EROI_by[j] > TROI_ty[i]) || 
					EROI_lx[j] > TROI_lx[i] && EROI_rx[j] < TROI_rx[i] && (EROI_ty[j] > TROI_by[i] && EROI_by[j] < TROI_ty[i]))
				{
					exclusived[i].lx = TROI_lx[i];
					exclusived[i].rx = TROI_rx[i];
					exclusived[i].ty = TROI_ty[i];
					exclusived[i].by = TROI_by[i];

					TROI[i]->SetPosXY(X_LEFT, 0);
					TROI[i]->SetPosXY(X_RIGHT, 0);
					TROI[i]->SetPosXY(Y_TOP, 0);
					TROI[i]->SetPosXY(Y_BOTTOM, 0);
				}
			}
		}

		for (int i = 0; i < m_baseROIRXTmp_idx; i++)
		{
			TROI_lx[i] = TROI[i]->GetPosXY(X_LEFT);
			TROI_rx[i] = TROI[i]->GetPosXY(X_RIGHT);
			TROI_ty[i] = TROI[i]->GetPosXY(Y_TOP);
			TROI_by[i] = TROI[i]->GetPosXY(Y_BOTTOM);
		}

		for (int i = m_baseROIRXTmp_idx; i < MAX_ROI_CNT; i++)
		{
			TROI[i]->SetPosXY(X_LEFT, 0);
			TROI[i]->SetPosXY(X_RIGHT, 0);
		}

		// 최대, 최소, 평균 온도값 찾기
		for (int k = 0; k < m_SelXPxlChrt_idx; k++)
		{
			for (int j = TROI_ty[k]; j <= TROI_by[k]; j++)
			{
				for (int i = TROI_lx[k]; i <= TROI_rx[k]; i++)
				{
					float val = pBuffer[sizeX*j + i];

					pixelCount[k]++;

					sumTemp[k] += val;

					if (isArrFirst[k]) {
						m_ResultData.TMin[k] = m_ResultData.TMax[k] = val;
						isArrFirst[k] = false;
					}
					else {
						if (val < m_ResultData.TMin[k])
						{
							m_ResultData.TMin[k] = val;
							m_ResultData.TMinX[k] = i;
							m_ResultData.TMinY[k] = j;
						}
						else if (val > m_ResultData.TMax[k])
						{
							m_ResultData.TMax[k] = val;
							m_ResultData.TMaxX[k] = i;
							m_ResultData.TMaxY[k] = j;
						}
					}
				}
			}
			m_ResultData.TAvg[k] = sumTemp[k] / pixelCount[k];
		}
	}

	for (int i = 0; i < m_ROICount; i++)
	{
		if (m_ResultData.TMin[i] <= 0 || (TROI_lx[i] == 0 && TROI_ty[i] == 0 && TROI_rx[i] == 0 && TROI_by[i] == 0))
			m_ResultData.TMin[i] = 0.0f;
		else if (m_ResultData.TMin[i] < 600)
			m_ResultData.TMin[i] = 600.0f;
		if (m_ResultData.TMax[i] <= 0 || (TROI_lx[i] == 0 && TROI_ty[i] == 0 && TROI_rx[i] == 0 && TROI_by[i] == 0))
			m_ResultData.TMax[i] = 0.0f;
		if (m_ResultData.TAvg[i] <= 0 || (TROI_lx[i] == 0 && TROI_ty[i] == 0 && TROI_rx[i] == 0 && TROI_by[i] == 0))
			m_ResultData.TAvg[i] = 0.0f;
	}

	// ResultView Spread Condition flag
	if (m_bSpreadCondition && m_spreadlimitTempr < m_spreadTempr)
		m_bSpreadCondition = false;

	// ROI Buffer
	m_ROI_loop_count = m_SelXPxlChrt_idx;

	if (m_ROIBufferCnt == MAX_ROI_CNT)
	{
		m_ROIBufferCnt = m_ROIBufferLimit - 1;		

		for (int i = 0; i < m_ROIBufferCnt; i++)
		{
			m_ROIBuffer[i] = m_ROIBuffer[i + MAX_ROI_CNT - m_ROIBufferCnt];	
		}

		for (int i = m_ROIBufferCnt; i < MAX_ROI_CNT - 1; i++)	
			m_ROIBuffer[i] = 0;
	}

	m_ROIBuffer[m_ROIBufferCnt++] = m_SelXPxlChrt_idx;
}

void CPyroSoftMDoc::InitROI(int& dst_lx, int& dst_rx, int& dst_ty, int& dst_by, int src_lx, int src_rx, int src_ty, int src_by)
{
	dst_lx = src_lx;	dst_rx = src_rx;
	dst_ty = src_ty;	dst_by = src_by;
}

void CPyroSoftMDoc::OnNewDataReady()
{
	if (!m_nDDAQDeviceNo || !m_bAcqIsRunning)
		return;

	// update the new data from device
	if (!theApp.DDAQ_DEVICE_DO_UpdateData(m_nDDAQDeviceNo))
	{
		//OnDeviceLoggingStop();
		OnDeviceDoStop();
		return;
	}

	// Update Main View //////////////////////////
	UpdateAllViews(NULL, 1);

	if (pFrameBuffer == NULL)
	{
		AfxMessageBox(IDS_ERROR_ALLOCMEM);
		return;
	}

	// get data
	if (!theApp.DDAQ_IRDX_PIXEL_GetData(m_hIRDX_Doc, pFrameBuffer, BufSize))
	{
		delete[] pFrameBuffer;
		return;
	}

	m_DateTime = CTime::GetCurrentTime();
	m_StrDateTime = m_DateTime.Format(_T("%Y-%m-%d %H:%M:%S"));

	// Extract Spot Area ****************************************************

	g_cs.Lock();

	//if(m_RHK_logging)		// 시작 버튼 누르지 않아도 ROI 계산이 되게끔
	CalculateResult(PPointArr.GetCount(), sizeX, sizeY, pFrameBuffer);

	g_cs.Unlock();

	// Update GlassView ///////////////
	UpdateAllViews(NULL, 3);

	theApp.m_pPropertyWnd->UpdateDataSet();
	//****************************************************************************************

	// Zone Information 정보 적용 여부 (Zone 별 속도, 거리 등)
	if (theApp.m_bLoggingRunning && m_ZoneInfoEnable)
		CalRHKTotalDist();

	// 화면 속도에 따라 저장 (시간 트리거에 따라 저장하려면 이전 프로젝트의 m_writeFlag 참조) 
	if ((m_hIRDX_WRITE != INVALID_HANDLE_VALUE) && m_bRawDataSave)
	{
		if (theApp.m_bLoggingRunning)
		{
			if (theApp.m_StatusInfFlag && theApp.m_FocusFlag)
				RHKStatusInf();

			//if (m_RHK_logging && theApp.m_TimerFlag)
			if(m_RHKStream != NULL && theApp.m_TimerFlag)
			{
				theApp.m_TimerFlag = false;

				if (!theApp.DDAQ_IRDX_FILE_AddIRDX(m_hIRDX_WRITE, m_hIRDX_DEVICE) || !AddRHKData(newRHKDataFileName, true))
				{
					OnDeviceLoggingStop();
					return;
				}
			}
			else if(theApp.m_TimerFlag)
			{
				theApp.m_TimerFlag = false;

				if (!theApp.DDAQ_IRDX_FILE_AddIRDX(m_hIRDX_WRITE, m_hIRDX_DEVICE))
				//if (!theApp.DDAQ_IRDX_FILE_AddIRDX(m_hIRDX_WRITE, m_hIRDX_DEVICE) || !AddRHKData(newRHKDataFileName, true))
				{
					OnDeviceLoggingStop();
					return;
				}
			}
		}
	}

	if (m_ROIBufferCnt >= m_ROIBufferLimit)	
	{
		int cnt = 0;
		int sumation = 0;

		for (int i = m_ROIBufferCnt - m_ROIBufferLimit; i < m_ROIBufferCnt; i++)	
		{
			if (m_ROIBuffer[i] >= m_ROICatchCnt)
				cnt++;

			sumation += m_ROIBuffer[i];
		}

		if (!theApp.m_bLoggingRunning && cnt >= m_ROIBufferLimit)	
		{
			OnDeviceLoggingStart();
		}
		else if (theApp.m_bLoggingRunning && sumation == 0)
		{
			for (int i = 0; i < MAX_ROI_CNT - 1; i++)
				m_ROIBuffer[i] = 0;

			m_ROIBufferCnt = 0;

			OnDeviceLoggingStop();
		}
	}

	// Enable NextMessage
	theApp.DDAQ_DEVICE_DO_ENABLE_NextMsg(m_nDDAQDeviceNo);
}

void CPyroSoftMDoc::OnNewIRDXDataReady()
{

}

void CPyroSoftMDoc::FSetFrequency()
{
	if (theApp.m_bLoggingRunning)
	{
		CString s;
		unsigned short m_logging_avg = 8;
		float fav = m_FPS / m_logging_avg; /// 적합한 변수 필요

		if (!theApp.DDAQ_DEVICE_DO_Stop(m_nDDAQDeviceNo))
			return;

		/// 화면 Freq 에 따라 변수를 넣어주면 된다.
		s.Format("%d (%.1f Hz)", m_logging_avg, fav);	/// 적합한 변수 필요
		theApp.m_pPropertyWnd->pDataAcq_AcqFreq->SetValue((_variant_t)(s));
		theApp.DDAQ_IRDX_ACQUISITION_SetAveraging(m_hIRDX_DEVICE, m_logging_avg);

		if (!theApp.DDAQ_DEVICE_DO_Start(m_nDDAQDeviceNo))
			return;
	}
}

void CPyroSoftMDoc::RHKStatusInf()
{
	theApp.m_StatusInfFlag = false;
	CMainFrame* pMWnd = (CMainFrame*)AfxGetMainWnd();
	CString statusStr, strMovDistance, strFocusCount;

	CTimeSpan spanTime = m_DateTime - m_RHKStartTime;
	CString strSpenTime = spanTime.Format("%H:%M:%S");

	strMovDistance.Format("%f", m_RHKTotalDistance);
	
	if (m_OpenMode == 3)
	{
		if (m_RHK_logging)
		{
			strFocusCount.Format("%d", m_RHKZoneCount);
			statusStr.Format("동작 시간 : " + strSpenTime + "  이동 거리 : " + strMovDistance + "(mm)   Zone Pos : " + strFocusCount);
		}
	}
	else
		strFocusCount = "--";

	pMWnd->SetStatusText(statusStr);		// 하단 상태바에 좌표값과 온도값 표시
}

void CPyroSoftMDoc::CalRHKTotalDist()
{
	CString timeDistance;

	if (m_RHKDistance >= m_ZoneDistance[m_RHKZoneCount] && m_NumOfZone - 1 > m_RHKZoneCount  )		// Zone 구간의 거리를 넘었을 시
	{
		m_RHKZoneCount++;

		m_RHKStepTime = m_DateTime;
		m_RHKPreDistance += m_RHKDistance;

		if (!m_RHK_log_done && m_RHKZoneCount > m_EndingPos)		// Ending Position 까지 도달 하면 종료
		{
			OnDeviceLoggingStop();
		}
		else if (!m_RHK_log_done && !m_RHK_logging && m_RHKZoneCount >= m_StartingPos )	// Starting Position이 시작하면 저장 시작해라
		{
			m_RHK_logging = true;

			int RHKData = 3;
			newRHKDataFileName = GetNewDataFileName(RHKData);

			if (!CreateRHKDataFile(newRHKDataFileName))
				AfxMessageBox("파일을 만들지 못하였습니다.\n경로를 확인하세요.");
		}
	}
	else if (m_RHKStepTime == 0)	// Zone 구간의 거리를 넘지 않았을 시
	{
		m_RHKStepTime = m_RHKStartTime;
	}

	// 거리 계산과 방사율 & 투과율 계산
	if (m_NumOfZone - 1 > m_RHKZoneCount)
	{
		m_RHKSpenTime = m_DateTime - m_RHKStepTime;
		m_RHKDistance = m_ZoneSpeed[m_RHKZoneCount] / 60.0f * m_RHKSpenTime.GetTotalSeconds();
		m_RHKTotalDistance = m_RHKPreDistance + m_RHKDistance;

		//if (m_Auto_ET_Mode)
		//{
		//	if (m_RHKTotalDistance > m_ET_Distance[m_ET_idx])
		//	{
		//		//m_Emissivity = m_resultEmissivity[m_ET_idx];   
		//		m_Emissivity = m_ET_Emissivity[m_ET_idx];
		//		m_Transmission = m_ET_Transmission[m_ET_idx];

		//		theApp.m_pPropertyWnd->pMeasObj_Emissivity->SetValue((_variant_t)m_Emissivity);
		//		theApp.DDAQ_IRDX_OBJECT_SetEmissivity(m_hIRDX_Doc, m_Emissivity);

		//		theApp.m_pPropertyWnd->pMeasObj_Trans->SetValue((_variant_t)m_Transmission);
		//		theApp.DDAQ_IRDX_OBJECT_SetTransmission(m_hIRDX_Doc, m_Transmission);

		//		m_ET_idx++;
		//		UpdateAllViews(NULL, 2);
		//	}
		//}
	}
}
/////////////////////////////////////////////////////////////////////////////

void CPyroSoftMDoc::LoadConfig()
{
	FILE *stream;
	CString str;
	int side;
	int tmpLCount, tmpRCount;
	float tmpFirstCoeff;

	char dummy[128];
	int L_lx, L_ty, L_rx, L_by,R_lx, R_ty, R_rx, R_by;
	int lx[2][MAX_EROI_CNT], rx[2][MAX_EROI_CNT], ty[2][MAX_EROI_CNT], by[2][MAX_EROI_CNT];

	::GetModuleFileName(::AfxGetInstanceHandle(), m_AppPath, _MAX_PATH);
	PathRemoveFileSpec(m_AppPath);

	CString ConfigFileName = m_AppPath + CString("\\config.ini");

	stream = fopen((CStringA)ConfigFileName, "rt");

	// [Data Acquisition]
	fgets(dummy, 127, stream);
	fscanf(stream, "AcquisitionFrequency=%d\n", &theApp.m_acqFreq);
	fscanf(stream, "LoggingInterval(ms)=%d\n\n", &theApp.m_loggingInterval);

	// [Mesurement]
	fgets(dummy, 127, stream);
	fscanf(stream, "Emissivity=%f\n", &m_Emissivity);
	fscanf(stream, "Transmission=%f\n", &m_Transmission);

	// [Scaling]
	fgets(dummy, 127, stream);
	fscanf(stream, "ColorBar=%d\n", &m_nBar);
	fscanf(stream, "NumberOfColor=%d\n", &m_NoCol);
	fscanf(stream, "Max=%f\n", &m_ScaleMax);
	fscanf(stream, "Min=%f\n", &m_ScaleMin);
	fscanf(stream, "IsoTherm=%f\n", &m_ScaleIsoTherm);

	// [Focus]
	fgets(dummy, 127, stream);
	fscanf(stream, "Location=%d\n", &m_FocusPosition);

	//[Infomation]
	fgets(dummy, 127, stream);
	fscanf(stream, "Threshold=%f\n", &m_Threshold);
	fscanf(stream, "ROIBuffer=%d\n", &m_ROIBufferLimit);
	fscanf(stream, "CatchROINum=%d\n", &m_ROICatchCnt);
	fscanf(stream, "ROICount=%d\n", &m_ROICount);
	fscanf(stream, "ObjectROIMinSize=%d\n", &m_BROI_minSize);
	BROI->SetMinSize(m_BROI_minSize);

	// [Regression analysis]
	fgets(dummy, 127, stream);
	fscanf(stream, "ThirdCoeffient=%lf\n", &m_ThirdCeoff);
	fscanf(stream, "SecondCoeffient=%lf\n", &m_SecondCeoff);
	fscanf(stream, "FirstCoefficient=%lf\n", &m_FirstCeoff);
	fscanf(stream, "Constant=%d\n", &m_Intercept);

	// [Step Difference]
	fgets(dummy, 127, stream);
	fscanf(stream, "UpStepDifference=%f\n", &m_upStepCoeff);
	fscanf(stream, "MiddleStepDifference=%f\n", &m_middleStepCoeff);
	fscanf(stream, "DownStepDifference=%f\n", &m_downStepCoeff);

	// [Screen Flag]
	fgets(dummy, 127, stream);
	fscanf(stream, "CursorTempFlag=%d\n", &m_cursorShow);
	fscanf(stream, "ObjectROINumberFlag=%d\n", &m_OOIShowNum);
	fscanf(stream, "ROINumberFlag=%d\n", &m_ROIShow);
	fscanf(stream, "POINumberFlag=%d\n", &m_POIShowNum);
	fscanf(stream, "MaxPointerFlag=%d\n", &m_maxPointerShow);
	fscanf(stream, "MinPointerFlag=%d\n", &m_minPointerShow);

	// [Spread Alarm]
	fgets(dummy, 127, stream);
	fscanf(stream, "SpreadDetecRange=%f\n", &m_spreadDetctRange);
	fscanf(stream, "SpreadLimitTempurature=%f\n", &m_spreadlimitTempr);

	// [Base ROI]
	fgets(dummy, 127, stream);
	fscanf(stream, "WhichSide(0-Left,1-Right)=%d\n", &side);
	fscanf(stream, "(L_LX,L_TY)=(%d,%d)\n", &L_lx, &L_ty);
	fscanf(stream, "(L_RX,L_BY)=(%d,%d)\n", &L_rx, &L_by);
	fscanf(stream, "(R_LX,R_TY)=(%d,%d)\n", &R_lx, &R_ty);
	fscanf(stream, "(R_RX,R_BY)=(%d,%d)\n\n", &R_rx, &R_by);
	
	// [Exclusive ROI]
	fgets(dummy, 127, stream);
	fscanf(stream, "ExclusiveROICount(L,R)=(%d,%d)\n", &tmpLCount, &tmpRCount);
	for (int i = 0; i < MAX_EROI_CNT; i++)
	{
		str.Format("(L_LX_%d,L_TY_%d)", (i + 1), (i + 1));
		fscanf(stream, str + "=(%d,%d)\n", &lx[LEFT_SIDE][i], &ty[LEFT_SIDE][i]);
		str.Format("(L_RX_%d,L_BY_%d)", (i + 1), (i + 1));
		fscanf(stream, str + "=(%d,%d)\n", &rx[LEFT_SIDE][i], &by[LEFT_SIDE][i]);
	}
	for (int i = 0; i < MAX_EROI_CNT; i++)
	{
		str.Format("(R_LX_%d,R_TY_%d)", (i + 1), (i + 1));
		fscanf(stream, str + "=(%d,%d)\n", &lx[RIGHT_SIDE][i], &ty[RIGHT_SIDE][i]);
		str.Format("(R_RX_%d,R_BY_%d)", (i + 1), (i + 1)); 
		fscanf(stream, str + "=(%d,%d)\n", &rx[RIGHT_SIDE][i], &by[RIGHT_SIDE][i]);
	}
	fprintf(stream, "\n");

	// [Logging path]
	fgets(dummy, 127, stream);
	fgets(dummy, 127, stream);
	m_strRawDataFilePath = dummy;
	m_strRawDataFilePath.TrimLeft("RawDataFilePath=");
	m_strRawDataFilePath.TrimRight("\n");
	fgets(dummy, 127, stream);
	m_strResultDataFilePath = dummy;
	m_strResultDataFilePath.TrimLeft("ResultDataFilePath=");
	m_strResultDataFilePath.TrimRight("\n");

	/////////////////////////////////////////////////////////////////////////////

	fclose(stream);

	BROI->SetDirection(side);

	bool ret = theApp.DDAQ_SetTempPrecision(0);

	if (m_OpenMode == 3)
	{
		theApp.DDAQ_IRDX_OBJECT_SetEmissivity(m_hIRDX_Doc, m_Emissivity);
		theApp.DDAQ_IRDX_OBJECT_SetTransmission(m_hIRDX_Doc, m_Transmission);
		theApp.DDAQ_IRDX_PALLET_SetBar(m_hIRDX_Doc, m_nBar, m_NoCol);
		theApp.DDAQ_IRDX_SCALE_SetMinMax(m_hIRDX_Doc, m_ScaleMin, m_ScaleMax);
		theApp.DDAQ_IRDX_PALLET_SetIsotherm(m_hIRDX_Doc, 1, m_MinTemp, m_ScaleIsoTherm, RGB(0, 0, 0), 0, false);
	}

	if (BROI->GetDirection() == RIGHT_SIDE)	// RIGHT Side
	{
		// Base ROI Initializing
		BROI->SetPosXY(X_LEFT, R_lx);		BROI->SetPosXY(X_RIGHT, R_rx);
		BROI->SetPosXY(Y_TOP, R_ty);		BROI->SetPosXY(Y_BOTTOM, R_by);

		BROI->SetOppPosXY(X_LEFT, L_lx);	BROI->SetOppPosXY(X_RIGHT, L_rx);
		BROI->SetOppPosXY(Y_TOP, L_ty);		BROI->SetOppPosXY(Y_BOTTOM, L_by);

		// Exclusive ROI Initializing
		EROI[0]->SetCount(tmpRCount);
		EROI[0]->SetOppCount(tmpLCount);


		for (int i = 0; i < MAX_EROI_CNT; i++)
		{
			EROI[i]->SetPosXY(X_LEFT, lx[RIGHT_SIDE][i]);		EROI[i]->SetPosXY(X_RIGHT, rx[RIGHT_SIDE][i]);
			EROI[i]->SetPosXY(Y_TOP, ty[RIGHT_SIDE][i]);		EROI[i]->SetPosXY(Y_BOTTOM, by[RIGHT_SIDE][i]);

			EROI[i]->SetOppPosXY(X_LEFT, lx[LEFT_SIDE][i]);		EROI[i]->SetOppPosXY(X_RIGHT, rx[LEFT_SIDE][i]);
			EROI[i]->SetOppPosXY(Y_TOP, ty[LEFT_SIDE][i]);		EROI[i]->SetOppPosXY(Y_BOTTOM, by[LEFT_SIDE][i]);
		}
	}
	else				// LEFT Side
	{
		// Base ROI Initializing
		BROI->SetPosXY(X_LEFT, L_lx);		BROI->SetPosXY(X_RIGHT, L_rx);
		BROI->SetPosXY(Y_TOP, L_ty);		BROI->SetPosXY(Y_BOTTOM, L_by);

		BROI->SetOppPosXY(X_LEFT, R_lx);		BROI->SetOppPosXY(X_RIGHT, R_rx);
		BROI->SetOppPosXY(Y_TOP, R_ty);		BROI->SetOppPosXY(Y_BOTTOM, R_by);

		// Exclusive ROI Initializing
		EROI[0]->SetCount(tmpLCount);
		EROI[0]->SetOppCount(tmpRCount);

		for (int i = 0; i < MAX_EROI_CNT; i++)
		{
			EROI[i]->SetPosXY(X_LEFT, lx[LEFT_SIDE][i]);		EROI[i]->SetPosXY(X_RIGHT, rx[LEFT_SIDE][i]);
			EROI[i]->SetPosXY(Y_TOP, ty[LEFT_SIDE][i]);		EROI[i]->SetPosXY(Y_BOTTOM, by[LEFT_SIDE][i]);

			EROI[i]->SetOppPosXY(X_LEFT, lx[RIGHT_SIDE][i]);		EROI[i]->SetOppPosXY(X_RIGHT, rx[RIGHT_SIDE][i]);
			EROI[i]->SetOppPosXY(Y_TOP, ty[RIGHT_SIDE][i]);		EROI[i]->SetOppPosXY(Y_BOTTOM, by[RIGHT_SIDE][i]);
		}
	}

	if (m_downStepCoeff > 0)
		m_downStepCoeff = -m_downStepCoeff;
}

void CPyroSoftMDoc::LoadETPerDistance()
{
	::GetModuleFileName(::AfxGetInstanceHandle(), m_AppPath, _MAX_PATH);
	PathRemoveFileSpec(m_AppPath);
	CString FileName = m_AppPath + CString("\\SetETPerDistance.csv");

	int str_count = 0;
	double	db_temp = 0.0f;
	BOOL isNotEnd = true;
	CString strLine, str_tok;
	CString tempLine[3];
	CStdioFile sourceFile;

	if (!sourceFile.Open(FileName, CFile::modeRead, NULL)) {
		AfxMessageBox(_T("Cannot open \"SetETPerDistance\" file."));
		return;
	}
	else {
		int curPos = 0, sepCount = 0;   // dataline counting

		while (1) {
			isNotEnd = sourceFile.ReadString(strLine);
			if (isNotEnd == false) break;

			sepCount = GetFindCharCount(strLine, ',');

			AfxExtractSubString(str_tok, strLine, str_count++, ',');
			db_temp = _ttof(str_tok);
			m_ET_Distance[m_ET_dataCount - 1] = db_temp;
			//m_ET_Distance[m_ET_dataCount] = db_temp;
			AfxExtractSubString(str_tok, strLine, str_count++, ',');
			db_temp = _ttof(str_tok);
			m_ET_Emissivity[m_ET_dataCount - 1] = db_temp;
			//m_ET_Emissivity[m_ET_dataCount] = db_temp;
			AfxExtractSubString(str_tok, strLine, str_count++, ',');
			db_temp = _ttof(str_tok);
			m_ET_Transmission[m_ET_dataCount - 1] = db_temp;
			//m_ET_Transmission[m_ET_dataCount] = db_temp;

			m_ET_dataCount++;
			str_count = 0;
		}
	}

	sourceFile.Close();

	//GetEmissivityBySpline(m_ET_dataCount, m_ET_Distance, m_ET_Emissivity);
}

/// #############################################################################
void CPyroSoftMDoc::GetEmissivityBySpline(int pointCount, double distanceArray[], double emissivityArray[])
{
	double gapArr[MAX_ZONE * 5] = { 0 };
	double a = 0, b = 0, c = 0, d = 0, sum = 0;
	double s[MAX_ZONE * 5] = { 0 }, distArr[MAX_ZONE * 5] = { 0 }, F[MAX_ZONE * 5] = { 0 }, emisArr[MAX_ZONE * 5] = { 0 }, m[MAX_ZONE * 5][MAX_ZONE * 5] = { 0 };
	double temp = 0;

	for (int i = 0; i < pointCount; i++) {
		distArr[i] = distanceArray[i];
		emisArr[i] = emissivityArray[i];
	}

	int tempCount = 100;

	for (int i = tempCount - 1; i > 0; i--) {
		F[i] = (emisArr[i] - emisArr[i - 1]) / (distArr[i] - distArr[i - 1]);
		gapArr[i - 1] = distArr[i] - distArr[i - 1];
	}

	//*********** formation of h, s , f matrix **************//
	for (int i = 1; i < tempCount - 1; i++) {
		m[i][i] = 2 * (gapArr[i - 1] + gapArr[i]);
		if (i != 1) {
			m[i][i - 1] = gapArr[i - 1];
			m[i - 1][i] = gapArr[i - 1];
		}
		m[i][tempCount - 1] = 6 * (F[i + 1] - F[i]);
	}

	//***********  forward elimination **************//
	for (int i = 1; i < tempCount - 2; i++) {
		temp = (m[i + 1][i] / m[i][i]);
		for (int j = 1; j <= tempCount - 1; j++) {
			m[i + 1][j] -= temp*m[i][j];
		}
	}

	//*********** back ward substitution *********//
	for (int i = tempCount - 2; i > 0; i--) {
		sum = 0;
		for (int j = i; j <= tempCount - 2; j++) {
			sum += m[i][j] * s[j];
		}
		s[i] = (m[i][tempCount - 1] - sum) / m[i][i];
	}

	double p = distArr[0];
	//double resultEmissivity[MAX_ZONE * 5] = { 0 };

	for (int i = 0; i < MAX_ZONE * 5 - 1; i++) {
		if (distArr[i] != 0) {
			if (distArr[i] <= p || p <= distArr[i + 1]) {
				a = (s[i + 1] - s[i]) / (6 * gapArr[i]);
				b = s[i] / 2;
				c = (emisArr[i + 1] - emisArr[i]) / gapArr[i] - (2 * gapArr[i] * s[i] + s[i + 1] * gapArr[i]) / 6;
				d = emisArr[i];
				sum = a*pow((p - distArr[i]), 3) + b*pow((p - distArr[i]), 2) + c*(p - distArr[i]) + d;
				if (sum < 0)
					sum = 0;
				else if (sum > 1)
					sum = 1;
				m_resultEmissivity[i] = sum;
			}
		}
		p = p + 10;
	}
}

int CPyroSoftMDoc::GetFindCharCount(CString parm_str, char token_char)
{
	int length = parm_str.GetLength(), find_count = 0;

	for (int i = 0; i < length; i++)
	{
		if (parm_str[i] == token_char) find_count++;
	}
	return find_count;
}

void CPyroSoftMDoc::LoadRecipeConfig()
{
	FILE *stream;

	char dummy[128];

	::GetModuleFileName(::AfxGetInstanceHandle(), m_AppPath, _MAX_PATH);
	PathRemoveFileSpec(m_AppPath);

	CString ConfigFileName = m_AppPath + CString("\\Recipe_config.ini");

	stream = fopen((CStringA)ConfigFileName, "rt");	//	단순 Read로만 가능한지 테스트 해보자

													// [Recipe Setting]
	fgets(dummy, 127, stream);
	fscanf(stream, "NumberOfRecipe=%d\n", &m_NumOfZone);
	fscanf(stream, "RowNum=%d\n", &m_RowNum);
	fscanf(stream, "ColNum=%d\n\n", &m_ColNum);

	// [Recipe Information]
	fgets(dummy, 127, stream);
	fscanf(stream, "StartingPoisition=%d\n", &m_StartingPos);
	fscanf(stream, "EndingPosition=%d\n\n", &m_EndingPos);

	for (int j = 0; j < m_RowNum; j++)
	{
		fgets(dummy, 127, stream);
		fscanf(stream, "거리(mm)\t");
		for (int i = 0; i<m_ColNum; i++)
		{
			fscanf(stream, "%d\t", &m_ZoneDistance[j*m_ColNum + i]);
		}
		(stream, "\n");
		fscanf(stream, "속도(mm/Min)\t");
		for (int i = 0; i<m_ColNum; i++)
		{
			fscanf(stream, "%f\t", &m_ZoneSpeed[j*m_ColNum + i]);
		}
		(stream, "\n");
		fscanf(stream, "온도(℃)\t");
		for (int i = 0; i<m_ColNum; i++)
		{
			fscanf(stream, "%f\t", &m_ZoneTemp[j*m_ColNum + i]);
		}
		(stream, "\n\n");
	}

	/////////////////////////////////////////////////////////////////////////////

	fclose(stream);

	//if (m_RcpStartDist < 0 && m_RcpStartDist > 5000)
	//	m_RcpStartDist = 5001;
}

void CPyroSoftMDoc::SaveIRDXConfig()
{
	//FILE *stream;

	//char* buffer;
	//int strLen;

	//::GetModuleFileName(::AfxGetInstanceHandle(), m_AppPath, _MAX_PATH);
	//PathRemoveFileSpec(m_AppPath);

	//CString ConfigFileName = m_AppPath + CString("\\config.ini");
	////CString ConfigFileName = CString("H:\\PyroSoftM\\PyroSoftM\\Debug") +CString("\\config.ini");

	//stream = fopen((CStringA)ConfigFileName, "r+");

	////[Time]
	//fprintf(stream, "[Data Acquisition]\n");
	//fprintf(stream, "AcquisitionFrequency=%d\n", theApp.m_acqFreq);
	//fprintf(stream, "");

	//fclose(stream);
}


void CPyroSoftMDoc::SaveConfig()
{
	FILE *stream;

	char* buffer;
	int strLen;

	::GetModuleFileName(::AfxGetInstanceHandle(), m_AppPath, _MAX_PATH);
	PathRemoveFileSpec(m_AppPath);

	CString ConfigFileName = m_AppPath + CString("\\config.ini");
	//CString ConfigFileName = CString("H:\\PyroSoftM\\PyroSoftM\\Debug") +CString("\\config.ini");

	stream = fopen((CStringA)ConfigFileName, "wt");

	//[Time]
	fprintf(stream, "[Data Acquisition]\n");
	fprintf(stream, "AcquisitionFrequency=%d\n", theApp.m_acqFreq);
	fprintf(stream, "LoggingInterval(ms)=%d\n\n", theApp.m_loggingInterval);

	// [Mesurement]
	fprintf(stream, "[Mesurement]\n");
	fprintf(stream, "Emissivity=%.2f\n", m_Emissivity);
	fprintf(stream, "Transmission=%.2f\n\n", m_Transmission);

	// [Scaling]
	fprintf(stream, "[Scaling]\n");
	fprintf(stream, "ColorBar=%d\n", m_nBar);
	fprintf(stream, "NumberOfColor=%d\n", m_NoCol);
	fprintf(stream, "Max=%.0f\n", m_ScaleMax);
	fprintf(stream, "Min=%.0f\n", m_ScaleMin);
	fprintf(stream, "IsoTherm=%.0f\n\n", m_ScaleIsoTherm);

	// [Focus]
	fprintf(stream, "[Focus]\n");
	//fprintf(stream, "FocusFlag=%d\n", theApp.m_ForcedFocusFlg);
	fprintf(stream, "Location=%d\n\n", m_FocusPosition);

	// [Information]
	fprintf(stream, "[Infomation]\n");
	fprintf(stream, "Threshold=%.2f\n", m_Threshold);
	fprintf(stream, "ROIBuffer=%d\n", m_ROIBufferLimit);
	fprintf(stream, "CatchROINum=%d\n", m_ROICatchCnt);
	fprintf(stream, "ROICount=%d\n", m_ROICount);
	fprintf(stream, "ObjectROIMinSize=%d\n\n", m_BROI_minSize);

	// [Regression analysis]
	fprintf(stream, "[Regression analysis]\n");
	fprintf(stream, "ThirdCoeffient=%.2f\n", m_ThirdCeoff);
	fprintf(stream, "SecondCoeffient=%.2f\n", m_SecondCeoff);
	fprintf(stream, "FirstCoefficient=%.2f\n", m_FirstCeoff);
	fprintf(stream, "Constant=%d\n\n", m_Intercept);

	// [Step Difference]
	fprintf(stream, "[Step Difference]\n");
	fprintf(stream, "UpStepDifference=%.2f\n", m_upStepCoeff);
	fprintf(stream, "MiddleStepDifference=%.2f\n", m_middleStepCoeff);
	fprintf(stream, "DownStepDifference=%.2f\n\n", abs(m_downStepCoeff));

	// [Screen Flag]
	fprintf(stream, "[Screen Flag]\n");
	fprintf(stream, "CursorTempFlag=%d\n", m_cursorShow);
	fprintf(stream, "ObjectROINumberFlag=%d\n", m_OOIShowNum);
	fprintf(stream, "ROINumberFlag=%d\n", m_ROIShow);
	fprintf(stream, "POINumberFlag=%d\n", m_POIShowNum);
	fprintf(stream, "MaxPointerFlag=%d\n", m_maxPointerShow);
	fprintf(stream, "MinPointerFlag=%d\n\n", m_minPointerShow);

	// [Spread Alarm]
	fprintf(stream, "[Spread Alarm]\n");
	fprintf(stream, "SpreadDetecRange=%.2f\n", m_spreadDetctRange);
	fprintf(stream, "SpreadLimitTempurature=%.2f\n\n", m_spreadlimitTempr);

	// [Base ROI]
	fprintf(stream, "[Base ROI]\n");
	fprintf(stream, "WhichSide(0-Left,1-Right)=%d\n", BROI->GetDirection());
	if (BROI->GetDirection())		// Right Side
	{
		fprintf(stream, "(L_LX,L_TY)=(%d,%d)\n", BROI->GetOppPosXY(X_LEFT), BROI->GetOppPosXY(Y_TOP));
		fprintf(stream, "(L_RX,L_BY)=(%d,%d)\n", BROI->GetOppPosXY(X_RIGHT), BROI->GetOppPosXY(Y_BOTTOM));
		fprintf(stream, "(R_LX,R_TY)=(%d,%d)\n", BROI->GetPosXY(X_LEFT), BROI->GetPosXY(Y_TOP));
		fprintf(stream, "(R_RX,R_BY)=(%d,%d)\n\n", BROI->GetPosXY(X_RIGHT), BROI->GetPosXY(Y_BOTTOM));
	}
	else					// Left Side
	{
		fprintf(stream, "(L_LX,L_TY)=(%d,%d)\n", BROI->GetPosXY(X_LEFT), BROI->GetPosXY(Y_TOP));
		fprintf(stream, "(L_RX,L_BY)=(%d,%d)\n", BROI->GetPosXY(X_RIGHT), BROI->GetPosXY(Y_BOTTOM));
		fprintf(stream, "(R_LX,R_TY)=(%d,%d)\n", BROI->GetOppPosXY(X_LEFT), BROI->GetOppPosXY(Y_TOP));
		fprintf(stream, "(R_RX,R_BY)=(%d,%d)\n\n", BROI->GetOppPosXY(X_RIGHT), BROI->GetOppPosXY(Y_BOTTOM));
	}
	
	// [Exclusive ROI]
	fprintf(stream, "[Exclusive ROI]\n");

	if (BROI->GetDirection())		// Right Side
	{
		fprintf(stream, "ExclusiveROICount(L,R)=(%d,%d)\n", EROI[0]->GetOppCount() , EROI[0]->GetCount());

		for (int i = 0; i < MAX_EROI_CNT; i++)
		{
			fprintf(stream, "(L_LX_%d,L_TY_%d)=(%d,%d)\n", (i + 1), (i + 1), EROI[i]->GetOppPosXY(X_LEFT), EROI[i]->GetOppPosXY(Y_TOP));
			fprintf(stream, "(L_RX_%d,L_BY_%d)=(%d,%d)\n", (i + 1), (i + 1), EROI[i]->GetOppPosXY(X_RIGHT), EROI[i]->GetOppPosXY(Y_BOTTOM));
		}
		for (int i = 0; i < MAX_EROI_CNT; i++)
		{
			fprintf(stream, "(R_LX_%d,R_TY_%d)=(%d,%d)\n", (i + 1), (i + 1), EROI[i]->GetPosXY(X_LEFT), EROI[i]->GetPosXY(Y_TOP));
			fprintf(stream, "(R_RX_%d,R_BY_%d)=(%d,%d)\n", (i + 1), (i + 1), EROI[i]->GetPosXY(X_RIGHT), EROI[i]->GetPosXY(Y_BOTTOM));
		}
	}
	else					// Left Side
	{
		fprintf(stream, "ExclusiveROICount(L,R)=(%d,%d)\n", EROI[0]->GetCount(), EROI[0]->GetOppCount());

		for (int i = 0; i < MAX_EROI_CNT; i++)
		{
			fprintf(stream, "(L_LX_%d,L_TY_%d)=(%d,%d)\n", (i + 1), (i + 1), EROI[i]->GetPosXY(X_LEFT), EROI[i]->GetPosXY(Y_TOP));
			fprintf(stream, "(L_RX_%d,L_BY_%d)=(%d,%d)\n", (i + 1), (i + 1), EROI[i]->GetPosXY(X_RIGHT), EROI[i]->GetPosXY(Y_BOTTOM));
		}
		for (int i = 0; i < MAX_EROI_CNT; i++)
		{
			fprintf(stream, "(R_LX_%d,R_TY_%d)=(%d,%d)\n", (i + 1), (i + 1), EROI[i]->GetOppPosXY(X_LEFT), EROI[i]->GetOppPosXY(Y_TOP));
			fprintf(stream, "(R_RX_%d,R_BY_%d)=(%d,%d)\n", (i + 1), (i + 1), EROI[i]->GetOppPosXY(X_RIGHT), EROI[i]->GetOppPosXY(Y_BOTTOM));
		}
	}
	fprintf(stream, "\n");
	
	// [DataFilePath]
	fprintf(stream, "[DataFilePath]\n");
	fprintf(stream, "RawDataFilePath=%s\n", m_strRawDataFilePath);
	fprintf(stream, "ResultDataFilePath=%s\n\n", m_strResultDataFilePath);

	fclose(stream);
}

bool CPyroSoftMDoc::CreateResultDataFile(CString FileName)
{
	FILE *stream;
	CFileFind pFind;

	CString ConfigFileName = FileName;

	stream = fopen((CStringA)ConfigFileName, "w");

	BOOL bRet = pFind.FindFile((CStringA)ConfigFileName);

	fclose(stream);

	return bRet;
}

/*bool CPyroSoftMDoc::CreateRHKDataFile(CString FileName)
{
	//FILE *stream;
	CFileFind pFind;

	CString ConfigFileName = FileName;

	//stream
	m_RHKStream = fopen((CStringA)ConfigFileName, "w");

	BOOL bRet = pFind.FindFile((CStringA)ConfigFileName);

	CTime DateTime = CTime::GetCurrentTime();
	CString	strDate = DateTime.Format(_T("%Y-%m-%d %H:%M:%S"));

	fprintf(m_RHKStream, "%s\t\t%s\n", "Name", theApp.m_RHK_name);

	fprintf(m_RHKStream, "%s\t\t%s\n", "StartTime", strDate);

	SS_ET_OFFSET = ftell(m_RHKStream);
	fprintf(m_RHKStream, "%s\t\t%s\n", "EndTime  ", strDate);		///수정 요함

	fprintf(m_RHKStream, "%s\t\t%d\n", "StartingPos", m_StartingPos);

	fprintf(m_RHKStream, "%s\t\t%d\n", "EndingPos", m_EndingPos);

	fprintf(m_RHKStream, "%s\t\t%d\n", "ROI_Num  ", m_ROICount);

	fprintf(m_RHKStream, "%s\t\t%.2f\n", "MIN_TEMP(℃)", m_ScaleMin);

	fprintf(m_RHKStream, "%s\t\t%.2f\n", "MAX_TEMP(℃)", m_ScaleMax);

	fprintf(m_RHKStream, "%s\t\t%.2f\n", "Treshold(℃)", m_Threshold);

	fprintf(m_RHKStream, "\n\n");

	CString RowData[] = { "Index", "Date_Time", "Distance(mm)", "Emissivity",	"Transmission" };
	CString ROI_str = "ROI_";

	for (int i = 0; i<(sizeof(RowData) / sizeof(RowData[0])); i++)
	{
		fprintf(m_RHKStream, "%s\t", RowData[i]);
	}
	for (int i = 0; i < m_ROICount; i++)
	{
		fprintf(m_RHKStream, "%s%d\t\t\t", ROI_str, i + 1);
	}
	fprintf(m_RHKStream, "\n\t\t\t\t\t");
	for (int i = 0; i < m_ROICount; i++)
	{
		fprintf(m_RHKStream, "MIN\tMAX\tAVG\t");
	}

	fprintf(m_RHKStream, "\n\n");

	return bRet;
}*/

bool CPyroSoftMDoc::CreateRHKDataFile(CString FileName)
{
	//FILE *stream;
	CFileFind pFind;

	CString ConfigFileName = FileName;

	//stream
	m_RHKStream = fopen((CStringA)ConfigFileName, "w");

	BOOL bRet = pFind.FindFile((CStringA)ConfigFileName);

	CTime DateTime = CTime::GetCurrentTime();
	CString	strDate = DateTime.Format(_T("%Y-%m-%d %H:%M:%S"));

	fprintf(m_RHKStream, "%s\t\t%s\n", "Name", theApp.m_RHK_name);

	fprintf(m_RHKStream, "%s\t\t%s\n", "StartTime", strDate);

	SS_ET_OFFSET = ftell(m_RHKStream);
	fprintf(m_RHKStream, "%s\t\t%s\n", "EndTime  ", strDate);		///수정 요함

	fprintf(m_RHKStream, "%s\t\t%d\n", "StartingPos", m_StartingPos);

	fprintf(m_RHKStream, "%s\t\t%d\n", "EndingPos", m_EndingPos);

	fprintf(m_RHKStream, "%s\t\t%d\n", "ROI_Num  ", m_ROICount);

	fprintf(m_RHKStream, "%s\t\t%.2f\n", "MIN_TEMP(℃)", m_ScaleMin);

	fprintf(m_RHKStream, "%s\t\t%.2f\n", "MAX_TEMP(℃)", m_ScaleMax);

	fprintf(m_RHKStream, "%s\t\t%.2f\n", "Treshold(℃)", m_Threshold);

	fprintf(m_RHKStream, "\n\n");

	CString RowData[] = { "Index", "Date_Time"/*, "Distance(mm)", "Emissivity",	"Transmission" */ };
	CString ROI_str = "ROI_";

	for (int i = 0; i<(sizeof(RowData) / sizeof(RowData[0])); i++)
	{
		fprintf(m_RHKStream, "%s\t", RowData[i]);
	}
	for (int i = 0; i < m_ROICount; i++)
	{
		fprintf(m_RHKStream, "%s%d\t", ROI_str, i + 1);
	}
	fprintf(m_RHKStream, "Spread\t");

	fprintf(m_RHKStream, "\n\t\t\t\t\t");
	//for (int i = 0; i < m_ROICount; i++)
	//{
	//	//fprintf(m_RHKStream, "MIN\tMAX\tAVG\t");
	//	fprintf(m_RHKStream, "MAX\t");
	//}

	//fprintf(m_RHKStream, "\n\n");
	fprintf(m_RHKStream, "\n");

	return bRet;
}

bool CPyroSoftMDoc::CreateRawDataFile(CString FileName)
{
	FILE *stream;
	CFileFind pFind;

	CString ConfigFileName = FileName;

	stream = fopen((CStringA)ConfigFileName, "w");

	BOOL bRet = pFind.FindFile((CStringA)ConfigFileName);

	fclose(stream);

	return bRet;
}

void CPyroSoftMDoc::CloseRHKData()
{
	if (m_OpenMode == 3)		// Online Mode
	{
		if (m_bRawDataSave && m_RHK_logging == true)
		{
			CTime DateTime = CTime::GetCurrentTime();
			CString	strDate = DateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
			CString renameRHKDataFileName = newRHKDataFileName;

			fseek(m_RHKStream, SS_ET_OFFSET, SEEK_SET);
			fprintf(m_RHKStream, "%s\t\t%s\n", "EndTime  ", strDate);
			fclose(m_RHKStream);

			m_RHKStream = NULL;
		}
	}
	else if (m_OpenMode == 1)	// IRDX Mode
	{
		if (m_bRawDataSave && theApp.m_bIRDXLoggingRunning == true)
		{
			CTime DateTime = CTime::GetCurrentTime();
			CString	strDate = DateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
			CString renameRHKDataFileName = newRHKDataFileName;

			fseek(m_RHKStream, SS_ET_OFFSET, SEEK_SET);
			fprintf(m_RHKStream, "%s\t\t%s\n", "EndTime  ", strDate);
			fclose(m_RHKStream);

			m_RHKStream = NULL;
		}
	}
}

bool CPyroSoftMDoc::AddRHKData(CString FileName, bool flag)
{
	CFileFind pFind;

	CString ConfigFileName = FileName;
	CString	m_IRDXDateTime = theApp.m_systemDate + " " + theApp.m_systemTime;

	BOOL bRet = pFind.FindFile((CStringA)ConfigFileName);

	if(m_OpenMode == 1)		// IRDX mode
		WriteRHKData(m_RHKStream, flag, m_IRDXDateTime);
	else					// Online or Simualtion mode
		WriteRHKData(m_RHKStream, flag, m_StrDateTime);

	return bRet;
}

/*void CPyroSoftMDoc::WriteRHKData(FILE *stream, bool flag, CString DateTime)
{
	unsigned long id = 0;
	unsigned long type = 0;

	theApp.DDAQ_IRDX_DEVICE_GetID(m_hIRDX_Doc, &id, &type);

	bool ok = false;
	float temp = 0.0f;

	theApp.DDAQ_IRDX_DEVICE_GetCameraTemp(m_hIRDX_Doc, &temp, &ok);

	fprintf(stream, "%d\t", m_AcqIndex++);

	fprintf(stream, "%s\t", DateTime);

	fprintf(stream, "%.2lf\t", m_RHKTotalDistance);

	fprintf(stream, "%.2f\t", m_Emissivity);

	fprintf(stream, "%.2f\t", m_Transmission);

	for (int i = 0; i < m_ROICount; i++)
	{
		fprintf(stream, "%.2f\t%.2f\t%.2f\t", m_ResultData.TMin[i], m_ResultData.TMax[i], m_ResultData.TAvg[i]);
	}

	fprintf(stream, "\n");
}*/
void CPyroSoftMDoc::WriteRHKData(FILE *stream, bool flag, CString DateTime)
{
	unsigned long id = 0;
	unsigned long type = 0;

	theApp.DDAQ_IRDX_DEVICE_GetID(m_hIRDX_Doc, &id, &type);

	bool ok = false;
	float temp = 0.0f;

	theApp.DDAQ_IRDX_DEVICE_GetCameraTemp(m_hIRDX_Doc, &temp, &ok);

	fprintf(stream, "%d\t", m_AcqIndex++);

	CString dummy;
	AfxExtractSubString(dummy, DateTime, 0, '.');
	fprintf(stream, "%s\t", dummy);

	//fprintf(stream, "%.2lf\t", m_RHKTotalDistance);

	//fprintf(stream, "%.2f\t", m_Emissivity);

	//fprintf(stream, "%.2f\t", m_Transmission);

	if (EROI[0]->GetCount() > 0) {
		CheckWriting();
	}
	for (int i = 0; i < m_ROICount; i++) {
		if (bShouldWrite[i] == false || m_ResultData.TMax[i] == 0) {
			fprintf(stream, " \t", m_ResultData.TMax[i]);
		}
		else {
			fprintf(stream, "%.2f\t", m_ResultData.TMax[i]);
		}
	}

	dummy.Format("");
	int iTemp = 0;
	for (int k = 0; k < MAX_ROI_CNT; k++) {
		if (m_ResultData.TMax[k] != 0) iTemp++;
	}
	bool tmpFlag = false;
	for (int k = 0; k < iTemp; k++) {
		if (m_ResultData.TMax[k] < m_spreadDetctRange) {
			tmpFlag = true;
			break;
		}
	}

	if (iTemp == 0) tmpFlag = true;
	else tmpFlag = false;

	if (tmpFlag == true) dummy.Format(" ");
	else dummy.Format("%.1f", m_spreadTempr);

	fprintf(stream, "%s\t", dummy);

	fprintf(stream, "\n");
}

bool CPyroSoftMDoc::SaveRawData(CString FileName, int sizeX, int sizeY, float* pBuffer)
{
	FILE *stream;
	CFileFind pFind;

	CString ConfigFileName = FileName;

	stream = fopen((CStringA)ConfigFileName, "a+");

	BOOL bRet = pFind.FindFile((CStringA)ConfigFileName);

	fprintf(stream, "[%dth Frame]\n", m_bFrameNum++);

	fprintf(stream, "Y\\X\t");

	for (int i = 0; i<sizeX; i++)
	{
		fprintf(stream, "%d\t", i);
	}

	for (int i = 0; i<sizeY; i++)
	{
		fprintf(stream, "%d\t", i);

		for (int j = 0; j<sizeX; j++)
		{
			float val = pBuffer[sizeX*i + j];

			fprintf(stream, "%.2f\t", val);
		}
		fprintf(stream, "\n");
	}

	fprintf(stream, "\n");

	fclose(stream);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::OnCloseDocument()
{
	theApp.m_pMainWnd->SetWindowText("RHK Thermal Imaging System");

	// close open irdx
	if (m_hIRDX_READ != INVALID_HANDLE_VALUE)
	{
		theApp.DDAQ_IRDX_FILE_Close(m_hIRDX_READ);
		CloseRHKData();
	}

	if (m_hIRDX_WRITE != INVALID_HANDLE_VALUE && m_bRawDataOpen) {
		theApp.DDAQ_IRDX_FILE_Close(m_hIRDX_WRITE);
		CloseRHKData();
		m_bRawDataOpen = false;
		m_bFrameNum = 0;
	}

	// Save Config
	if (m_OpenMode == 2 || m_OpenMode == 3)
		SaveConfig();
	else
		SaveIRDXConfig();

	CDocument::OnCloseDocument();

	//*****************************************************************************
	int DocCount = 0;
	POSITION pos = theApp.GetFirstDocTemplatePosition();

	while (pos)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)theApp.GetNextDocTemplate(pos);
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while (pos2)
		{
			CDocument * pDocument;
			if ((pDocument = pTemplate->GetNextDoc(pos2)) != NULL)
				DocCount++;
		}
	}

	if (DocCount == 0) {
		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_pCurrentDoc = NULL;

		theApp.m_pPropertyWnd->InitPropList();
		//theApp.m_pScaleDlg->Invalidate();

		pFrame->m_DataPlaySlider.SetRange(0, 1, TRUE);
		pFrame->m_DataPlaySlider.SetPos(0);
		pFrame->m_DataPlaySlider.EnableWindow(0);
	}
}

BOOL CPyroSoftMDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	if (m_hIRDX_READ != INVALID_HANDLE_VALUE)
	{
		bool modified = FALSE;
		theApp.DDAQ_IRDX_FILE_GetModified(m_hIRDX_READ, &modified);
		SetModifiedFlag(modified);
	}

	return CDocument::CanCloseFrame(pFrame);
}

BOOL CPyroSoftMDoc::SaveModified()
{
	if (m_hIRDX_READ != INVALID_HANDLE_VALUE)
	{
		bool modified = FALSE;
		theApp.DDAQ_IRDX_FILE_GetModified(m_hIRDX_READ, &modified);
		SetModifiedFlag(modified);
	}

	return CDocument::SaveModified();
}

BOOL CPyroSoftMDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (!theApp.DDAQ_IRDX_FILE_Save(m_hIRDX_Doc, (char*)lpszPathName))
		return false;

	SetModifiedFlag(FALSE);

	return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::OnDeviceDoStart()
{
	TMessagingType	msg_type = theApp.GetMessagingType();

	HWND	msg_hwnd = (msg_type == Msg_Window) ? AfxGetMainWnd()->m_hWnd : 0;
	DWORD	thread_id = (msg_type == Msg_Thread) ? theApp.GetMessageThreadID() : 0;
	HANDLE	hevent = (msg_type == Msg_Event) ? m_hEventData : INVALID_HANDLE_VALUE;

	if (!theApp.DDAQ_DEVICE_SET_MsgHWND(m_nDDAQDeviceNo, msg_hwnd))
		return;

	if (!theApp.DDAQ_DEVICE_SET_MsgThreadID(m_nDDAQDeviceNo, thread_id))
		return;

	if (!theApp.DDAQ_DEVICE_SET_MsgHEvent(m_nDDAQDeviceNo, hevent))
		return;

	if (!theApp.DDAQ_DEVICE_DO_Start(m_nDDAQDeviceNo))
		return;

	m_bAcqIsRunning = true;

	TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	//nSendResultTimerID = timeSetEvent(1000, caps.wPeriodMin, SendDataTimer, (DWORD)NULL, TIME_PERIODIC);

	UpdateAllViews(NULL, 1);
}

void CPyroSoftMDoc::OnDeviceLoggingStart()
{
	if (m_nDDAQDeviceNo)	// Online mode or simulation mode
	{
		int IRDX = 0;
		int RHKData = 3;

		theApp.m_bLoggingRunning = true;
		m_bRawDataSave = true;
		m_ChartFlag = true;					// TChart refresh flag

		m_SaveEveryRecord = true;
		theApp.m_StatusInfFlag = true;
		CString newIRDXFileName = GetNewDataFileName(IRDX);

		m_RHKStartTime = CTime::GetCurrentTime();

		// 저장 및 TChart update,주기 1000ms
		TIMECAPS caps;
		timeGetDevCaps(&caps, sizeof(caps));

		////	IRDX File 만들기		////
		if (theApp.DDAQ_IRDX_FILE_OpenWrite(newIRDXFileName, true, &m_hIRDX_WRITE))
			m_bRawDataOpen = true;

		////	RHK File 만들기			////
		newRHKDataFileName = GetNewDataFileName(RHKData);

		if (!CreateRHKDataFile(newRHKDataFileName))
			AfxMessageBox("파일을 만들지 못하였습니다.\n경로를 확인하세요.");
		else 
		{
			m_RHK_logging = true;				// RHK text file logging flag
			AddRHKData(newRHKDataFileName, true);
		}

		////	저장 타이머 이벤트		////
		theApp.m_ClientTimerID = timeSetEvent(1000, caps.wPeriodMin, ClientTimer, (DWORD)NULL, TIME_PERIODIC);
		theApp.m_loggingIntervalTimerID = timeSetEvent(theApp.m_loggingInterval, caps.wPeriodMin, LoggingIntervalTimer, (DWORD)NULL, TIME_PERIODIC);

		// Property 활성화 여부
		PropertyEnableUpdate(FALSE);

		theApp.m_pPropertyWnd->UpdateData();

		UpdateAllViews(NULL, 1);
	}
	else		// IRDX mode
	{
		int RHKData = 3;

		theApp.m_bIRDXLoggingRunning = true;
		m_bRawDataSave = true;

		newRHKDataFileName = GetNewDataFileName(RHKData);

		if (!CreateRHKDataFile(newRHKDataFileName))
			AfxMessageBox("파일을 만들지 못하였습니다.\n경로를 확인하세요.");

		//AddRHKData(newRHKDataFileName, true);
	}
}

void CPyroSoftMDoc::PropertyEnableUpdate(BOOL flag)
{
	theApp.m_pPropertyWnd->pDataAcq_AcqFreq->Enable(flag);

	theApp.m_pPropertyWnd->pRawDataSaveEnable->Enable(flag);
	theApp.m_pPropertyWnd->pRawDataSaveFilePath->Enable(flag);
	theApp.m_pPropertyWnd->pResultDataSaveFilePath->Enable(flag);
	theApp.m_pPropertyWnd->pDataAcq_LoggingInterval->Enable(flag);
	theApp.m_pPropertyWnd->pMeasureDataZoneInfoEnable->Enable(flag);

	//if (m_Auto_ET_Mode)
		//theApp.m_pPropertyWnd->pMeasureDataAutoETMode->Enable(flag);
}

void CPyroSoftMDoc::OnUpdateDeviceDoStart(CCmdUI *pCmdUI)
{
	if (m_nDDAQDeviceNo)
		pCmdUI->Enable(!theApp.m_bLoggingRunning);
	else
		pCmdUI->Enable(!theApp.m_bIRDXLoggingRunning);
}

void CPyroSoftMDoc::OnDeviceDoStop()
{
	theApp.m_pMainWnd->SetWindowText("RHK Thermal Imaging System");

	theApp.DDAQ_DEVICE_DO_Stop(m_nDDAQDeviceNo);
	m_bAcqIsRunning = false;

	if (m_hIRDX_WRITE != INVALID_HANDLE_VALUE && m_bRawDataOpen) {
		theApp.DDAQ_IRDX_FILE_Close(m_hIRDX_WRITE);
		CloseRHKData();
		m_bRawDataOpen = false;
		m_bFrameNum = 0;
	}

	theApp.m_pPropertyWnd->pDataAcq_AcqFreq->Enable(TRUE);

	theApp.m_pPropertyWnd->pRawDataSaveEnable->Enable(TRUE);
	theApp.m_pPropertyWnd->pRawDataSaveFilePath->Enable(TRUE);
	theApp.m_pPropertyWnd->pResultDataSaveFilePath->Enable(TRUE);
	theApp.m_pPropertyWnd->pDataAcq_LoggingInterval->Enable(TRUE);

	UpdateAllViews(NULL, 1);
}

void CPyroSoftMDoc::OnDeviceLoggingStop()
{
	if (m_nDDAQDeviceNo)
	{
		m_AcqIndex = 0;					//속성창의 Data Acquisition ->Index

		if (m_hIRDX_WRITE != INVALID_HANDLE_VALUE && m_bRawDataOpen) {
			theApp.DDAQ_IRDX_FILE_Close(m_hIRDX_WRITE);
			CloseRHKData();
			m_bRawDataOpen = false;
			m_bFrameNum = 0;
		}

		m_bRawDataSave = false;
		theApp.m_bLoggingRunning = false;
		m_RHK_log_done = false;

		m_RHK_logging = false;			// 데이터 로깅 Flag
		m_ET_idx = 0;					// Emissivity, Transmission 인덱스 초기화
		m_row_switch_count = 0;			// 소성로의 대상체가 온도가 switch 될때 flag count !!!!!!!!
		m_low_temp_idx = 0;				// 대상물의 온도가 낮을 때 ROI 갯수

		theApp.m_RHK_name = "";

		if (theApp.m_ClientTimerID != NULL)
			timeKillEvent(theApp.m_ClientTimerID);

		if (theApp.m_loggingIntervalTimerID != NULL)
			timeKillEvent(theApp.m_loggingIntervalTimerID);
		
		// Property 활성화 여부
		PropertyEnableUpdate(TRUE);

		m_RHKEndTime = CTime::GetCurrentTime();

		// ROI값들 초기화		
		InitROI();
		// RHK값 초기화
		InitRHKValues();

		UpdateAllViews(NULL, 1);
	}
	else		// IRDX mode
	{
		int RHKData = 3;

		CloseRHKData();

		theApp.m_bIRDXLoggingRunning = false;
		m_bRawDataSave = false;
	}

	m_bSpreadCondition = true;
}

void CPyroSoftMDoc::InitROI()
{
	for (int i = 0; i < m_ROICount; i++) {
		//TROI_lx[i] = 0;
		//TROI_ty[i] = 0;
		//TROI_rx[i] = 0;
		//TROI_by[i] = 0;
		

		m_ResultData.TMinX[i] = 0;
		m_ResultData.TMinY[i] = 0;
		m_ResultData.TMaxX[i] = 0;
		m_ResultData.TMaxY[i] = 0;

		m_ResultData.TMax[i] = 0;
		m_ResultData.TMin[i] = 0;
		m_ResultData.TAvg[i] = 0;
	}
}

void CPyroSoftMDoc::InitRHKValues()
{
	m_RHKDistance = 0.0f;
	m_RHKPreDistance = 0.0f;
	m_RHKTotalDistance = 0.0f;
	m_RHKZoneCount = -1;
	
	m_RHKSpenTime = m_RHKStartTime - m_RHKStartTime;
}

///////////////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::OnUpdateDeviceDoStop(CCmdUI *pCmdUI)
{
	if(m_nDDAQDeviceNo)
		pCmdUI->Enable(theApp.m_bLoggingRunning);
	else
		pCmdUI->Enable(theApp.m_bIRDXLoggingRunning);
}

void CPyroSoftMDoc::OnDataacquisitionShowmin()
{
	UpdateAllViews(NULL, 4);

	m_bShowPointer = !m_bShowPointer;
}

void CPyroSoftMDoc::OnUpdateDataacquisitionShowmin(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nDDAQDeviceNo);
	pCmdUI->SetCheck(m_bShowPointer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString	CPyroSoftMDoc::GetNewDataFileName(int SaveType)
{
	CString newRawDataFolderName;
	CString newResultDataFolderNmae;
	CString strFileName;
	unsigned long id = 0;
	unsigned long type = 0;

	theApp.DDAQ_IRDX_DEVICE_GetID(m_hIRDX_Doc, &id, &type);

	CTime DateTime = CTime::GetCurrentTime();
	CString strMonth = DateTime.Format(_T("%Y-%m"));
	CString strDate = DateTime.Format(_T("%Y-%m-%d"));
	CString	strTime = DateTime.Format(_T("%Y-%m-%d_%H%M%S"));
	strFileName.Format("[%d]", id);

	CFileFind pFind;
	BOOL bRet_one = pFind.FindFile((CStringA)m_strRawDataFilePath);
	BOOL bRet_two = pFind.FindFile((CStringA)m_strResultDataFilePath);

	if(!bRet_one || !bRet_two)
	{
		::GetModuleFileName(::AfxGetInstanceHandle(), m_AppPath, _MAX_PATH);
		PathRemoveFileSpec(m_AppPath);

		//CString ConfigFileName = m_AppPath + CString("\\config.ini");

		switch (SaveType)
		{
		case 0:			//IRDX
			m_strRawDataFilePath = m_AppPath;

			m_strRawDataFilePath = (CString)m_strRawDataFilePath + "\\RawData";
			CreateDirectory((CString)m_strRawDataFilePath, NULL);
			newRawDataFolderName = (CString)m_strRawDataFilePath + "\\" + strMonth;
			CreateDirectory(newRawDataFolderName, NULL);
			newRawDataFolderName += "\\" + strDate;
			CreateDirectory(newRawDataFolderName, NULL);
			
			break;
		case 1:			//Raw Data
			m_strRawDataFilePath = m_AppPath;

			m_strRawDataFilePath = (CString)m_strRawDataFilePath + "\\RawData";
			CreateDirectory((CString)m_strRawDataFilePath, NULL);

			break;
		case 2:			//Result Data
			m_strResultDataFilePath = m_AppPath;

			m_strResultDataFilePath = (CString)m_strResultDataFilePath + "\\ResultData";
			CreateDirectory((CString)m_strResultDataFilePath, NULL);

			break;
		case 3:			//RHK Data
			m_strResultDataFilePath = m_AppPath;

			m_strResultDataFilePath = (CString)m_strResultDataFilePath + "\\ResultData";
			CreateDirectory((CString)m_strResultDataFilePath, NULL);
			newResultDataFolderNmae = (CString)m_strResultDataFilePath + "\\" + strMonth;
			CreateDirectory(newResultDataFolderNmae, NULL);
			newResultDataFolderNmae += "\\" + strDate;
			CreateDirectory(newResultDataFolderNmae, NULL);

			break;
		}
	}
	if (bRet_one)
	{
		newRawDataFolderName = (CString)m_strRawDataFilePath + "\\" + strMonth;
		CreateDirectory(newRawDataFolderName, NULL);
		newRawDataFolderName += "\\" + strDate;
		CreateDirectory(newRawDataFolderName, NULL);
	}
	if (bRet_two)
	{
		newResultDataFolderNmae = (CString)m_strResultDataFilePath + "\\" + strMonth;
		CreateDirectory(newResultDataFolderNmae, NULL);
		newResultDataFolderNmae += "\\" + strDate;
		CreateDirectory(newResultDataFolderNmae, NULL);
	}

	switch (SaveType)
	{
	case 0:			//IRDX
		strFileName = strFileName + strTime + _T(".irdx");
		//strFileName = strFileName + strMonth + _T(".irdx");

		strFileName = newRawDataFolderName + "\\" + strFileName;
		break;
	case 1:			//Raw Data
		strFileName = strFileName + strTime + _T(".txt");

		strFileName = newRawDataFolderName + "\\" + strFileName;
		break;
	case 2:			//Result Data
		strFileName = strFileName + strTime + _T(".txt");

		strFileName = newResultDataFolderNmae + "\\" + "[Result]" + strFileName;
		break;
	case 3:			//RHK Data
		strFileName = strFileName + strTime + _T(".txt");

		strFileName = newResultDataFolderNmae + "\\" + "[RHK]" + strFileName;
		break;
	}

	return strFileName;
}

void CPyroSoftMDoc::OnButtonFocusN() {
	theApp.DDAQ_DEVICE_DO_MotorFocus(m_nDDAQDeviceNo, DDAQ_MOTORFOCUS_CMD_NEAR);
}

void CPyroSoftMDoc::OnUpdateFocusN(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_nDDAQDeviceNo && m_OpenMode == 3);// && m_FocusPosition < END_FOCUS_POSITION);
}

void CPyroSoftMDoc::OnButtonFocusNStep() {
	theApp.DDAQ_DEVICE_DO_MotorFocus(m_nDDAQDeviceNo, DDAQ_MOTORFOCUS_CMD_NEAR_STEP);
}

void CPyroSoftMDoc::OnUpdateFocusNStep(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_nDDAQDeviceNo && m_OpenMode == 3);// && m_FocusPosition < END_FOCUS_POSITION);
}

void CPyroSoftMDoc::OnButtonFocusFStep() {
	theApp.DDAQ_DEVICE_DO_MotorFocus(m_nDDAQDeviceNo, DDAQ_MOTORFOCUS_CMD_FAR_STEP);
}

void CPyroSoftMDoc::OnUpdateFocusFStep(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_nDDAQDeviceNo && m_OpenMode == 3);// && m_FocusPosition > START_FOCUS_POSITION);
}

void CPyroSoftMDoc::OnButtonFocusF() {
	theApp.DDAQ_DEVICE_DO_MotorFocus(m_nDDAQDeviceNo, DDAQ_MOTORFOCUS_CMD_FAR);
}

void CPyroSoftMDoc::OnUpdateFocusF(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_nDDAQDeviceNo && m_OpenMode == 3);// && m_FocusPosition > START_FOCUS_POSITION);
}

void CPyroSoftMDoc::OnDataplayerPreviousrecord() {
	if (m_IdxDS > 0) {
		theApp.DDAQ_IRDX_FILE_SetCurDataSet(m_hIRDX_Doc, m_IdxDS - 1);
		m_IdxDS--;
	
		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_DataPlaySlider.SetPos(m_IdxDS);
	
		m_bPreviousBtnClick = true;
	
		IRDXUpdate();
	
		theApp.m_pPropertyWnd->UpdateDataSet();
	}
}

void CPyroSoftMDoc::OnUpdateDataplayerPreviousrecord(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_IdxDS>0 && !m_bDataPlay);
}

void CPyroSoftMDoc::OnDataplayerNextrecord() {
	if (m_IdxDS < m_NoDS - 1) {
		theApp.DDAQ_IRDX_FILE_SetCurDataSet(m_hIRDX_Doc, m_IdxDS + 1);

		if (m_max_idxDS < m_IdxDS)
		{
			m_max_idxDS = m_IdxDS;
			m_bPreviousBtnClick = false;
		}
		else
			m_bPreviousBtnClick = true;

		m_IdxDS++;

		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_DataPlaySlider.SetPos(m_IdxDS);

		IRDXUpdate();

		if (theApp.m_bIRDXLoggingRunning && !(m_bPreviousBtnClick))
			AddRHKData(newRHKDataFileName, true);

		theApp.m_pPropertyWnd->UpdateDataSet();
	}
}

void CPyroSoftMDoc::OnUpdateDataplayerNextrecord(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_IdxDS < m_NoDS - 1 && !m_bDataPlay);
}

void CPyroSoftMDoc::OnDataplayerPlay() 
{
	if (theApp.m_TimerID == NULL)
	{
		m_bDataPlay = true;

		POSITION pos = GetFirstViewPosition();
		CView* pView;
		pView = GetNextView(pos);
		((CGlassFlowView *)pView)->OnIRDXDataplayerPlay();

		theApp.m_pPropertyWnd->pDataAcq_AcqFreq->Enable(false);
	}

}

void CPyroSoftMDoc::OnUpdateDataplayerPlay(CCmdUI *pCmdUI) {
	pCmdUI->Enable(!m_bDataPlay && m_IdxDS < m_NoDS - 1);
}

void CPyroSoftMDoc::OnDataplayerStop() 
{
	if (theApp.m_TimerID != NULL)
	{
		m_bDataPlay = false;

		if (theApp.m_TimerID != NULL) {
			KillTimer(m_GFV_Hwnd, theApp.m_TimerID);
			theApp.m_TimerID = NULL;
		}
		

		theApp.m_pPropertyWnd->pDataAcq_AcqFreq->Enable(true);
	}
}

void CPyroSoftMDoc::OnUpdateDataplayerStop(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_bDataPlay && m_NoDS>1);
}

void CPyroSoftMDoc::OnSwitchLeftSide()
{
	if (BROI->GetDirection() == LEFT_SIDE)
		return;

	SwitchDirection();

	BROI->SetDirection(LEFT_SIDE);

	UpdateAllViews(NULL, 1);
}

void CPyroSoftMDoc::OnUpdateSwitchLeftSide(CCmdUI *pCmdUI)
{
	if (BROI->GetDirection() == LEFT_SIDE)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CPyroSoftMDoc::OnSwitchRightSide()
{
	if (BROI->GetDirection() == RIGHT_SIDE)
		return;

	SwitchDirection();

	BROI->SetDirection(RIGHT_SIDE);

	UpdateAllViews(NULL, 1);
}

void CPyroSoftMDoc::OnUpdateSwitchRightSide(CCmdUI *pCmdUI)
{
	if (BROI->GetDirection() == RIGHT_SIDE)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CPyroSoftMDoc::SwitchDirection()
{
	bool tmpFirst;
	bool tmpDrawDone[MAX_EROI_CNT];

	int tmpCount;
	int tmpLX, tmpRX, tmpTY, tmpBY;
	int lx[MAX_EROI_CNT], rx[MAX_EROI_CNT], ty[MAX_EROI_CNT], by[MAX_EROI_CNT];

	tmpLX = BROI->GetPosXY(X_LEFT);
	tmpRX = BROI->GetPosXY(X_RIGHT);
	tmpTY = BROI->GetPosXY(Y_TOP);
	tmpBY = BROI->GetPosXY(Y_BOTTOM);

	BROI->SetPosXY(X_LEFT, BROI->GetOppPosXY(X_LEFT));
	BROI->SetPosXY(X_RIGHT, BROI->GetOppPosXY(X_RIGHT));
	BROI->SetPosXY(Y_TOP, BROI->GetOppPosXY(Y_TOP));
	BROI->SetPosXY(Y_BOTTOM, BROI->GetOppPosXY(Y_BOTTOM));

	BROI->SetOppPosXY(X_LEFT, tmpLX);
	BROI->SetOppPosXY(X_RIGHT, tmpRX);
	BROI->SetOppPosXY(Y_TOP, tmpTY);
	BROI->SetOppPosXY(Y_BOTTOM, tmpBY);

	// First flag switch
	tmpFirst = EROI[0]->GetDrawFirst();

	EROI[0]->SetDrawFirst(EROI[0]->GetOppDrawFirst());// = m_TempEXROI[0].isDrawFirst;

	EROI[0]->SetOppDrawFirst(tmpFirst);

	// Count num switch
	tmpCount = EROI[0]->GetCount();

	EROI[0]->SetCount(EROI[0]->GetOppCount());

	EROI[0]->SetOppCount(tmpCount);

	// Each point value switch
	for (int i = 0; i < MAX_EROI_CNT; i++)
	{
		tmpDrawDone[i] = EROI[i]->GetDrawDone();

		EROI[i]->SetDrawDone(EROI[i]->GetOppDrawDone());

		EROI[i]->SetOppDrawDone(tmpDrawDone[i]);

		lx[i] = EROI[i]->GetPosXY(X_LEFT);
		rx[i] = EROI[i]->GetPosXY(X_RIGHT);
		ty[i] = EROI[i]->GetPosXY(Y_TOP);
		by[i] = EROI[i]->GetPosXY(Y_BOTTOM);

		EROI[i]->SetPosXY(X_LEFT	, EROI[i]->GetOppPosXY(X_LEFT));
		EROI[i]->SetPosXY(X_RIGHT	, EROI[i]->GetOppPosXY(X_RIGHT));
		EROI[i]->SetPosXY(Y_TOP		, EROI[i]->GetOppPosXY(Y_TOP));
		EROI[i]->SetPosXY(Y_BOTTOM	, EROI[i]->GetOppPosXY(Y_BOTTOM));
			
		EROI[i]->SetOppPosXY(X_LEFT, lx[i]);
		EROI[i]->SetOppPosXY(X_RIGHT, rx[i]);
		EROI[i]->SetOppPosXY(Y_TOP, ty[i]);
		EROI[i]->SetOppPosXY(Y_BOTTOM, by[i]);
	}
}

//
//////	POI Toolbar		////
//void CPyroSoftMDoc::OnPOIDraw()
//{
//	if(m_POI_count < 10)
//		m_POI_Mode = DRAW_POI;		// POI 그리기
//}
//
//void CPyroSoftMDoc::OnUpdatePOIDraw(CCmdUI *pCmdUI)
//{
//	if (theApp.m_bLoggingRunning)
//	{
//		pCmdUI->Enable(FALSE);
//	}
//	else
//		pCmdUI->SetCheck(m_POI_Mode == DRAW_POI);
//}
//
//void CPyroSoftMDoc::OnPOIDelete()
//{
//	m_POI_Mode = MOVE_MODE;		// POI 지우기
//
//	if (m_POIArr.GetCount() > 0)
//	{
//		m_POI_count--;
//		columnUpdateFlag = true;
//
//		if (m_isPoint_clicked == true) 
//		{
//			m_POIArr.RemoveAt(m_point_idx);
//
//			DeletedIndex.Add(pDoc->RunningIndex[m_point_idx]);
//			RunningIndex.RemoveAt(m_point_idx);
//
//			// DeletedIndex Element Sorting
//			SortCArray(pDoc->DeletedIndex, CompareInt);
//		}
//		else
//		{
//			m_POIArr.RemoveAt(pDoc->m_POIArr.GetCount()-1);
//
//			DeletedIndex.Add(pDoc->RunningIndex[pDoc->RunningIndex.GetCount() - 1]);
//			RunningIndex.RemoveAt(pDoc->RunningIndex.GetCount()-1);
//
//			// DeletedIndex Element Sorting
//			SortCArray(pDoc->DeletedIndex, CompareInt);
//		}
//	}
//	this->Invalidate(FALSE);
//	UpdateAllViews(NULL, 6);
//
//	m_isPoint_clicked = false;
//}
//
//void CPyroSoftMDoc::OnUpdatePOIDelete(CCmdUI *pCmdUI)
//{
//	if (theApp.m_bLoggingRunning)
//	{
//		pCmdUI->Enable(FALSE);
//	}
//	else
//		pCmdUI->SetCheck(m_POI_Mode == SPARE_MODE);
//}
//
//void CPyroSoftMDoc::OnRoiRefMove()
//{
//	m_POI_Mode = MOVE_MODE;		// POI 이동
//}
//
//void CPyroSoftMDoc::OnUpdateRoiRefMove(CCmdUI *pCmdUI)
//{
//	if (theApp.m_bLoggingRunning)
//	{
//		pCmdUI->Enable(FALSE);
//	}
//	else
//		pCmdUI->SetCheck(m_POI_Mode == MOVE_MODE);
//}


void CPyroSoftMDoc::OnButtonZoomin()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	
	int previous = m_ZoomMode;
	if (previous == ZOOM_AUTO) {		// auto 상태에서 줌 땡기기 버튼을 누름
		if (zoom_inAuto < 0.25) {
			m_ZoomRatio = 0.25;
			m_ZoomMode = ZOOM_QUATER;
		}
		else if (0.25 < zoom_inAuto  && zoom_inAuto < 0.5) {
			m_ZoomRatio = 0.5;
			m_ZoomMode = ZOOM_HALF;
		}
		else if (0.5 < zoom_inAuto && zoom_inAuto < 1) {
			m_ZoomRatio = 1;
			m_ZoomMode = ZOOM_ONE;
		}
		else if (1 < zoom_inAuto && zoom_inAuto < 2) {
			m_ZoomRatio = 2;
			m_ZoomMode = ZOOM_TWOTIMES;
		}
		else if (2 < zoom_inAuto && zoom_inAuto < 3) {
			m_ZoomRatio = 3;
			m_ZoomMode = ZOOM_THREETIMES;
		}
		else if (3 < zoom_inAuto && zoom_inAuto < 4) {
			m_ZoomRatio = 4;
			m_ZoomMode = ZOOM_FOURTIMES;
		}
		else if (4 < zoom_inAuto && zoom_inAuto < 5) {
			m_ZoomRatio = 5;
			m_ZoomMode = ZOOM_FIVETIMES;
		}
		else if (5 < zoom_inAuto && zoom_inAuto < 6) {
			m_ZoomRatio = 6;
			m_ZoomMode = ZOOM_SIXTIMES;
		}
		else if (6 < zoom_inAuto && zoom_inAuto < 8) {
			m_ZoomRatio = 8;
			m_ZoomMode = ZOOM_EIGHTTIMES;
		}
		else if (8 < zoom_inAuto && zoom_inAuto < 10) {
			m_ZoomRatio = 10;
			m_ZoomMode = ZOOM_TENTIMES;
		}
		else if (10 < zoom_inAuto && zoom_inAuto < 15) {
			m_ZoomRatio = 15;
			m_ZoomMode = ZOOM_FIFTEENTIMES;
		}
	}
	else {
		if (m_ZoomMode < 11) 	m_ZoomMode++;
		switch (m_ZoomMode) {
		case ZOOM_QUATER:
			m_ZoomRatio = 0.25;
			break;
		case ZOOM_HALF:
			m_ZoomRatio = 0.5;
			break;
		case ZOOM_ONE:
			m_ZoomRatio = 1;
			break;
		case ZOOM_TWOTIMES:
			m_ZoomRatio = 2;
			break;
		case ZOOM_THREETIMES:
			m_ZoomRatio = 3;
			break;
		case ZOOM_FOURTIMES:
			m_ZoomRatio = 4;
			break;
		case ZOOM_FIVETIMES:
			m_ZoomRatio = 5;
			break;
		case ZOOM_SIXTIMES:
			m_ZoomRatio = 6;
			break;
		case ZOOM_EIGHTTIMES:
			m_ZoomRatio = 8;
			break;
		case ZOOM_TENTIMES:
			m_ZoomRatio = 10;
			break;
		case ZOOM_FIFTEENTIMES:
			m_ZoomRatio = 15;
			break;
		default:
			m_ZoomMode = ZOOM_AUTO;
			m_ZoomRatio = 1;
			break;
		}
	}
	theApp.m_pPropertyWnd->OnChangedZoomOnly();
	UpdateAllViews(NULL, 2);
}


void CPyroSoftMDoc::OnButtonZoomout()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int previous = m_ZoomMode;

	if (previous == ZOOM_AUTO) 
	{		// auto 상태에서 줌 땡기기 버튼을 누름
		if (zoom_inAuto < 0.25) 
		{
			m_ZoomRatio = 0.25;
			m_ZoomMode = ZOOM_QUATER;
		}
		else if (0.25 < zoom_inAuto  && zoom_inAuto < 0.5) {
			m_ZoomRatio = 0.5;
			m_ZoomMode = ZOOM_HALF;
		}
		else if (0.5 < zoom_inAuto && zoom_inAuto < 1) {
			m_ZoomRatio = 1;
			m_ZoomMode = ZOOM_ONE;
		}
		else if (1 < zoom_inAuto && zoom_inAuto < 2) {
			m_ZoomRatio = 2;
			m_ZoomMode = ZOOM_TWOTIMES;
		}
		else if (2 < zoom_inAuto && zoom_inAuto < 3) {
			m_ZoomRatio = 3;
			m_ZoomMode = ZOOM_THREETIMES;
		}
		else if (3 < zoom_inAuto && zoom_inAuto < 4) {
			m_ZoomRatio = 4;
			m_ZoomMode = ZOOM_FOURTIMES;
		}
		else if (4 < zoom_inAuto && zoom_inAuto < 5) {
			m_ZoomRatio = 5;
			m_ZoomMode = ZOOM_FIVETIMES;
		}
		else if (5 < zoom_inAuto && zoom_inAuto < 6) {
			m_ZoomRatio = 6;
			m_ZoomMode = ZOOM_SIXTIMES;
		}
		else if (6 < zoom_inAuto && zoom_inAuto < 8) {
			m_ZoomRatio = 8;
			m_ZoomMode = ZOOM_EIGHTTIMES;
		}
		else if (8 < zoom_inAuto && zoom_inAuto < 10) {
			m_ZoomRatio = 10;
			m_ZoomMode = ZOOM_TENTIMES;
		}
		else if (10 < zoom_inAuto && zoom_inAuto < 15) {
			m_ZoomRatio = 15;
			m_ZoomMode = ZOOM_FIFTEENTIMES;
		}
	}
	else {
		if (m_ZoomMode < 11) 	m_ZoomMode--;
		switch (m_ZoomMode) {
		case ZOOM_QUATER:
			m_ZoomRatio = 0.25;
			break;
		case ZOOM_HALF:
			m_ZoomRatio = 0.5;
			break;
		case ZOOM_ONE:
			m_ZoomRatio = 1;
			break;
		case ZOOM_TWOTIMES:
			m_ZoomRatio = 2;
			break;
		case ZOOM_THREETIMES:
			m_ZoomRatio = 3;
			break;
		case ZOOM_FOURTIMES:
			m_ZoomRatio = 4;
			break;
		case ZOOM_FIVETIMES:
			m_ZoomRatio = 5;
			break;
		case ZOOM_SIXTIMES:
			m_ZoomRatio = 6;
			break;
		case ZOOM_EIGHTTIMES:
			m_ZoomRatio = 8;
			break;
		case ZOOM_TENTIMES:
			m_ZoomRatio = 10;
			break;
		case ZOOM_FIFTEENTIMES:
			m_ZoomRatio = 15;
			break;
		default:
			m_ZoomMode = ZOOM_AUTO;
			m_ZoomRatio = 1;
			break;
		}
	}
	theApp.m_pPropertyWnd->OnChangedZoomOnly();
	UpdateAllViews(NULL, 2);
}

void CPyroSoftMDoc::OnUpdateButtonZoomin(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (m_ZoomMode == 11) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CPyroSoftMDoc::OnUpdateButtonZoomout(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (m_ZoomMode == 0)pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CPyroSoftMDoc::CheckWriting() {
	for (int k = 0; k < m_ROICount; k++) {		// find target roi in exclusive roi
		if (m_ResultData.TMax[k] == 0 || 
			EROI[k]->GetPosXY(X_LEFT) < exclusived[k].lx && EROI[k]->GetPosXY(X_RIGHT) > exclusived[k].rx ||
			EROI[k]->GetPosXY(Y_TOP) < exclusived[k].ty && EROI[k]->GetPosXY(Y_BOTTOM) > exclusived[k].by) {
			bShouldWrite[k] = false;
		}
		else {
			bShouldWrite[k] = true;
		}
	}
}

