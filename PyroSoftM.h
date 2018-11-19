
// PyroSoftM.h : PyroSoftM 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

#include "..\DIASDAQ\DIASDAQDLL.h"

#include "PropertiesWnd.h"
#include "ResultDlg.h"
#include "ScaleDlg.h"
#include "LogoDlg.h"


/// Valid Device ////////////////////////////////////////////////
#define TEST_DEVICE_SERIAL		"C1139027"
#define SAMSUNG_EM_SERIAL		"C1118828"

/// //////////////////////////////////////////////////////////////

#define IDT_TIMER_SCREEN_REFRESH   1


// CPyroSoftMApp:
// 이 클래스의 구현에 대해서는 PyroSoftM.cpp을 참조하십시오.
//

/////////////////////////////////////////////////////////////////////////////

typedef enum
{
	Msg_No,			// no messaging, polling
	Msg_Window,		// message to main frame window
	Msg_Thread,		// message to application thread
	Msg_Event		// event to application thread
}	TMessagingType;	//

typedef enum
{
	PropertyMode = 0,
	SynchronousWriteMode,
	AsynchronousWriteMode,
	AddMode
} EnumUseMode;

typedef enum
{
	EventMode = 0,
	SynchronousReadCacheMode,
	SynchronousReadDeviceMode,
	AsynchronousRefreshCacheMode,
	AsynchronousRefreshDeviceMode,
	AsynchronousRead
} EnumReadMode;

/////////////////////////////////////////////////////////////////////////////


class CPyroSoftMDoc;
class CDataMsgThread;
class CDataSocket;

class CAppTraceView;
/////////////////////////////////////////////////////////////////////////////


class CPyroSoftMApp : public CWinAppEx
{
public:
	CPyroSoftMApp();
	//EnumReadMode  GetReadMode();

	// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);		// Ok

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnDeviceDoOpensimulation();
	afx_msg void OnDeviceDoOpen();
	afx_msg void OnFileOpen();
	afx_msg void OnDeviceDoScan();

	DECLARE_MESSAGE_MAP()

	// New Add ////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	//////////////////MainFrame/////////////////////////////////

	unsigned long		m_nNumDetectDevices;				///< number of real detectec devices from DDAQ-DLL
	CPyroSoftMDoc*		m_DocWithDevice[DDAQ_MAX_DEVICES];		///< documents with devices 

	TMessagingType		m_MessagingType;					///< type of messaging for new data ready

	CDataMsgThread*		m_pDataMsgThread;					///< if messaging with thread

	EnumUseMode			m_iUseMode;


	BOOL				m_bOpenViaBuffer;					///< active during open via buffer

															//////////////////MainFrame/////////////////////////////////
	CString				m_strServerName;
	CString				m_strProgID;
	CString				m_strCLSID;

	////////////////////////////////////////////////////////////
	
	CString				m_strSingleValue;

public:
	CPyroSoftMDoc * pDocTemp;

	UINT				m_TimerID;

	//				DDAQ_DEVICE functions
	bool			DDAQ_DEVICE_DO_Open(unsigned long nDevNo, const char* pFileName);
	bool			DDAQ_DEVICE_DO_OpenSimulation(unsigned long nDevNo, const char* pFileName);
	bool			DDAQ_DEVICE_DO_Close(unsigned long nDevNo);
	bool			DDAQ_DEVICE_DO_Start(unsigned long nDevNo);
	bool			DDAQ_DEVICE_DO_StartSingleShot(unsigned long nDevNo);
	bool			DDAQ_DEVICE_DO_Stop(unsigned long nDevNo);
	bool			DDAQ_DEVICE_DO_UpdateData(unsigned long nDevNo);
	bool			DDAQ_DEVICE_DO_DownloadImage(unsigned long nDevNo, unsigned short ImageIdx, const char* pFileName);
	bool			DDAQ_DEVICE_DO_DeleteImages(unsigned long nDevNo);
	bool			DDAQ_DEVICE_DO_MotorFocus(unsigned long nDevNo, unsigned short Cmd);

