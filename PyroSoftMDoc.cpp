
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
	ON_COMMAND(ID_FOCUS_N_STEP, &CPyroSoftMDoc::OnButtonFocusNStep)
	ON_COMMAND(ID_FOCUS_F_STEP, &CPyroSoftMDoc::OnButtonFocusFStep)
	ON_COMMAND(ID_FOCUS_F, &CPyroSoftMDoc::OnButtonFocusF)
	ON_UPDATE_COMMAND_UI(ID_FOCUS_N, &CPyroSoftMDoc::OnUpdateFocusN)
	ON_UPDATE_COMMAND_UI(ID_FOCUS_N_STEP, &CPyroSoftMDoc::OnUpdateFocusNStep)
	ON_UPDATE_COMMAND_UI(ID_FOCUS_F_STEP, &CPyroSoftMDoc::OnUpdateFocusFStep)
	ON_UPDATE_COMMAND_UI(ID_FOCUS_F, &CPyroSoftMDoc::OnUpdateFocusF)
END_MESSAGE_MAP()


// CPyroSoftMDoc 생성/소멸

CPyroSoftMDoc::CPyroSoftMDoc()
{
	m_OpenMode = 1;			// 1:IRDX, 2:Simulation, 3:Device

	m_nDDAQDeviceNo = 0;	///< invalid or offline
	m_hIRDX_DEVICE = INVALID_HANDLE_VALUE;

	m_bAcqIsRunning = false;

	m_bStretchDIB = true;

	m_hIRDX_Doc = INVALID_HANDLE_VALUE;
	m_hIRDX_READ = INVALID_HANDLE_VALUE;
	m_hIRDX_WRITE = INVALID_HANDLE_VALUE;

	m_AppendIRDXFile = true;
	m_SaveEveryRecord = false;

	m_hEventData = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_NoDS = m_IdxDS = 0;

	m_CamHFOV = MAXHFOV;
	m_CamVFOV = MAXVFOV;

	m_bDataPlay = false;

	m_bRawDataSave = false;

	m_bMaxPointEnable = false;

	m_MaxTemp = m_MinTemp = 0.0f;
	m_FPS = 0.0f;
	m_Emissivity = m_Transmission = m_AmbTemp = 0.0f;

	m_nBar = 0;
	m_NoCol = 256;
	m_ScaleMin = 0.0f;
	m_ScaleMax = 100.0f;
	m_ScaleIsoTherm = 0.0f;

	m_AcqIndex = 0;
	m_FocusLocation = 0;

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
	m_RefAreaFlag = false;
	m_ChartFlag = false;
	m_RHK_logging = false;
	m_Auto_ET_Mode = false;

	m_bShowPointer = true;

	m_RHK_log_done = false;

	columnUpdateFlag = false;

	m_ROI_check_flag = false;
	indexMoveFlag = false;
	m_NearFromDev = true;
	m_TriggerOnOff = false;

	m_cursorShow = false;
	m_ROIShow = false;
	m_maxPointerShow = false;
	m_minPointerShow = false;
	m_triggerLoggingFirst = false;
	m_POIShowNum = 0;

	m_comp_ROICount = 0;

	pFrameBuffer = NULL;

	m_BaseROI.lx = 0;
	m_BaseROI.ty = 0;
	m_BaseROI.rx = 0;
	m_BaseROI.by = 0;

	m_SelXPxlChrt_idx = 0;
	m_SelYPxlChrt_idx = 0;

	m_row_switch_count = 0;

	m_low_temp_idx = 0;
	m_lx_pxl_avg_idx = 0;

	m_ROI_loop_count = 0;
	m_TriggerDist = 0;

	m_ZoomMode = 1;		//100%
	m_ZoomRatio = 0.0f;

	m_upStepCoeff = 0.0f;
	m_middleStepCoeff = 0.0f;
	m_downStepCoeff = 0.0f;

	m_POIArr.SetSize(0);
	RunningIndex.SetSize(0);
	DeletedIndex.SetSize(0);

	for (int i = 0; i < MAX_POI_NUM; i++)
		POI_TemperatureArray[i] = 0;

	for (int i = 0; i<MAX_ROI_NUM; i++)
	{
		max_x[i] = 0;
		min_x[i] = 0;
		max_y[i] = 0;
		min_y[i] = 0;
		temptt[i] = 0;
	}

	for (int i = 0; i < MAX_ET_PER_DISTANCE; i++)
	{
		m_ET_Distance[i] = 0;
		m_ET_Emissivity[i] = 0;
		m_ET_Transmission[i] = 0;
	}
	for (int i = 0; i < MAX_ROI_NUM * 2; i++)
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
	for (int i = 0; i < MAX_ROI_NUM; i++)
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


	//Recipe
	m_NumOfZone = 0;
	m_RowNum = 0;
	m_ColNum = 0;

	for (int i = 0; i < MAX_ZONE; i++)
	{
		m_ZoneDistance[i] = 0;
		m_ZoneSpeed[i] = 0;
		m_ZoneEmissivity[i] = 0;
	}

	m_ResultData.Min = m_ResultData.Max = m_ResultData.Avg = 600.0f;

	pResultData = m_Result;
}

void CALLBACK ClientTimer(UINT m_nTimerID, UINT uiMsg, DWORD dwUser, DWORD dw1, DWORD d2)
{
	theApp.m_StatusInfFlag = true;
	theApp.m_TchartFlag = true;
	theApp.m_TimerFlag = true;
	//theApp.m_
}

