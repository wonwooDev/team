
// PyroSoftMDoc.h : CPyroSoftMDoc 클래스의 인터페이스
//


#pragma once

#include "..\DIASDAQ\DIASDAQDLL.h"
#include "NIDAQmx.h"


#define MIN_X_PIXEL				0
#define MAX_X_PIXEL				512 //320 //512
#define	MIN_Y_PIXEL				0
#define	MAX_Y_PIXEL				384 //240 //384

#define	 N_AREA					8

#define	DECTECTION_TIMEOUT_SEC	300

#define MIN(x,y)				(x < y ? x : y) 
#define MAX(x,y)				(x > y ? x : y) 
#define POLYGONINSIDE			0 
#define POLYGONOUTSIDE			1 

#define MAXHFOV					36 //52 //36
#define	MINHFOV					1
#define	MAXVFOV					27 //40 //27
#define MINVFOV					1
#define	MAXINCIDENTANGLE		90
#define	MININCIDENTANGLE		30
#define	MAXCAMDISTANCE			10000
#define	MINCAMDISTANCE			0

#define GAPOFPIXEL				8

//포스코 제 4철강 래들과 카메라와의 거리 및 각도 등의 값
#define	ARRLENGTH				49
#define PIXELARRLEN				48
#define	EACHANGLEOFEIGHTPIEXL(x)	(x / 2.0f) / (MAX_Y_PIXEL / 2.0f) * 8

#define	PI	3.14159265358979323846

#define MAX_ROI_NUM				50
#define MAX_ET_PER_DISTANCE		200

// 소성로 챔버의 Zone의 최대 갯수
#define MAX_ZONE				50
#define MAX_POI_NUM				20

#define LASER_TIMER_ID			71129		// 레이져 ID값 - 201(7 11 29)
#define LASER_TIMER_FREQUENCY	100			// 레이져 반복실행 시간 값		

#define MAX_DISCRIMINANT_NUM	4
#define MAX_PIXEL_AVG_NUM		5

#define UNDISPLAY				0
#define TEMPER_ONLY				1
#define NUMBER_ONLY				2
#define SHOW_ALL				3




typedef struct
{
	float	Min;
	float	Max;
	float	Avg;

	UINT	left, top, right, bottom;


} RESULT_T;

typedef struct
{
	float Min;
	float Max;
	float Avg;

	float TMin[MAX_ROI_NUM];
	float TMax[MAX_ROI_NUM];
	float TAvg[MAX_ROI_NUM];

	int TMinX[MAX_ROI_NUM];
	int TMinY[MAX_ROI_NUM];
	int TMaxX[MAX_ROI_NUM];
	int TMaxY[MAX_ROI_NUM];
} RESULT_RD;

typedef struct
{
	int lx;
	int rx;
	int ty;
	int by;
} BASE_ROI;


class CPyroSoftMDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CPyroSoftMDoc();
	DECLARE_DYNCREATE(CPyroSoftMDoc)

	unsigned long		m_nDDAQDeviceNo;	///< device number from DDAQ-DLL (0 invalid, 1..DDAQ_MAX_DEVICES)
	HANDLE				m_hIRDX_DEVICE;		///< handle for irdx access to device
	CString				m_DeviceString;		///< device string with no and interface (only online)
	BOOL				m_bAcqIsRunning;	///< allways start
											//BOOL				m_bSingleShot;		///< if only one single shot

	int					m_bStretchDIB;		///< if stretch dib

	HANDLE				m_hIRDX_Doc;		///< general irdx handle for the document

											//HANDLE			m_hIRDX_MEM;		///< optional handle for irdx MEM
	HANDLE				m_hIRDX_READ;		///< optional handle for irdx READ
	HANDLE				m_hIRDX_WRITE;		///< optional handle for irdx WRITE
											//HANDLE			m_hIRDX_READWRITE;	///< optional handle for irdx READWRITE

	bool				m_AppendIRDXFile;	///< do append to existing file
	bool				m_SaveEveryRecord;	///< if online saving
	bool				m_triggerLoggingFirst;
	int					m_low_temp_idx;

	HANDLE				m_hEventData;		///< if event signal for new data

	FILE*				m_RHKStream;

	CString				newRawDataFileName;
	CString				newResultDataFileName;
	CString				newRHKDataFileName;
	int					m_bFrameNum;
	long int			SS_ET_OFFSET;

	void CalROI(int index, int sizeX, int sizeY, float* pBuffer);
	void CalMeasurement(int index, int sizeX, int sizeY, float* pBuffer);
	void GetCamNBufferSize();
	void CloseRHKData();
	void CalRHKTotalDist();
	void RHKStatusInf();

	// 특성입니다.