	bool			DDAQ_DEVICE_GET_Id(unsigned long nDevNo, unsigned long* pID, unsigned long* pType);
	bool			DDAQ_DEVICE_GET_IDString(unsigned long nDevNo, CString& s);
	bool			DDAQ_DEVICE_GET_InterfaceString(unsigned long nDevNo, CString& s);
	bool			DDAQ_DEVICE_GET_IRDXHandle(unsigned long nDevNo, HANDLE* pH);
	bool			DDAQ_DEVICE_GET_NewDataReady(unsigned long nDevNo, bool* pbool);
	bool			DDAQ_DEVICE_GET_UpdateFrequency(unsigned long nDevNo, float* pFrequ);
	bool			DDAQ_DEVICE_GET_NumMRanges(unsigned long nDevNo, unsigned short* pNum);
	bool			DDAQ_DEVICE_GET_MRInfoString(unsigned long nDevNo, unsigned short mr, CString& s);
	bool			DDAQ_DEVICE_GET_ErrorState(unsigned long nDevNo, unsigned long* pBuf, unsigned long nBufSize);
	bool			DDAQ_DEVICE_GET_Din(unsigned long nDevNo, unsigned long* pDIN);
	bool			DDAQ_DEVICE_GET_CameraMode(unsigned long nDevNo, bool* pOK);
	bool			DDAQ_DEVICE_GET_DetectorTemp(unsigned long nDevNo, float* pTemp, bool* pOK);
	bool			DDAQ_DEVICE_GET_CameraTemp(unsigned long nDevNo, float* pTemp, bool* pOK);
	bool			DDAQ_DEVICE_GET_NumImages(unsigned long nDevNo, unsigned short* pNumImages);

	bool			DDAQ_DEVICE_SET_Dout(unsigned long nDevNo, unsigned long DOUT);
	bool			DDAQ_DEVICE_SET_MsgHWND(unsigned long nDevNo, HWND hWnd);
	bool			DDAQ_DEVICE_SET_MsgThreadID(unsigned long nDevNo, unsigned long nThreadID);
	bool			DDAQ_DEVICE_SET_MsgHEvent(unsigned long nDevNo, HANDLE hEvent);
	bool			DDAQ_DEVICE_DO_ENABLE_NextMsg(unsigned long nDevNo);

	//				DDAQ_IRDX functions
	bool			DDAQ_IRDX_FILE_OpenMem(HANDLE* phIRDX);
	bool			DDAQ_IRDX_FILE_OpenRead(const char* pFileName, HANDLE* phIRDX);
	bool			DDAQ_IRDX_FILE_OpenWrite(const char* pFileName, bool append, HANDLE* phIRDX);
	bool			DDAQ_IRDX_FILE_OpenReadWrite(const char* pFileName, bool append, HANDLE* phIRDX);
	bool			DDAQ_IRDX_FILE_Close(HANDLE hIRDX);
	bool			DDAQ_IRDX_FILE_Save(HANDLE hIRDX, const char* pFileName);
	bool			DDAQ_IRDX_FILE_AddIRDX(HANDLE hIRDX, HANDLE hIRDX_add);
	bool			DDAQ_IRDX_FILE_GetNumDataSets(HANDLE hIRDX, unsigned long* pNumDS);
	bool			DDAQ_IRDX_FILE_GetCurDataSet(HANDLE hIRDX, unsigned long* pDS);
	bool			DDAQ_IRDX_FILE_SetCurDataSet(HANDLE hIRDX, unsigned long DS);
	bool			DDAQ_IRDX_FILE_GetModified(HANDLE hIRDX, bool* pModif);
	bool			DDAQ_IRDX_FILE_RemoveDataSet(HANDLE hIRDX, unsigned long DS);

	bool			DDAQ_IRDX_DEVICE_GetID(HANDLE hIRDX, unsigned long* pID, unsigned long* pType);
	bool			DDAQ_IRDX_DEVICE_GetIDString(HANDLE hIRDX, CString& s);
	bool			DDAQ_IRDX_DEVICE_GetRangeMinMax(HANDLE hIRDX, float* pMin, float* pMax);
	bool			DDAQ_IRDX_DEVICE_GetInternalFPS(HANDLE hIRDX, float* pFps);
	bool			DDAQ_IRDX_DEVICE_GetCameraMode(HANDLE hIRDX, bool* pOK);
	bool			DDAQ_IRDX_DEVICE_GetDetectorTemp(HANDLE hIRDX, float* pTemp, bool* pOK);
	bool			DDAQ_IRDX_DEVICE_GetCameraTemp(HANDLE hIRDX, float* pTemp, bool* pOK);