void CALLBACK LaserTimer(UINT m_nTimerID, UINT uiMsg, DWORD dwUser, DWORD dw1, DWORD d2)
{
	theApp.DAQ_LASER_DAQmxReadAnalogScalarF64(theApp.DAQ_TaskHandle, 0, &theApp.m_Laser_Distance, NULL);
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
		((CResultView *)pView)->m_MinTabDlg.pDoc = this;
		((CResultView *)pView)->m_AvgTabDlg.pDoc = this;
		((CResultView *)pView)->InitROIData();
		((CROIGridView *)pPView)->InitStatusData();
	//}
	///////////////////////////////////////////////////

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
/////////////////////////////////////////////////////////////////////////////


double CPyroSoftMDoc::getLineAvgTemp(float *pBuf, unsigned short size)
{
	double sum = 0.0;

	if (size <= 0) return 0;

	for (unsigned short i = 0; i<size; i++) {
		sum += pBuf[i];
	}

	return sum / size;
}

//////////////////////////////////////////////////////////////////////////////

// for irdx file
void CPyroSoftMDoc::CalculateResult(int index, int sizeX, int sizeY, float* pBuffer)
{
	/// pBuffer의 주소값이 아닌 안의 배열에 값이 존재 하지 않는 조건문으로 변경해야한다.
	//if(pBuffer != NULL)
	CalROI(index, sizeX, sizeY, pBuffer);
}

/////  대상체의 온도가 확연하게 뜨거워 졌을 시 //////////////
void CPyroSoftMDoc::CalROI(int index, int sizeX, int sizeY, float* pBuffer)
{
	/// pBuffer의 주소값이 아닌 안의 배열에 값이 존재 하지 않는 조건문으로 변경해야한다.

	int arr_count = 0, max_arr = 0, hit_count = 0;	
	int	m_baseROILXTmp_idx = 0;
	int m_baseROIRXTmp_idx = 0;
	int	m_discrim_idx = 0;
	int inclin_down = 0;
	int inclin_zero = 0;
	int inclin_up = 0;
	int m_col_max = 0;		
	int m_col_min = 0;		

	float sumTemp[MAX_ROI_NUM] = { 0.0f };
	int pixelCount[MAX_ROI_NUM] = { 0 };
	bool isArrFirst[MAX_ROI_NUM];
	bool isColFirst = false;
	bool m_inclin_up_flag = false;
	bool m_inclin_zero_flag = false;
	bool m_inclin_down_flag = false;
	//float WidthPerPixel = (m_CamDistance / tan(PI / 180 * (90 - m_CamHFOV / 2))) / (MAX_X_PIXEL / 2);
	int gapX = 0, gapY = 0;

	for (int i = 0; i < m_ROICount; i++)	// ROI XY 좌표의 min, max 값 초기화
	{
		//max_x[i] = 0, max_y[i] = 0, min_x[i] = 0, min_y[i] = 0;
		m_ResultData.TMinX[i] = 0; m_ResultData.TMinY[i] = 0;
		m_ResultData.TMaxX[i] = 0; m_ResultData.TMaxY[i] = 0;
	}

	for (int i = 0; i < MAX_ROI_NUM; i++)
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
	/////////////////////////// 계산 /////////////////////////////////

	if (m_BaseROI.rx != 0 && m_BaseROI.by != 0)
	{
		for (int i = m_BaseROI.ty; i < m_BaseROI.by; i++)
		{
			for (int j = m_BaseROI.lx; j < m_BaseROI.rx; j++)
			{
				float val = pBuffer[sizeX*i + j];

				//m_row_projection[j - m_BaseROI.lx] += val;
				::CalArr(m_row_projection, val, j, (int)m_BaseROI.lx);

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

		for (int i = BOXCAR; i < sizeof(m_row_projection) / sizeof(m_row_projection[0]) - BOXCAR; i++)	// x축의 projection 된 수 만큼
		{

		}

		CRect m_baseROI;
		m_baseROI.left = m_BaseROI.lx;
		m_baseROI.right = m_BaseROI.rx;
		m_baseROI.top = m_BaseROI.ty;
		m_baseROI.bottom = m_BaseROI.by;

		::CalMeasure(pBuffer, &m_baseROI, &m_baseROIRXTmp_idx, m_upStepCoeff, m_middleStepCoeff, m_downStepCoeff, m_baseROILXTemp, m_baseROIRXTemp);
		/*
		for (int i = 0; i < sizeof(m_row_projection) / sizeof(m_row_projection[0]); i++)	// x축의 projection 된 수 만큼
		{
			if (m_row_projection[i] == 0) break;	// 온도값이 0인 부분은 볼 필요 없다.

			if (m_discrim_idx == 4)
			{
				for (int i = 0; i < sizeof(m_discrim_arr) / sizeof(m_discrim_arr[0]) - 1; i++)	// n개의 판별점 저장
				{
					m_discrim_arr[i] = m_discrim_arr[i + 1];
				}
				m_discrim_arr[m_discrim_idx - 1] = m_row_projection[i];

				for (int j = 1; j < sizeof(m_discrim_arr) / sizeof(m_discrim_arr[0]); j++)	// 기울기 계산 후 기울기 배열에 값 넣는작업
				{
					m_inclin_arr[j - 1] = (m_discrim_arr[j] - m_discrim_arr[j - 1]) / (m_BaseROI.by - m_BaseROI.ty); //기울기 계산

					if (m_baseROILXTmp_idx > 50 || m_baseROIRXTmp_idx > 50)
						continue;

					if (!m_inclin_up_flag && inclin_up < 3 && m_inclin_arr[j - 1] > m_upStepCoeff)		//시작 기울기
					{
						for (int i = 0; i < sizeof(m_inclin_arr) / sizeof(m_inclin_arr[0]); i++)
							inclin_up++;
					}
					else if (inclin_up < 3)
					{
						inclin_up = 0;
					}
					else if (!m_inclin_zero_flag && inclin_up >= 3 && inclin_zero < 3 && m_inclin_arr[j - 1] <= m_middleStepCoeff)		//중간 기울기
					{
						inclin_zero++;
						m_inclin_up_flag = true;

						if (inclin_zero == 1)
							m_baseROILXTemp[m_baseROILXTmp_idx++] = i + m_BaseROI.lx;	// 저장
					}
					else if (!m_inclin_down_flag && m_inclin_up_flag && inclin_zero >= 3 && inclin_down < 3 && m_inclin_arr[j - 1] < m_downStepCoeff)	// 종료 기울기
					{
						inclin_down++;
						m_inclin_zero_flag = true;

						if (inclin_down == 1)
							m_baseROIRXTemp[m_baseROIRXTmp_idx++] = i + m_BaseROI.lx;	// 저장
					}
					else if (!m_inclin_down_flag && m_inclin_zero_flag && inclin_down >= 3)
					{
						m_inclin_down_flag = true;

						inclin_up = 0;
						inclin_zero = 0;
						inclin_down = 0;

						m_inclin_up_flag = false;
						m_inclin_zero_flag = false;
						m_inclin_down_flag = false;
					}
				}
			}
			else if (m_discrim_idx < 4)
			{
				m_discrim_arr[m_discrim_idx++] = m_row_projection[i];
			}
		}*/

		for (int i = 0; i < m_baseROIRXTmp_idx; i++)
		{
			//m_SelXPixelChart[m_SelXPxlChrt_idx++] = m_baseROILXTemp[i];
			//m_SelXPixelChart[m_SelXPxlChrt_idx++] = m_baseROIRXTemp[i];
			if (abs(m_baseROILXTemp[i] - min_x[m_SelXPxlChrt_idx]) < 2 || abs(m_baseROILXTemp[i] - min_x[m_SelXPxlChrt_idx]) > 5)
				min_x[m_SelXPxlChrt_idx] = m_baseROILXTemp[i];
			if (abs(m_baseROIRXTemp[i] - max_x[m_SelXPxlChrt_idx]) < 2 || abs(m_baseROIRXTemp[i] - max_x[m_SelXPxlChrt_idx]) > 5)
				max_x[m_SelXPxlChrt_idx] = m_baseROIRXTemp[i];
			min_y[m_SelXPxlChrt_idx] = m_col_min;
			max_y[m_SelXPxlChrt_idx] = m_col_max;
			m_SelXPxlChrt_idx++;
		}
		for (int i = m_baseROIRXTmp_idx; i < MAX_ROI_NUM; i++)
		{
			min_x[i] = 0;
			max_x[i] = 0;
		}
		//m_SelYPixelChart[m_SelYPxlChrt_idx++] = m_col_min;
		//m_SelYPixelChart[m_SelYPxlChrt_idx++] = m_col_max;


		/// 위의 코드에 삽입되도록 만들기
		for (int k = 0; k < m_SelXPxlChrt_idx; k++)
		{
			for (int j = min_y[k]; j <= max_y[k]; j++)
			{
				for (int i = min_x[k]; i <= max_x[k]; i++)
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
		if (m_ResultData.TMin[i] <= 0 || (min_x[i] == 0 && min_y[i] == 0 && max_x[i] == 0 && max_y[i] == 0))
			m_ResultData.TMin[i] = 0.0f;
		else if (m_ResultData.TMin[i] < 600)
			m_ResultData.TMin[i] = 600.0f;
		if (m_ResultData.TMax[i] <= 0 || (min_x[i] == 0 && min_y[i] == 0 && max_x[i] == 0 && max_y[i] == 0))
			m_ResultData.TMax[i] = 0.0f;
		if (m_ResultData.TAvg[i] <= 0 || (min_x[i] == 0 && min_y[i] == 0 && max_x[i] == 0 && max_y[i] == 0))
			m_ResultData.TAvg[i] = 0.0f;
	}

	m_ROI_loop_count = m_SelXPxlChrt_idx;

}


//void CPyroSoftMDoc::CalROI(int index, int sizeX, int sizeY, float* pBuffer)
//{
//	/// pBuffer의 주소값이 아닌 안의 배열에 값이 존재 하지 않는 조건문으로 변경해야한다.
//
//	//if (pBuffer == NULL || !m_RHK_logging)
//	//	return;
//
//	int arr_count = 0, max_arr = 0, hit_count = 0;
//	int m_baseROIRXTmp_idx = 0;
//
//	int m_col_max = 0;
//	int m_col_min = 0;
//	int m_lRow_down_count = 0;
//	int m_lRow_up_count = 0;
//	int m_lRow_arr_idx = 0;
//	int m_change_pxl_cnt = 0;
//	float m_lRow_temp = 0;
//	float m_lRow_avg = 0;
//
//	float sumTemp[MAX_ROI_NUM] = { 0.0f };
//	int pixelCount[MAX_ROI_NUM] = { 0 };
//	bool isArrFirst[MAX_ROI_NUM];
//	bool isColFirst = false;
//
//	bool m_lRow_down_flag = false;
//	bool m_lRow_up_flag = false;
//	bool isPxlZero = false;
//	int gapX = 0, gapY = 0;
//
//	for (int i = 0; i < m_ROICount; i++)	// ROI XY 좌표의 min, max 값 초기화
//	{
//		//max_x[i] = 0, max_y[i] = 0, min_x[i] = 0, min_y[i] = 0;	// ROI 별 좌표 값
//		m_ResultData.TMinX[i] = 0; m_ResultData.TMinY[i] = 0;
//		m_ResultData.TMaxX[i] = 0; m_ResultData.TMaxY[i] = 0;
//	}
//
//	for (int i = 0; i < 512; i++)
//		m_row_projection[i] = 0;
//
//	for (int i = 0; i < 384; i++)
//		m_col_projection[i] = 0;
//
//	for (int i = 0; i < m_SelXPxlChrt_idx; i++)
//		m_SelXPixelChart[i] = 0;
//	m_SelXPxlChrt_idx = 0;
//
//	for (int i = 0; i < m_SelYPxlChrt_idx; i++)
//		m_SelYPixelChart[i] = 0;
//	m_SelYPxlChrt_idx = 0;
//	m_ROI_loop_count = 0;
//
//	/////////////////////////// Base ROI 내 Threshold 를 기준으로 Y축 min, max 좌표값 계산 /////////////////////////////////
//
//	if (m_BaseROI.rx != 0 && m_BaseROI.by != 0)
//	{
//		for (int i = m_BaseROI.ty; i < m_BaseROI.by; i++)
//		{
//			for (int j = m_BaseROI.lx; j < m_BaseROI.rx; j++)
//			{
//				float val = pBuffer[sizeX*i + j];
//
//				//m_row_projection[j - m_BaseROI.lx] += val;		///수정
//				::CalArr(m_row_projection, val, j, (int)m_BaseROI.lx);
//
//				if (val < m_Threshold)
//				{
//					if (!isColFirst)
//					{
//						isColFirst = true;
//						m_col_max = i;
//						m_col_min = i;
//					}
//					else if (i > m_col_max)
//					{
//						m_col_max = i;
//					}
//					else if (i < m_col_min)
//					{
//						m_col_min = i;
//					}
//				}
//			}
//		}
//
//		////////////////// Base ROI 내 Threshold 를 기준으로 Y축 min, max 좌표값 계산 끝 ///////////////////////////
//
//		////////////ty + 1의 라인 프로젝션과 평균온도를 구하기 위함////////// 
//
//		for (int i = m_BaseROI.lx; i < m_BaseROI.rx; i++)	// 
//		{
//			float val = pBuffer[sizeX*(m_col_min + 1) + i];
//
//			m_lRow_projection[i - m_BaseROI.lx] = val;
//			m_lRow_temp += val;
//		}
//
//		m_lRow_avg = m_lRow_temp / (m_BaseROI.rx - m_BaseROI.lx);
//
//		//////////ty + 1의 라인 프로젝션과 평균온도를 구하기 위함 끝///////////
//
//		///////////// ROI의 온도가 취출구 보다 낮을 경우 lx, rx를 구하기 위함 /////////////////
//		if (m_row_switch_count < 3)
//		{
//			for (int i = m_BaseROI.lx; i < m_BaseROI.rx; i++)	// 라인 프로젝션한 결과 roi의 좌표와 갯수를 구하기 위함
//			{
//				if (m_lRow_projection[i - m_BaseROI.lx] < (m_lRow_avg - m_lRow_avg * 0.015) && !m_lRow_down_flag)
//				{
//					m_lRow_down_count++;		// 온도값 기준 Down
//
//					if (m_lRow_down_count >= 2)
//					{
//						m_lRow_down_flag = true;
//						m_baseROILXTemp[m_lRow_arr_idx] = i;
//					}
//				}
//				else if (!m_lRow_up_flag && m_lRow_down_flag && m_lRow_projection[i - m_BaseROI.lx] > (m_lRow_avg /* up 조건*/))
//				{
//					m_lRow_up_count++;
//
//					if (m_lRow_up_count >= 2)
//					{
//						m_lRow_up_flag = true;
//						m_baseROIRXTemp[m_lRow_arr_idx] = i - 1;
//						m_lRow_arr_idx += 1;
//					}
//				}
//				else if (m_lRow_down_flag && m_lRow_up_flag)
//				{
//					m_lRow_down_count = 0;
//					m_lRow_up_count = 0;
//
//					m_lRow_down_flag = false;
//					m_lRow_up_flag = false;
//				}
//			}
//		}
//
//		///////////// ROI의 온도가 취출구 보다 낮을 경우 lx, rx를 구하기 위함 끝 /////////////////
//		
//		//////////////////////
//
//		if (m_row_switch_count == 0 && abs(m_baseROIRXTemp[0] - max_x[0]) > 5 && abs(m_baseROILXTemp[0] - min_x[0]) > 5)	// 초기값 픽셀 저장 
//		{
//			m_row_switch_count = 1;		
//
//			for (int i = 0; i < m_lRow_arr_idx; i++)
//			{
//				min_x[i] = m_baseROILXTemp[i];			min_y[i] = m_col_min;
//				max_x[i] = m_baseROIRXTemp[i];			max_y[i] = m_col_max;
//			}
//			for (int i = m_lRow_arr_idx; i < 50; i++)
//			{
//				min_x[i] = 0;			min_y[i] = 0;
//				max_x[i] = 0; 			max_y[i] = 0;
//			}
//		}
//		else if (m_row_switch_count == 1)	// 급격하게 변하는 픽셀 Catch
//		{
//			for (int i = 0; i < m_lRow_arr_idx; i++)
//			{
//				if (abs(min_x[i] - m_baseROILXTemp[i]) > 4 || abs(max_x[i] - m_baseROIRXTemp[i]) > 4)
//				{
//					m_row_switch_count = 2;
//					break;
//				}
//			}
//
//			for (int i = 0; i < m_lRow_arr_idx; i++)
//			{
//				if (abs(min_x[i] - m_baseROILXTemp[i]) < 2)
//					min_x[i] = m_baseROILXTemp[i];			
//				min_y[i] = m_col_min;
//				if (abs(max_x[i] - m_baseROIRXTemp[i]) < 2)
//					max_x[i] = m_baseROIRXTemp[i];			
//				max_y[i] = m_col_max;
//			}
//			for (int i = m_lRow_arr_idx; i < 50; i++)
//			{
//				min_x[i] = 0;			min_y[i] = 0;
//				max_x[i] = 0; 			max_y[i] = 0;
//			}
//		}
//		else if (m_row_switch_count == 2)	// 대상물의 온도가 애매할 경우의 픽셀의 범위에 들어오기 전까지 픽셀 고정
//		{
//			for (int i = 0; i < m_lRow_arr_idx; i++)
//			{
//				if (abs(min_x[i] - m_baseROILXTemp[i]) <= 3)
//					m_change_pxl_cnt++;
//				if (abs(max_x[i] - m_baseROIRXTemp[i]) <= 3)
//					m_change_pxl_cnt++;
//			}
//
//			if (m_change_pxl_cnt == m_lRow_arr_idx * 2)
//			{
//				m_row_switch_count = 3;
//				m_low_temp_idx = m_lRow_arr_idx;
//			}
//		}
//		else if (m_row_switch_count == 3)	// 대상물의 온도가 취출구의 온도와 확연한 차이가 있을 때
//		{
//			///////////////////////////////////// BOXCAR ////////////////////////////////////
//			if (BOXCAR > 0)
//			{
//				float temp_arr[MAX_X_PIXEL] = { 0, };
//				float temp_val = 0;
//				float temp_fVal = 0, temp_rVal = 0;
//
//				for (int i = BOXCAR; i < m_BaseROI.rx - m_BaseROI.lx - BOXCAR; i++)	// x축의 projection 된 수 만큼
//				{
//					for (int j = -BOXCAR; j <= BOXCAR; j++)
//						temp_val += m_row_projection[i + j];
//
//					temp_arr[i] = temp_val / (BOXCAR * 2 + 1);
//					temp_val = 0;
//				}
//
//				for (int i = 0; i < BOXCAR; i++)
//				{
//					for (int j = 0; j <= BOXCAR + i; j++)
//					{
//						temp_fVal += m_row_projection[j];
//						temp_rVal += m_row_projection[(m_BaseROI.rx - m_BaseROI.lx) - (j + 1)];
//					}
//					temp_arr[i] = temp_fVal / (BOXCAR + i + 1);
//					temp_arr[(m_BaseROI.rx - m_BaseROI.lx) - (i + 1)] = temp_rVal / (BOXCAR + i + 1);
//					temp_fVal = 0;
//					temp_rVal = 0;
//				}
//
//				for (int i = 0; i < sizeof(m_row_projection) / sizeof(m_row_projection[0]); i++)
//					m_row_projection[i] = temp_arr[i];
//			}
//
//			/////////////////////////// END OF BOXCAR ////////////////////////////////////////////////
//
//			/////////////////////////// PROJECTION ALGORITHM /////////////////////////////////////////
//			CRect m_baseROI;
//			m_baseROI.left = m_BaseROI.lx;
//			m_baseROI.right = m_BaseROI.rx;
//			m_baseROI.top = m_BaseROI.ty;
//			m_baseROI.bottom = m_BaseROI.by;
//
//			::CalMeasure(pBuffer, &m_baseROI, &m_baseROIRXTmp_idx, m_upStepCoeff, m_middleStepCoeff, m_downStepCoeff, m_baseROILXTemp, m_baseROIRXTemp);
//			/*
//			int	m_discrim_idx = 0;
//			int	m_baseROILXTmp_idx = 0;
//			int inclin_down = 0;
//			int inclin_zero = 0;
//			int inclin_up = 0;
//
//			bool m_inclin_up_flag = false;
//			bool m_inclin_zero_flag = false;
//			bool m_inclin_down_flag = false;
//
//			float m_discrim_arr[4];
//			float m_inclin_arr[4];
//
//			for (int i = 0; i < 4; i++)
//			{
//				m_discrim_arr[i] = 0;
//				m_inclin_arr[i] = 0;
//			}
//
//			for (int i = 0; i < sizeof(m_row_projection) / sizeof(m_row_projection[0]); i++)	// x축의 projection 된 수 만큼
//			{
//				if (m_row_projection[i] == 0) break;	// 온도값이 0인 부분은 볼 필요 없다.
//
//				if (m_discrim_idx == MAX_DISCRIMINANT_NUM)
//				{
//					for (int i = 0; i < sizeof(m_discrim_arr) / sizeof(m_discrim_arr[0]) - 1; i++)	// n개의 판별점 저장
//					{
//						m_discrim_arr[i] = m_discrim_arr[i + 1];
//					}
//					m_discrim_arr[m_discrim_idx - 1] = m_row_projection[i];
//
//					for (int j = 1; j < sizeof(m_discrim_arr) / sizeof(m_discrim_arr[0]); j++)	// 기울기 계산 후 기울기 배열에 값 넣는작업
//					{
//						m_inclin_arr[j - 1] = (m_discrim_arr[j] - m_discrim_arr[j - 1]) / (m_BaseROI.by - m_BaseROI.ty); //기울기 계산
//
//						if (m_baseROILXTmp_idx > 50 || m_baseROIRXTmp_idx > 50)
//							continue;
//
//						if (!m_inclin_up_flag && inclin_up < 3 && m_inclin_arr[j - 1] > m_upStepCoeff)		// 시작 기울기
//						{
//							for (int i = 0; i < sizeof(m_inclin_arr) / sizeof(m_inclin_arr[0]); i++)
//								inclin_up++;		// 기울기 기준 Up
//						}
//						else if (inclin_up < 3)
//						{
//							inclin_up = 0;
//						}
//						else if (!m_inclin_zero_flag && inclin_up >= 3 && inclin_zero < 3 && m_inclin_arr[j - 1] <= m_middleStepCoeff)	// 중간 기울기
//						{
//							inclin_zero++;
//							m_inclin_up_flag = true;
//
//							if (inclin_zero == 1)
//								m_baseROILXTemp[m_baseROILXTmp_idx++] = i + m_BaseROI.lx;
//						}
//						else if (!m_inclin_down_flag && m_inclin_up_flag && inclin_zero >= 3 && inclin_down < 3 && m_inclin_arr[j - 1] < m_downStepCoeff)	// 종료 기울기
//						{
//							inclin_down++;
//							m_inclin_zero_flag = true;
//
//							if (inclin_down == 1)
//								m_baseROIRXTemp[m_baseROIRXTmp_idx++] = i + m_BaseROI.lx;
//						}
//						else if (!m_inclin_down_flag && m_inclin_zero_flag && inclin_down >= 3)
//						{
//							m_inclin_down_flag = true;
//
//							inclin_up = 0;
//							inclin_zero = 0;
//							inclin_down = 0;
//
//							m_inclin_up_flag = false;
//							m_inclin_zero_flag = false;
//							m_inclin_down_flag = false;
//						}
//					}
//				}
//				else if (m_discrim_idx < MAX_DISCRIMINANT_NUM)
//				{
//					m_discrim_arr[m_discrim_idx++] = m_row_projection[i];
//				}
//			}
//			/////////////////////////// END OF PROJECTION ALGORITHM //////////////////////////
//			*/
//
//			if(m_low_temp_idx > m_baseROIRXTmp_idx)	
//				isPxlZero = true;
//			else
//				isPxlZero = false;
//
//			/////////////////////////// 가로축 야스리 작업 ///////////////////////////////////
//			//
//			if (!isPxlZero)
//			{
//				if (m_lx_pxl_avg_idx > 4)
//				{
//					int lx_sum[MAX_ROI_NUM] = { 0, };
//					int rx_sum[MAX_ROI_NUM] = { 0, };
//
//					for (int i = 0; i < m_baseROIRXTmp_idx; i++)
//					{
//						for (int j = 1; j < m_lx_pxl_avg_idx; j++)
//						{
//							if (m_lx_pxl_avg[i][j] == 0)
//								m_lx_pxl_avg[i][j] = m_baseROILXTemp[i];
//							m_lx_pxl_avg[i][j - 1] = m_lx_pxl_avg[i][j];
//							lx_sum[i] += m_lx_pxl_avg[i][j];
//
//							if (m_rx_pxl_avg[i][j] == 0)
//								m_rx_pxl_avg[i][j] = m_baseROIRXTemp[i];
//							m_rx_pxl_avg[i][j - 1] = m_rx_pxl_avg[i][j];
//							rx_sum[i] += m_rx_pxl_avg[i][j];
//						}
//
//						if (abs(m_baseROILXTemp[i] - min_x[m_SelXPxlChrt_idx]) < 4)
//						{
//							m_lx_pxl_avg[i][m_lx_pxl_avg_idx - 1] = m_baseROILXTemp[i];
//							lx_sum[i] += m_baseROILXTemp[i];
//						}
//						else
//							lx_sum[i] += m_lx_pxl_avg[i][m_lx_pxl_avg_idx - 1];
//						if (abs(m_baseROIRXTemp[i] - max_x[m_SelXPxlChrt_idx]) < 4)
//						{
//							m_rx_pxl_avg[i][m_lx_pxl_avg_idx - 1] = m_baseROIRXTemp[i];
//							rx_sum[i] += m_baseROIRXTemp[i];
//						}
//						else
//							rx_sum[i] += m_rx_pxl_avg[i][m_lx_pxl_avg_idx - 1];
//
//						min_x[m_SelXPxlChrt_idx] = (int)(lx_sum[i] / MAX_PIXEL_AVG_NUM + 0.5);
//						max_x[m_SelXPxlChrt_idx] = (int)(rx_sum[i] / MAX_PIXEL_AVG_NUM + 0.5);
//
//						min_y[m_SelXPxlChrt_idx] = m_col_min;
//						max_y[m_SelXPxlChrt_idx] = m_col_max;
//
//						m_SelXPxlChrt_idx++;
//					}
//				}
//				else
//				{
//					for (int i = 0; i < m_baseROIRXTmp_idx; i++)
//					{
//						if (abs(m_baseROILXTemp[i] - min_x[m_SelXPxlChrt_idx]) < 2 || abs(m_baseROILXTemp[i] - min_x[m_SelXPxlChrt_idx]) > 3)
//						{
//							min_x[m_SelXPxlChrt_idx] = m_baseROILXTemp[i];
//							m_lx_pxl_avg[i][m_lx_pxl_avg_idx] = m_baseROILXTemp[i];
//						}
//						if (abs(m_baseROIRXTemp[i] - max_x[m_SelXPxlChrt_idx]) < 2 || abs(m_baseROIRXTemp[i] - max_x[m_SelXPxlChrt_idx]) > 3)
//						{
//							max_x[m_SelXPxlChrt_idx] = m_baseROIRXTemp[i];
//							m_rx_pxl_avg[i][m_lx_pxl_avg_idx] = m_baseROIRXTemp[i];
//						}
//
//						min_y[m_SelXPxlChrt_idx] = m_col_min;
//						max_y[m_SelXPxlChrt_idx] = m_col_max;
//
//						m_SelXPxlChrt_idx++;
//					}
//
//					m_lx_pxl_avg_idx++;
//				}
//
//				for (int i = m_baseROIRXTmp_idx; i < MAX_ROI_NUM; i++)
//				{
//					min_x[i] = 0;
//					max_x[i] = 0;
//				}
//			}
//		}
//
//		//////////////////////// 가로축 야스리 작업 끝 ////////////////////////////////
//
//
//		///////////////////// ROI 내 최대, 최소, 평균 온도 구하기 ///////////////////////
//		if (m_low_temp_idx < m_SelXPxlChrt_idx)
//			m_ROI_loop_count = m_SelXPxlChrt_idx;
//		else if (m_SelXPxlChrt_idx == 0)
//			m_ROI_loop_count = m_lRow_arr_idx;
//		else
//			m_ROI_loop_count = m_low_temp_idx;
//
//		for (int k = 0; k < m_ROI_loop_count; k++)
//		{
//			for (int j = min_y[k]; j <= max_y[k]; j++)
//			{
//				for (int i = min_x[k]; i <= max_x[k]; i++)
//				{
//					float val = pBuffer[sizeX*j + i];
//
//					pixelCount[k]++;
//
//					sumTemp[k] += val;
//
//					if (isArrFirst[k]) {
//						m_ResultData.TMin[k] = m_ResultData.TMax[k] = val;
//						isArrFirst[k] = false;
//					}
//					else {
//						if (val < m_ResultData.TMin[k])
//						{
//							m_ResultData.TMin[k] = val;
//							m_ResultData.TMinX[k] = i;
//							m_ResultData.TMinY[k] = j;
//						}
//						else if (val > m_ResultData.TMax[k])
//						{
//							m_ResultData.TMax[k] = val;
//							m_ResultData.TMaxX[k] = i;
//							m_ResultData.TMaxY[k] = j;
//						}
//					}
//				}
//			}
//			m_ResultData.TAvg[k] = sumTemp[k] / pixelCount[k];
//		}
//	}
//
//	/////////////////////// 온도 계산 끝 ////////////////////////////
//
//	for (int i = 0; i < m_ROICount; i++)
//	{
//		if (m_ResultData.TMin[i] <= 0 || (min_x[i] == 0 && min_y[i] == 0 && max_x[i] == 0 && max_y[i] == 0))
//			m_ResultData.TMin[i] = 0.0f;
//		else if (m_ResultData.TMin[i] < 600)
//			m_ResultData.TMin[i] = 600.0f;
//		if (m_ResultData.TMax[i] <= 0 || (min_x[i] == 0 && min_y[i] == 0 && max_x[i] == 0 && max_y[i] == 0))
//			m_ResultData.TMax[i] = 0.0f;
//		if (m_ResultData.TAvg[i] <= 0 || (min_x[i] == 0 && min_y[i] == 0 && max_x[i] == 0 && max_y[i] == 0))
//			m_ResultData.TAvg[i] = 0.0f;
//
//		if (m_BaseROI.lx > min_x[i] || m_BaseROI.ty > min_y[i] || m_BaseROI.rx < max_x[i] || m_BaseROI.by < max_y[i])
//		{
//			min_x[i] = 0;	max_x[i] = 0;
//			min_y[i] = 0;	max_y[i] = 0;
//		}
//	}
//}


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
	if(m_RHK_logging)
		CalculateResult(PPointArr.GetCount(), sizeX, sizeY, pFrameBuffer);

	g_cs.Unlock();

	// Update GlassView ///////////////
	UpdateAllViews(NULL, 3);

	theApp.m_pPropertyWnd->UpdateDataSet();
	//****************************************************************************************

	/// 여기다 CalRHKTotalDist() 넣을 건데 조건을 아래와 같이 준다.
	// Trigger 사용을 하지 않을 시 - 코드 쓰지 말고 calrhktotaldist에서 ending 에서 device do stop만 넣어 주자 [엔딩시에 laser 타이머 다시 재 가동+종료할 시 타이머 제대로 종료되나 다시 확인]
	if (!m_TriggerOnOff && theApp.m_bLoggingRunning)
		CalRHKTotalDist();
	else if (m_TriggerOnOff && m_NearFromDev && theApp.m_Laser_Distance <= m_TriggerDist && theApp.m_Laser_Distance > 0)
	//else if (m_TriggerOnOff && m_NearFromDev && m_TriggerDist <= 30)
	{
		if(!theApp.m_bLoggingRunning)
			OnDeviceLoggingStart();
		CalRHKTotalDist();
	}
	else if (m_TriggerOnOff && !m_NearFromDev && theApp.m_Laser_Distance >= m_TriggerDist && theApp.m_Laser_Distance > 0)
	//else if (m_TriggerOnOff && !m_NearFromDev && m_TriggerDist >= 2005)
	{
		if (!theApp.m_bLoggingRunning)
			OnDeviceLoggingStart();
		CalRHKTotalDist();
	}

	// Trigger 사용을 할 시 - 

	// 화면 속도에 따라 저장 (시간 트리거에 따라 저장하려면 이전 프로젝트의 m_writeFlag 참조) 
	if ((m_hIRDX_WRITE != INVALID_HANDLE_VALUE) && m_bRawDataSave)
	{
		if (theApp.m_bLoggingRunning)
		{
			//CalRHKTotalDist();

			if (theApp.m_StatusInfFlag && theApp.m_FocusFlag)
			{
				//ModifyFocusLoc();
				RHKStatusInf();
			}

			if (m_RHK_logging && theApp.m_TimerFlag)
			{
				theApp.m_TimerFlag = false;

				if (!theApp.DDAQ_IRDX_FILE_AddIRDX(m_hIRDX_WRITE, m_hIRDX_DEVICE) || !SaveRHKData(newRHKDataFileName, true))
				{
					OnDeviceLoggingStop();
					return;
				}
			}
			else if(theApp.m_TimerFlag)
			{
				theApp.m_TimerFlag = false;

				if (!theApp.DDAQ_IRDX_FILE_AddIRDX(m_hIRDX_WRITE, m_hIRDX_DEVICE))
				{
					OnDeviceLoggingStop();
					return;
				}
			}

		}
	}

	//if (theApp.m_bLoggingRunning)
	//{
	//	m_AcqIndex++;
	//}

	///FSetFrequency();

	// Enable NextMessage
	theApp.DDAQ_DEVICE_DO_ENABLE_NextMsg(m_nDDAQDeviceNo);
}

void CPyroSoftMDoc::FSetFrequency()
{
	if (m_AcqIndex == 1)
	{
		CString s;
		int fav = m_FPS / m_Avg; /// 적합한 변수 필요

		if (!theApp.DDAQ_DEVICE_DO_Stop(m_nDDAQDeviceNo))
			return;

		/// 화면 Freq 에 따라 변수를 넣어주면 된다.
		s.Format("%d (%.1f Hz)", m_Avg, fav);	/// 적합한 변수 필요
		theApp.m_pPropertyWnd->pDataAcq_AcqFreq->SetValue((_variant_t)(s));
		theApp.DDAQ_IRDX_ACQUISITION_SetAveraging(m_hIRDX_DEVICE, 8);

		if (!theApp.DDAQ_DEVICE_DO_Start(m_nDDAQDeviceNo))
			return;
	}
}

void CPyroSoftMDoc::ModifyFocusLoc()
{
	unsigned short	Pos;
	unsigned short	Status;
	unsigned long	version;

	theApp.DDAQ_IRDX_ACQUISITION_GetMotorFocusPos(m_hIRDX_Doc, &Pos, &Status);

	theApp.DDAQ_IRDX_ACQUISITION_SetMotorFocusPos(m_hIRDX_Doc, m_FocusLocation);

	theApp.DDAQ_IRDX_ACQUISITION_GetMotorFocusPos(m_hIRDX_Doc, &Pos, &Status);
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
		else
		{
			if ((int)theApp.m_Laser_Distance > 0)
				strFocusCount.Format("%d", (int)(theApp.m_Laser_Distance));
			else
				strFocusCount = "--";

			statusStr.Format("동작 시간 : " + strSpenTime + "  이동 거리 : " + strMovDistance + "(mm)   레이저 거리 : " + strFocusCount + "(mm)");
		}
	}
	else
		strFocusCount = "--";

	pMWnd->SetStatusText(statusStr);		//하단 상태바에 좌표값과 온도값 표시
}

void CPyroSoftMDoc::CalRHKTotalDist()
{
	CString timeDistance;

	if (m_RHKDistance >= m_ZoneDistance[m_RHKZoneCount] && m_NumOfZone - 1 > m_RHKZoneCount)		// Zone 구간의 거리를 넘었을 시
	{
		m_RHKZoneCount++;

		m_RHKStepTime = m_DateTime;
		m_RHKPreDistance += m_RHKDistance;

		if (m_TriggerOnOff && theApp.m_Laser_Distance >= 0)		//Laser Trigger가 on일 때
		//if (m_TriggerOnOff )		//Laser Trigger가 on일 때
		{
			bool nearOnOff = false;

			if (m_NearFromDev)				// Trigger 기준 거리보다 작을 시
				if (theApp.m_Laser_Distance >= m_TriggerDist && theApp.m_Laser_Distance >= 5)
				//if (30 >= m_TriggerDist)
					nearOnOff = true;
				else
					nearOnOff = false;
			else                           // Trigger 기준 거리보다 클 시
				if (theApp.m_Laser_Distance <= m_TriggerDist && theApp.m_Laser_Distance >= 5)
				//if (30 <= m_TriggerDist)
					nearOnOff = true;
				else
					nearOnOff = false;

			if (!m_RHK_log_done && m_RHKZoneCount > m_EndingPos)	// Ending Position 까지 가면 종료해라
			{
				OnDeviceLoggingStop();

				m_TriggerOnOff = false;
				theApp.m_pPropertyWnd->pDataTriggerFlag->SetValue(m_TriggerOnOff);
				m_triggerLoggingFirst = false;

				for (int i = 0; i < m_ROICount; i++)
				{
					m_ResultData.TMax[i] = 0;
					m_ResultData.TMin[i] = 0;
					m_ResultData.TAvg[i] = 0;
				}

				UpdateAllViews(NULL, 8);
			}
			else if (!m_RHK_log_done && !m_RHK_logging && m_RHKZoneCount >= m_StartingPos && m_triggerLoggingFirst)
			{
				m_RHK_logging = true;
			}
			else if (!m_RHK_log_done && !m_RHK_logging && nearOnOff && !m_triggerLoggingFirst)	// Laser Trigger의 기준 거리를 만족 했을 시
			{
				m_triggerLoggingFirst = true;		//logging stop에 false 로 초기화 해주는것 필요 + m_LaserTimerID 타이머 실행 시키기

				if (theApp.m_LaserTimerID != NULL)
					timeKillEvent(theApp.m_LaserTimerID);
			}
		}
		else                // Laser Trigger가 off 일 때
		{
			if (!m_RHK_log_done && m_RHKZoneCount > m_EndingPos)		// Ending Position 까지 도달 하면 종료
			{
				OnDeviceLoggingStop();

				for (int i = 0; i < m_ROICount; i++)
				{
					m_ResultData.TMax[i] = 0;
					m_ResultData.TMin[i] = 0;
					m_ResultData.TAvg[i] = 0;
				}
			}
			else if (!m_RHK_log_done && !m_RHK_logging && m_RHKZoneCount >= m_StartingPos)	// Starting Position이 시작하면 저장 시작해라
			{
				m_RHK_logging = true;

				if (theApp.m_LaserTimerID != NULL)
					timeKillEvent(theApp.m_LaserTimerID);
			}
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

		if (m_Auto_ET_Mode)
		{
			if (m_RHKTotalDistance > m_ET_Distance[m_ET_idx])
			{
				//m_Emissivity = m_resultEmissivity[m_ET_idx];   
				m_Emissivity = m_ET_Emissivity[m_ET_idx];
				m_Transmission = m_ET_Transmission[m_ET_idx];

				theApp.m_pPropertyWnd->pMeasObj_Emissivity->SetValue((_variant_t)m_Emissivity);
				theApp.DDAQ_IRDX_OBJECT_SetEmissivity(m_hIRDX_Doc, m_Emissivity);

				theApp.m_pPropertyWnd->pMeasObj_Trans->SetValue((_variant_t)m_Transmission);
				theApp.DDAQ_IRDX_OBJECT_SetTransmission(m_hIRDX_Doc, m_Transmission);

				m_ET_idx++;
				UpdateAllViews(NULL, 2);
			}
		}
	}

	/// m_FocusingIndex 로 최종적으로 수정해야한다.
	CalFocusLocation();
}

void CPyroSoftMDoc::CalFocusLocation()
{
	m_FocusLocation = m_ThirdCeoff * pow(m_RHKTotalDistance, 3) + m_SecondCeoff * pow(m_RHKTotalDistance, 2) + m_FirstCeoff* pow(m_RHKTotalDistance, 1) + m_Intercept;
}

/////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::LoadConfig()
{
	FILE *stream;
	float tmpFirstCoeff;

	char dummy[128];

	::GetModuleFileName(::AfxGetInstanceHandle(), m_AppPath, _MAX_PATH);
	PathRemoveFileSpec(m_AppPath);

	CString ConfigFileName = m_AppPath + CString("\\config.ini");

	stream = fopen((CStringA)ConfigFileName, "rt");

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
	fscanf(stream, "Location=%d\n", &m_FocusingIndex);

	//[Infomation]
	fgets(dummy, 127, stream);
	fscanf(stream, "Threshold=%f\n", &m_Threshold);
	fscanf(stream, "ROICount=%d\n", &m_ROICount);
	fscanf(stream, "TriggerOnOff=%d\n", &m_TriggerOnOff);
	fscanf(stream, "NearTrigger=%d\n", &m_NearFromDev);
	fscanf(stream, "TriggerDistance=%f\n", &m_TriggerDist); 

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
	fscanf(stream, "ROINumberFlag=%d\n", &m_ROIShow);
	fscanf(stream, "POINumberFlag=%d\n", &m_POIShowNum);
	fscanf(stream, "MaxPointerFlag=%d\n", &m_maxPointerShow);
	fscanf(stream, "MinPointerFlag=%d\n", &m_minPointerShow);

	// [Base ROI]
	fgets(dummy, 127, stream);
	fscanf(stream, "(X1,Y1)=(%d,%d)\n", &m_BaseROI.lx, &m_BaseROI.ty);
	fscanf(stream, "(X2,Y2)=(%d,%d)\n\n", &m_BaseROI.rx, &m_BaseROI.by);

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

	bool ret = theApp.DDAQ_SetTempPrecision(0);

	if (m_OpenMode == 3)
	{
		theApp.DDAQ_IRDX_OBJECT_SetEmissivity(m_hIRDX_Doc, m_Emissivity);
		theApp.DDAQ_IRDX_OBJECT_SetTransmission(m_hIRDX_Doc, m_Transmission);
		theApp.DDAQ_IRDX_PALLET_SetBar(m_hIRDX_Doc, m_nBar, m_NoCol);
		theApp.DDAQ_IRDX_SCALE_SetMinMax(m_hIRDX_Doc, m_ScaleMin, m_ScaleMax);
		theApp.DDAQ_IRDX_PALLET_SetIsotherm(m_hIRDX_Doc, 1, m_MinTemp, m_ScaleIsoTherm, RGB(0, 0, 0), 0, false);
	}
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

/// #############################################################################

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
	//fscanf(stream, "StartingDistance=%dmm\n", &m_RcpStartDist);
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
	fprintf(stream, "Location=%d\n\n", m_FocusingIndex);

	//[NMS Infomation]
	fprintf(stream, "[Infomation]\n");
	fprintf(stream, "Threshold=%.2f\n", m_Threshold);
	fprintf(stream, "ROICount=%d\n", m_ROICount);
	fprintf(stream, "TriggerOnOff=%d\n", m_TriggerOnOff);
	fprintf(stream, "NearTrigger=%d\n", m_NearFromDev);
	fprintf(stream, "TriggerDistance=%.0f\n\n", m_TriggerDist);


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
	fprintf(stream, "DownStepDifference=%.2f\n\n", m_downStepCoeff);

	// [Screen Flag]
	fprintf(stream, "[Screen Flag]\n");
	fprintf(stream, "CursorTempFlag=%d\n", m_cursorShow);
	fprintf(stream, "ROINumberFlag=%d\n", m_ROIShow);
	fprintf(stream, "POINumberFlag=%d\n", m_POIShowNum);
	fprintf(stream, "MaxPointerFlag=%d\n", m_maxPointerShow);
	fprintf(stream, "MinPointerFlag=%d\n\n", m_minPointerShow);

	// [Base ROI]
	fprintf(stream, "[Base ROI]\n");
	fprintf(stream, "(X1,Y1)=(%d,%d)\n", m_BaseROI.lx, m_BaseROI.ty);
	fprintf(stream, "(X2,Y2)=(%d,%d)\n\n", m_BaseROI.rx, m_BaseROI.by);

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

	fprintf(m_RHKStream, "%s\t\t%s\n", "StartingPos", m_StartingPos);

	fprintf(m_RHKStream, "%s\t\t%s\n", "EndingPos", m_EndingPos);

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

bool CPyroSoftMDoc::SaveResultData(CString FileName, int sizeX, int sizeY, float* pBuffer)
{
	FILE *stream;
	CFileFind pFind;

	CString ConfigFileName = FileName;

	stream = fopen((CStringA)ConfigFileName, "a+");

	BOOL bRet = pFind.FindFile((CStringA)ConfigFileName);

	// [Mesurement]
	fprintf(stream, "[Mesurement]\n");
	fprintf(stream, "Emissivity=%.2f\n", m_Emissivity);
	fprintf(stream, "Transmission=%.2f\n\n", m_Transmission);

	// [Scaling]
	fprintf(stream, "[Scaling]\n");
	fprintf(stream, "ColorBar=%d\n", m_nBar);
	fprintf(stream, "NumberOfColor=%d\n", m_NoCol);
	fprintf(stream, "Max=%.0f℃\n", m_ScaleMax);
	fprintf(stream, "Min=%.0f℃\n", m_ScaleMin);
	fprintf(stream, "IsoTherm=%.0f℃\n\n", m_ScaleIsoTherm);

	// [Focus]
	fprintf(stream, "[Focus]\n");
	fprintf(stream, "Location=%d\n\n", m_FocusingIndex);

	//[NMS Infomation]
	fprintf(stream, "[NMS Infomation]\n");
	fprintf(stream, "Threshold=%.2f℃\n\n", m_Threshold);

	// [Reference ]
	fprintf(stream, "[Reference]\n");

	// [Measurement]
	fprintf(stream, "[Measurement]\n");
	fprintf(stream, "HFOV=%.1f℃\n", m_CamHFOV);
	fprintf(stream, "VFOV=%.1f℃\n", m_CamVFOV);
	fprintf(stream, "IncidentAngle=%.1f℃\n", m_CamIncidentAngle);
	fprintf(stream, "Distance=%.2fcm\n\n\n", m_CamDistance);

	fclose(stream);

	return bRet;
}

void CPyroSoftMDoc::CloseRHKData()
{
	if (m_bRawDataSave)
	{
		CTime DateTime = CTime::GetCurrentTime();
		CString	strDate = DateTime.Format(_T("%Y-%m-%d %H:%M:%S"));

		fseek(m_RHKStream, SS_ET_OFFSET, SEEK_SET);
		fprintf(m_RHKStream, "%s\t\t%s\n", "EndTime  ", strDate);
		fclose(m_RHKStream);
	}
}


bool CPyroSoftMDoc::SaveRHKData(CString FileName, bool flag)
{
	CFileFind pFind;

	CString ConfigFileName = FileName;

	BOOL bRet = pFind.FindFile((CStringA)ConfigFileName);

	SaveRowData(m_RHKStream, flag);

	return bRet;
}

void CPyroSoftMDoc::SaveRowData(FILE *stream, bool flag)
{
	unsigned long id = 0;
	unsigned long type = 0;

	theApp.DDAQ_IRDX_DEVICE_GetID(m_hIRDX_Doc, &id, &type);

	bool ok = false;
	float temp = 0.0f;

	theApp.DDAQ_IRDX_DEVICE_GetCameraTemp(m_hIRDX_Doc, &temp, &ok);

	fprintf(stream, "%d\t", m_AcqIndex++);

	fprintf(stream, "%s\t", m_StrDateTime);

	fprintf(stream, "%.2lf\t", m_RHKTotalDistance);

	fprintf(stream, "%.2f\t", m_Emissivity);

	fprintf(stream, "%.2f\t", m_Transmission);

	for (int i = 0; i < m_ROICount; i++)
	{
		fprintf(stream, "%.2f\t%.2f\t%.2f\t", m_ResultData.TMin[i], m_ResultData.TMax[i], m_ResultData.TAvg[i]);
	}

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
	//theApp.m_pMainWnd->SetWindowText("RHK Thermal Imaging System");

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
	int IRDX = 0;
	int RawData = 1;
	int ResultData = 2;
	int RHKData = 3;

	theApp.m_bLoggingRunning = true;
	m_bRawDataSave = true;
	m_ChartFlag = true;

	m_SaveEveryRecord = true;
	theApp.m_StatusInfFlag = true;
	CString newIRDXFileName = GetNewDataFileName(IRDX);
	newRawDataFileName = GetNewDataFileName(RawData);
	newResultDataFileName = GetNewDataFileName(ResultData);
	newRHKDataFileName = GetNewDataFileName(RHKData);

	m_RHKStartTime = CTime::GetCurrentTime();

	// ,주기 1000ms
	TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	theApp.m_ClientTimerID = timeSetEvent(1000, caps.wPeriodMin, ClientTimer, (DWORD)NULL, TIME_PERIODIC);

	// laser timer, 주기 100ms
	timecaps_tag laser_caps;
	timeGetDevCaps(&laser_caps, sizeof(caps));
	theApp.m_LaserTimerID = timeSetEvent(LASER_TIMER_FREQUENCY, laser_caps.wPeriodMin, LaserTimer, (DWORD)NULL, TIME_PERIODIC);

	//IRDX File 만들기
	if (theApp.DDAQ_IRDX_FILE_OpenWrite(newIRDXFileName, true, &m_hIRDX_WRITE))
		m_bRawDataOpen = true;
	//if (theApp.DDAQ_IRDX_FILE_OpenReadWrite(newIRDXFileName, true, &m_hIRDX_WRITE))
	//	m_bRawDataOpen = true;
	//if (theApp.DDAQ_IRDX_FILE_OpenWrite(newIRDXFileName, true, &m_hIRDX_WRITE))
	//	m_bRawDataOpen = true;

	if (!CreateRHKDataFile(newRHKDataFileName))
		AfxMessageBox("파일을 만들지 못하였습니다.\n경로를 확인하세요.");

	// Property 비활성화
	theApp.m_pPropertyWnd->pDataAcq_AcqFreq->Enable(FALSE);

	theApp.m_pPropertyWnd->pRawDataSaveEnable->Enable(FALSE);
	theApp.m_pPropertyWnd->pRawDataSaveFilePath->Enable(FALSE);
	theApp.m_pPropertyWnd->pResultDataSaveFilePath->Enable(FALSE);
	//theApp.m_pPropertyWnd->pMeasureDataAutoETMode->Enable(FALSE);

	theApp.m_pPropertyWnd->UpdateData();

	UpdateAllViews(NULL, 1);
}

void CPyroSoftMDoc::OnUpdateDeviceDoStart(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nDDAQDeviceNo && !theApp.m_bLoggingRunning);
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
	//theApp.m_pPropertyWnd->pMeasureDataAutoETMode->Enable(TRUE);

	UpdateAllViews(NULL, 1);
}

void CPyroSoftMDoc::OnDeviceLoggingStop()
{
	m_AcqIndex = 0;		//속성창의 Data Acquisition ->Index

	if (m_hIRDX_WRITE != INVALID_HANDLE_VALUE && m_bRawDataOpen) {
		theApp.DDAQ_IRDX_FILE_Close(m_hIRDX_WRITE);
		CloseRHKData();
		m_bRawDataOpen = false;
		m_bFrameNum = 0;
	}

	m_bRawDataSave = false;
	theApp.m_bLoggingRunning = false;
	m_RHK_log_done = false;

	m_RHK_logging = false;	// 데이터 로깅 Flag
	m_ET_idx = 0;			// Emissivity, Transmission 인덱스 초기화
	m_row_switch_count = 0; // 소성로의 대상체가 온도가 switch 될때 flag count !!!!!!!!
	m_low_temp_idx = 0;		// 대상물의 온도가 낮을 때 ROI 갯수

	theApp.m_RHK_name = "";

	if (theApp.m_ClientTimerID != NULL)
		timeKillEvent(theApp.m_ClientTimerID);

	// Property 활성화
	theApp.m_pPropertyWnd->pDataAcq_AcqFreq->Enable(TRUE);

	theApp.m_pPropertyWnd->pRawDataSaveEnable->Enable(TRUE);
	theApp.m_pPropertyWnd->pRawDataSaveFilePath->Enable(TRUE);
	theApp.m_pPropertyWnd->pResultDataSaveFilePath->Enable(TRUE);

	m_RHKEndTime = CTime::GetCurrentTime();

	// ROI값들 초기화		///mody 05-17
	InitROI();
	// RHK값 초기화
	InitRHKValues();

	if (theApp.m_FocusFlag && m_OpenMode == 3)
	{
		BeginWaitCursor();
		////////
		unsigned short Pos;
		unsigned short Status;

		theApp.DDAQ_IRDX_ACQUISITION_GetMotorFocusPos(m_hIRDX_Doc, &Pos, &Status);

		theApp.DDAQ_IRDX_ACQUISITION_SetMotorFocusPos(m_hIRDX_Doc, 0);
		Sleep(Pos * 2 / 3);
		theApp.DDAQ_IRDX_ACQUISITION_GetMotorFocusPos(m_hIRDX_Doc, &Pos, &Status);
		////////
		EndWaitCursor();
	}

	UpdateAllViews(NULL, 1);
}

void CPyroSoftMDoc::InitROI()
{
	for (int i = m_ROICount; i < MAX_ROI_NUM; i++){
		min_x[i] = 0;
		max_x[i] = 0;
	}
}

void CPyroSoftMDoc::InitRHKValues()
{
	m_RHKDistance = 0.0f;
	m_RHKPreDistance = 0.0f;
	m_RHKTotalDistance = 0.0f;
	m_RHKZoneCount = -1;
	
	m_RHKSpenTime = m_RHKStartTime - m_RHKStartTime;

	/// 끝이냐 처음이냐에 따라 값이 달라진다.
	m_FocusLocation = 0;
}

///////////////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::OnUpdateDeviceDoStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nDDAQDeviceNo && theApp.m_bLoggingRunning);
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

///////////////////////////////////////////////////////////////////////////////////////
void CPyroSoftMDoc::OnButtonFocusN()
{

	bool ret = theApp.DDAQ_DEVICE_DO_MotorFocus(m_nDDAQDeviceNo, DDAQ_MOTORFOCUS_CMD_NEAR);
	//bool ret = true;

	if (ret && m_FocusingIndex>0) {
		m_FocusingIndex = 0;

		CString s;
		s.Format("%d", m_FocusingIndex);
		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_wndProperties.pFocusLocation->SetValue((_variant_t)(s));
	}
}


void CPyroSoftMDoc::OnButtonFocusNStep()
{
	bool ret = theApp.DDAQ_DEVICE_DO_MotorFocus(m_nDDAQDeviceNo, DDAQ_MOTORFOCUS_CMD_NEAR_STEP);
	//bool ret = true;

	if (ret && m_FocusingIndex>0) {
		m_FocusingIndex--;

		CString s;
		s.Format("%d", m_FocusingIndex);
		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_wndProperties.pFocusLocation->SetValue((_variant_t)(s));
	}
}


void CPyroSoftMDoc::OnButtonFocusFStep()
{
	bool ret = theApp.DDAQ_DEVICE_DO_MotorFocus(m_nDDAQDeviceNo, DDAQ_MOTORFOCUS_CMD_FAR_STEP);
	//bool ret = true;

	if (ret && m_FocusingIndex<172) {
		m_FocusingIndex++;

		CString s;
		s.Format("%d", m_FocusingIndex);
		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_wndProperties.pFocusLocation->SetValue((_variant_t)(s));
	}
}


void CPyroSoftMDoc::OnButtonFocusF()
{
	bool ret = theApp.DDAQ_DEVICE_DO_MotorFocus(m_nDDAQDeviceNo, DDAQ_MOTORFOCUS_CMD_FAR);
	//bool ret = true;

	if (ret && m_FocusingIndex<172) {
		m_FocusingIndex = 172;

		CString s;
		s.Format("%d", m_FocusingIndex);
		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pFrame->m_wndProperties.pFocusLocation->SetValue((_variant_t)(s));
	}
}


void CPyroSoftMDoc::OnUpdateFocusN(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nDDAQDeviceNo && m_OpenMode == 3 && m_FocusingIndex>0);
	//pCmdUI->Enable(m_FocusingIndex>0);
}


void CPyroSoftMDoc::OnUpdateFocusNStep(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nDDAQDeviceNo && m_OpenMode == 3 && m_FocusingIndex>0);
	//pCmdUI->Enable(m_FocusingIndex>0);
}


void CPyroSoftMDoc::OnUpdateFocusFStep(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nDDAQDeviceNo && m_OpenMode == 3 && m_FocusingIndex<172);
	//pCmdUI->Enable(m_FocusingIndex<172);
}


void CPyroSoftMDoc::OnUpdateFocusF(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nDDAQDeviceNo && m_OpenMode == 3 && m_FocusingIndex<172);
	//pCmdUI->Enable(m_FocusingIndex<172);
}