public:
	unsigned short		m_OpenMode;

	//온도 데이터를 모두 읽어옴
	float*				pFrameBuffer;

	unsigned long		m_NoDS, m_IdxDS;
	BOOL				m_bDataPlay;

	// Raw Data
	bool				m_bRawDataSave;
	CString				m_strRawDataFilePath;
	CString				m_strRawDataFileName;
	BOOL				m_bRawDataOpen;

	// Ref Max Point
	bool				m_bMaxPointEnable;

	// Result Data
	CString				m_strResultDataFilePath;
	CString				m_strResultDataFileName;


	int					m_PixelX, m_PixelY;
	// Device
	float				m_MaxTemp, m_MinTemp;
	float				m_FPS;
	unsigned short		m_Avg;

	float				m_Emissivity, m_Transmission, m_AmbTemp;

	// Scale
	unsigned short		m_nBar;
	unsigned short		m_NoCol;
	float				m_ScaleMin;
	float				m_ScaleMax;
	float				m_ScaleIsoTherm;
	float				m_IsoThermMin;

	// Acqusition		
	unsigned long		m_AcqIndex;

	// Detection
	unsigned short		m_PixelSize;

	bool				m_bShowPointer;

	bool				m_RefAreaFlag;
	bool				m_ChartFlag;
	bool				m_RHK_logging;
	bool				m_RHK_log_done;
	bool				m_Auto_ET_Mode;
	bool				m_TriggerOnOff;
	bool				m_NearFromDev;
	float				m_TriggerDist;

	bool				m_ROI_check_flag;
	int					m_comp_ROICount;
	
	CArray<POINT, POINT&> m_POIArr;

	CTime				m_DateTime;
	CString				m_StrDateTime;
	// Detection *****************************************************************************	
	unsigned short		sizeX, sizeY;
	unsigned long		BufSize;

	//구조체 선언
	RESULT_T			m_Result[N_AREA];
	RESULT_RD			m_ResultData;
	BASE_ROI			m_BaseROI;
	CArray <POINT, POINT&> PPointArr;

	// Focus
	int					m_FocusingIndex;
	BOOL				m_TestROI;

	//Camera
	float				m_CamHFOV, m_CamVFOV;
	float				m_CamIncidentAngle;
	float				m_CamDistance;
	
	CTime				m_RHKStartTime;
	CTime				m_RHKEndTime;
	CTime				m_RHKStepTime;
	CTimeSpan			m_RHKSpenTime;
	double				m_RHKDistance;
	double				m_RHKPreDistance;
	double				m_RHKTotalDistance;
	int					m_RHKZoneCount;
	UINT				m_FocusLocation;

	double				m_FirstCeoff;
	double				m_SecondCeoff;
	double				m_ThirdCeoff;
	int					m_Intercept;

	int					m_Thermal_Gap;
	int					m_ROICount;
	int					m_Font_Size;
	int					m_ET_idx;

	int					m_ET_dataCount;
	int					m_POI_count;

	bool				columnUpdateFlag;

	int					m_SelXPxlChrt_idx;
	int					m_SelYPxlChrt_idx;

	int					m_row_switch_count;
	int					m_lx_pxl_avg_idx;

	int					m_ROI_loop_count = 0;

	float				m_Threshold;

	float				m_upStepCoeff;
	float				m_middleStepCoeff;
	float				m_downStepCoeff;

	CArray<int, int&>	RunningIndex;
	CArray<int, int&>	DeletedIndex;
	bool				indexMoveFlag;
	
	bool				m_cursorShow;
	bool				m_ROIShow;
	int					m_POIShowNum;
	bool				m_maxPointerShow;
	bool				m_minPointerShow;

	int					m_ZoomMode;
	float				m_ZoomRatio;

	int					m_baseROILXTemp[MAX_ROI_NUM];
	int					m_baseROIRXTemp[MAX_ROI_NUM];
	int					m_SelXPixelChart[MAX_ROI_NUM * 2];
	int					m_SelYPixelChart[MAX_ROI_NUM * 2];
	float				m_discrim_arr[MAX_DISCRIMINANT_NUM];
	float				m_inclin_arr[MAX_DISCRIMINANT_NUM];
	float				m_row_projection[MAX_X_PIXEL];
	float				m_lRow_projection[MAX_X_PIXEL];
	float				m_col_projection[MAX_Y_PIXEL];

	double				m_ET_Distance[MAX_ET_PER_DISTANCE];
	double				m_ET_Emissivity[MAX_ET_PER_DISTANCE];
	double				m_ET_Transmission[MAX_ET_PER_DISTANCE];
	double				m_resultEmissivity[MAX_ET_PER_DISTANCE];

	// Recipe Var
	int					m_NumOfZone;
	int					m_RowNum;
	int					m_ColNum;
	int					m_StartingPos;
	int					m_EndingPos;
	int					m_RcpStartDist;
	int					m_ZoneDistance[MAX_ZONE];
	float				m_ZoneSpeed[MAX_ZONE];
	float				m_ZoneTemp[MAX_ZONE];
	float				m_ZoneEmissivity[MAX_ZONE];

	int					max_x[MAX_ROI_NUM];
	int					min_x[MAX_ROI_NUM];
	int					max_y[MAX_ROI_NUM];
	int					min_y[MAX_ROI_NUM];
	int					temptt[MAX_ROI_NUM];
	int					m_ty_pxl_avg[MAX_ROI_NUM][MAX_PIXEL_AVG_NUM];
	int					m_by_pxl_avg[MAX_ROI_NUM][MAX_PIXEL_AVG_NUM];
	int					m_lx_pxl_avg[MAX_ROI_NUM][MAX_PIXEL_AVG_NUM];
	int					m_rx_pxl_avg[MAX_ROI_NUM][MAX_PIXEL_AVG_NUM];

	float				POI_TemperatureArray[MAX_POI_NUM];

	// 작업입니다.