	bool			DDAQ_IRDX_OBJECT_GetEmissivity(HANDLE hIRDX, float* pEmiss);
	bool			DDAQ_IRDX_OBJECT_SetEmissivity(HANDLE hIRDX, float Emiss);
	bool			DDAQ_IRDX_OBJECT_GetTransmission(HANDLE hIRDX, float* pTrans);
	bool			DDAQ_IRDX_OBJECT_SetTransmission(HANDLE hIRDX, float Trans);
	bool			DDAQ_IRDX_OBJECT_GetAmbientTemp(HANDLE hIRDX, float* pAmbientTemp);
	bool			DDAQ_IRDX_OBJECT_SetAmbientTemp(HANDLE hIRDX, float AmbientTemp);
	bool			DDAQ_IRDX_OBJECT_GetAmbientTempCorr(HANDLE hIRDX, unsigned short* pmode, float* poffs);
	bool			DDAQ_IRDX_OBJECT_SetAmbientTempCorr(HANDLE hIRDX, unsigned short mode, float offs);
	bool			DDAQ_IRDX_OBJECT_GetTransmissionCorr(HANDLE hIRDX, unsigned short* pCorrMode, float* pRefTemp, float* pRefEmmis, unsigned short* pLeft, unsigned short* pTop, unsigned short* pRight, unsigned short* pBottom, float* pWeight, float* pMinTransm);
	bool			DDAQ_IRDX_OBJECT_SetTransmissionCorr(HANDLE hIRDX, unsigned short CorrMode, float RefTemp, float RefEmmis, unsigned short Left, unsigned short Top, unsigned short Right, unsigned short Bottom, float Weight, float MinTransm);
	bool			DDAQ_IRDX_OBJECT_DoTransmissionCorr(HANDLE hIRDX, float* pCalcTransm, unsigned short* pValidity);
	bool			DDAQ_IRDX_OBJECT_GetTransmissionCorrState(HANDLE hIRDX, float* pCalcTransm, unsigned short* pValidity);

	bool			DDAQ_IRDX_ACQUISITION_GetTimeStampString(HANDLE hIRDX, CString& date, CString& time);
	bool			DDAQ_IRDX_ACQUISITION_GetAveraging(HANDLE hIRDX, unsigned short* pav);
	bool			DDAQ_IRDX_ACQUISITION_SetAveraging(HANDLE hIRDX, unsigned short av);
	bool			DDAQ_IRDX_ACQUISITION_GetMeasurementRange(HANDLE hIRDX, unsigned short* pmr);
	bool			DDAQ_IRDX_ACQUISITION_SetMeasurementRange(HANDLE hIRDX, unsigned short mr);
	bool			DDAQ_IRDX_ACQUISITION_GetMRInfoString(HANDLE hIRDX, CString& s);;
	bool			DDAQ_IRDX_ACQUISITION_GetNumFrames(HANDLE hIRDX, unsigned short* pNumFrames);
	bool			DDAQ_IRDX_ACQUISITION_SetNumFrames(HANDLE hIRDX, unsigned short NumFrames);
	bool			DDAQ_IRDX_ACQUISITION_GetValidFrames(HANDLE hIRDX, unsigned short* pValidFrames);
	bool			DDAQ_IRDX_ACQUISITION_GetSingleTrigger(HANDLE hIRDX, unsigned short* ptrig);
	bool			DDAQ_IRDX_ACQUISITION_SetSingleTrigger(HANDLE hIRDX, unsigned short trig);
	bool			DDAQ_IRDX_ACQUISITION_GetSequenceTrigger(HANDLE hIRDX, unsigned short* ptrig);
	bool			DDAQ_IRDX_ACQUISITION_SetSequenceTrigger(HANDLE hIRDX, unsigned short trig);
	bool			DDAQ_IRDX_ACQUISITION_GetShutterTrigger(HANDLE hIRDX, unsigned short* ptrig);
	bool			DDAQ_IRDX_ACQUISITION_SetShutterTrigger(HANDLE hIRDX, unsigned short trig);
	bool			DDAQ_IRDX_ACQUISITION_GetTempSequenceTrigger(HANDLE  hIRDX, float* pThreshold, unsigned short* pMode);
	bool			DDAQ_IRDX_ACQUISITION_SetTempSequenceTrigger(HANDLE hIRDX, float Threshold, unsigned short Mode);
	bool			DDAQ_SetTempPrecision(unsigned short precision);

	bool			DDAQ_IRDX_SCALE_GetMinMax(HANDLE hIRDX, float* pMin, float* pMax);
	bool			DDAQ_IRDX_SCALE_SetMinMax(HANDLE hIRDX, float Min, float Max);
	bool			DDAQ_IRDX_SCALE_GetAutoMode(HANDLE hIRDX, unsigned short* pMode, float* pAVPlus, float* pAVMinus);
	bool			DDAQ_IRDX_SCALE_SetAutoMode(HANDLE hIRDX, unsigned short Mode, float AVPlus, float AVMinus);
	bool			DDAQ_IRDX_SCALE_GetStep(HANDLE hIRDX, float* pStep);
	bool			DDAQ_IRDX_SCALE_SetStep(HANDLE hIRDX, float Step);

	bool			DDAQ_IRDX_PALLET_GetBar(HANDLE hIRDX, unsigned short* pBar, unsigned short* pNumCol);
	bool			DDAQ_IRDX_PALLET_SetBar(HANDLE hIRDX, unsigned short Bar, unsigned short NumCol);
	bool			DDAQ_IRDX_PALLET_GetBackgroundColor(HANDLE hIRDX, unsigned long* pBkCol);
	bool			DDAQ_IRDX_PALLET_SetBackgroundColor(HANDLE hIRDX, unsigned long BkCol);
	bool			DDAQ_IRDX_PALLET_GetTextColor(HANDLE hIRDX, unsigned long* pTxtCol);
	bool			DDAQ_IRDX_PALLET_SetTextColor(HANDLE hIRDX, unsigned long TxtCol);
	bool			DDAQ_IRDX_PALLET_GetNumIsotherms(HANDLE hIRDX, unsigned short* pNumIsoth);
	bool			DDAQ_IRDX_PALLET_SetNumIsotherms(HANDLE hIRDX, unsigned short NumIsoth);
	bool			DDAQ_IRDX_PALLET_GetIsotherm(HANDLE hIRDX, unsigned short nNo, float* pMin, float* pMax, unsigned long* pCol, unsigned short* pSat, bool* pTransp);
	bool			DDAQ_IRDX_PALLET_SetIsotherm(HANDLE hIRDX, unsigned short nNo, float Min, float Max, unsigned long Col, unsigned short Sat, bool Transp);
	bool			DDAQ_IRDX_PALLET_GetBitmap(HANDLE hIRDX, unsigned short Width, unsigned short Height, void** ppBits, BITMAPINFO** ppBitmapInfo);
	bool			DDAQ_IRDX_PALLET_GetBitmapScale(HANDLE hIRDX, unsigned short Width, unsigned short Height, void** ppBits, BITMAPINFO** ppBitmapInfo);
	bool			DDAQ_IRDX_PALLET_GetBitmapScaleEx(HANDLE hIRDX, unsigned short Width, unsigned short Height, void** ppBits, BITMAPINFO** ppBitmapInfo, long* pparam, unsigned long size);
	bool			DDAQ_IRDX_PALLET_GetBarColor(HANDLE hIRDX, unsigned short Idx, unsigned long* pCol);

	bool			DDAQ_IRDX_IMAGE_GetSize(HANDLE hIRDX, unsigned short* pSizeX, unsigned short* pSizeY);
	bool			DDAQ_IRDX_IMAGE_GetZoom(HANDLE hIRDX, unsigned short* pZMode, float* pZoom);
	bool			DDAQ_IRDX_IMAGE_SetZoom(HANDLE hIRDX, unsigned short ZMode, float pZoom);
	bool			DDAQ_IRDX_IMAGE_GetDrawMode(HANDLE hIRDX, unsigned short* pDrawMode);
	bool			DDAQ_IRDX_IMAGE_SetDrawMode(HANDLE hIRDX, unsigned short DrawMode);
	bool			DDAQ_IRDX_IMAGE_GetBitmap(HANDLE hIRDX, unsigned short* pWidth, unsigned short* pHeight, void** ppBits, BITMAPINFO** ppBitmapInfo);

	bool			DDAQ_IRDX_PIXEL_GetSize(HANDLE hIRDX, unsigned short* pSizeX, unsigned short* pSizeY);
	bool			DDAQ_IRDX_PIXEL_GetMinMax(HANDLE hIRDX, float* pMin, float* pMax);
	bool			DDAQ_IRDX_PIXEL_GetAverage(HANDLE hIRDX, float* pAV);
	bool			DDAQ_IRDX_PIXEL_GetAVStdDev(HANDLE hIRDX, float* pAV, float* pStdDev);
	bool			DDAQ_IRDX_PIXEL_GetData(HANDLE hIRDX, float* pData, unsigned long BufSize);
	bool			DDAQ_IRDX_PIXEL_GetDataPoint(HANDLE hIRDX, unsigned short x, unsigned short y, float* pData);