public:

	void				SetDDAQDeviveNo(unsigned long nDevNo);	///< set device number while [File Open], 0 is invalid!

	void				GetAverageString(unsigned short av, CString& s);	///< get average string

	BOOL				IsAcqRunning() { return m_bAcqIsRunning; }

	double				getLineAvgTemp(float *pBuf, unsigned short size);

	CString				GetNewDataFileName(int SaveType);
	void				CalculateResult(int index, int sizeX, int sizeY, float* pBuffer);	///pBuffer는 온도값을 가지고 있는 매개 변수

	TCHAR				m_AppPath[_MAX_PATH];

	void				LoadConfig();
	void				LoadETPerDistance();
	void				SaveConfig();
	void				LoadRecipeConfig();
	bool				CreateRawDataFile(CString FileName);
	bool				CreateResultDataFile(CString FileName);
	bool				CreateRHKDataFile(CString FileName);
	bool				SaveRawData(CString FileName, int sizeX, int sizeY, float* pBuffer);
	bool				SaveResultData(CString FileName, int sizeX, int sizeY, float* pBuffer);
	bool				SaveRHKData(CString FileName, bool flag);
	void				SaveRowData(FILE *stream, bool flag);
	void				OnDeviceDoStart();
	void				OnDeviceDoStop();
	void				InitROI();
	void				InitRHKValues();
	void				CalFocusLocation();
	void				ModifyFocusLoc();
	int					GetFindCharCount(CString parm_string, char parm_find_char);
	void				GetEmissivityBySpline(int pointCount, double distanceArray[], double emissivityArray[]);
	void				FSetFrequency();

	// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	//virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	//virtual void InitializeSearchContent();
	//virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL SaveModified();

	virtual void OnIdle();

	// 구현입니다.
public:
	void	OnNewDataReady();

	virtual ~CPyroSoftMDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnDeviceLoggingStart();
	afx_msg void OnUpdateDeviceDoStart(CCmdUI *pCmdUI);
	afx_msg void OnDeviceLoggingStop();
	afx_msg void OnUpdateDeviceDoStop(CCmdUI *pCmdUI);
	afx_msg void OnDataacquisitionShowmin();
	afx_msg void OnUpdateDataacquisitionShowmin(CCmdUI *pCmdUI);
	afx_msg void OnButtonFocusN();
	afx_msg void OnButtonFocusNStep();
	afx_msg void OnButtonFocusFStep();
	afx_msg void OnButtonFocusF();
	afx_msg void OnUpdateFocusN(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFocusNStep(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFocusFStep(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFocusF(CCmdUI *pCmdUI);
};

void CALLBACK ClientTimer(UINT m_nTimerID, UINT uiMsg, DWORD dwUser, DWORD dw1, DWORD d2);
void CALLBACK LaserTimer(UINT m_nTimerID, UINT uiMsg, DWORD dwUser, DWORD dw1, DWORD d2);