	bool			DDAQ_IRDX_USERDATA_GetSize(HANDLE hIRDX, unsigned short no, unsigned long* pSize);
	bool			DDAQ_IRDX_USERDATA_GetData(HANDLE hIRDX, unsigned short no, unsigned long Size, void* pData);
	bool			DDAQ_IRDX_USERDATA_SetData(HANDLE hIRDX, unsigned short no, unsigned long Size, void* pData);

	bool			DDAQ_IRDX_BUFFER_GetSize(HANDLE hIRDX, DWORD* pBufSize);
	bool			DDAQ_IRDX_BUFFER_WriteTo(HANDLE hIRDX, BYTE* pBuf, DWORD BufSize, DWORD* pUsedBufSize);
	bool			DDAQ_IRDX_BUFFER_OpenFrom(BYTE* pBuf, DWORD BufSize, HANDLE* pIRDX);
	bool			DDAQ_IRDX_BUFFER_WriteToEx(HANDLE hIRDX, BYTE* pBuf, DWORD BufSize, DWORD* pUsedBufSize, DWORD* pRawDataOfs, DWORD* pRawDataSize);
	bool			DDAQ_IRDX_BUFFER_OpenFromEx(BYTE* pBuf, DWORD BufSize, HANDLE* pIRDX, DWORD* pRawDataOfs, DWORD* pRawDataSize);
	bool			DDAQ_IRDX_ACQUISITION_GetMotorFocusPos(HANDLE hIRDX, unsigned short* pPos, unsigned short* pState);
	bool			DDAQ_IRDX_ACQUISITION_SetMotorFocusPos(HANDLE hIRDX, unsigned short	Pos);
	bool			DDAQ_DEVICE_GET_MOTOR_FocusVersion(unsigned long nDAQDevNo, unsigned long* pVersion);

	void				DoErrorMessage(UINT IDS, unsigned long nErrorCode, BOOL with_box = TRUE);

	void				Devicedetection();

	void				IRDXUpdateThread();

	TMessagingType		GetMessagingType();

	DWORD				GetMessageThreadID();

	BOOL				GetbOpenViaBuffer() { return m_bOpenViaBuffer; }

	void				EndDataMsgThread();
	//void				PropertyUpdateMsgThread();
	double				roundXL(double x, int digit);

public:
	CPyroSoftMDoc*		GetDocumentFromDevice(unsigned short DevNo);

	CString				m_strImpExpFileName;  // Name of the file used to export or import data.
	UINT				m_uReadPeriod;      // Read Period
	EnumReadMode		m_uReadMode;          // Read Mode

	BOOL				m_bHiColorIcons;

	// Window Pointers
	CScaleDlg*			m_pScaleDlg;
	CLogoDlg*			m_pLogoDlg;
	CPropertiesWnd*		m_pPropertyWnd;
	CResultDlg*			m_pResultDlg;
	CDataSocket*		m_pDataSocket;

	//float64				m_Laser_Distance;
	double				m_Laser_mA;

	//TaskHandle			DAQ_TaskHandle;

	char				AppPath[_MAX_PATH];
	unsigned short		IRDX_Mode;

	CString				m_RHK_name;

	////////////////////////////////////////////////////////////
	bool				m_StatusInfFlag;
	bool				m_FocusFlag;
	bool				m_TchartFlag;
	bool				m_TimerFlag;
	unsigned long		m_ForcedFocusFlg;

	BOOL				m_bLoggingRunning;
	BOOL				m_bIRDXLoggingRunning;

	//TimerID
	UINT				m_ClientTimerID;
	UINT				m_loggingIntervalTimerID;
	
	unsigned long		TimerEventID;

	bool				m_isSamsung_dev;

	int					nResultDlgWidth;
	int					nStatusDlgHeight;
	int					nScaleDlgWidth;
	int					adjust_count;
	int					m_childFrmWidth;
	int					m_childFrmHeight;

	int					m_loggingInterval;
	int					m_acqFreq;
	
	BOOL				m_bFileOpen;
	bool				m_bCameraConnected;

	CListBox		m_ctlListValues;
	DWORD			m_dwTransactionID; // Client transaction ID
	CString			m_systemDate;
	CString			m_systemTime;

};

extern CPyroSoftMApp theApp;
