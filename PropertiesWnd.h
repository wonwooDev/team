
#pragma once



#include "PyroSoftMDoc.h"

#include "FloatPropertyGridProperty.h"
#include "CheckBoxPropertyGridProperty.h"

#include "PropertyGridCtrl.h"

class CPropertiesWnd : public CDockablePane
{
// 생성입니다.
public:
	CPropertiesWnd();

	void AdjustLayout();

// 특성입니다.
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

public:
	//CMFCPropertyGridCtrl m_wndPropList;
	CPropertyGridCtrl	m_wndPropList;

	CMFCPropertyGridProperty* pFile;
	CMFCPropertyGridProperty* pFile_NoDR;
	CMFCPropertyGridProperty* pFile_CurDR;	

	CMFCPropertyGridProperty* pMeasureera;
	CMFCPropertyGridProperty* pMeasureera_CType;
	CMFCPropertyGridProperty* pMeasureera_SN;
	CMFCPropertyGridProperty* pMeasureera_DTemp;
	CMFCPropertyGridProperty* pMeasureera_CTemp;

	CMFCPropertyGridProperty* pDocSize;
	CMFCPropertyGridProperty* pDocSize_Pixel;

	CMFCPropertyGridProperty* pDataAcq;
	CMFCPropertyGridProperty* pDataAcqIndex;
	CMFCPropertyGridProperty* pDataAcq_Date;
	CMFCPropertyGridProperty* pDataAcq_Time;
	CMFCPropertyGridProperty* pDataAcq_MeasRange;
	CMFCPropertyGridProperty* pDataAcq_AcqFreq;
	CMFCPropertyGridProperty* pDataAcq_LoggingInterval;
	CCheckBoxPropertyGridProperty* pDataTriggerFlag;
	CCheckBoxPropertyGridProperty* pDataAcq_Trigger;
	CMFCPropertyGridProperty* pDataAcq_TriggerDist;
	//CMFCPropertyGridProperty* pDataAcq_NoFrame;

	CMFCPropertyGridProperty* pMeasObj;
	CFLoatPropertyGridProperty*  pMeasObj_Emissivity;
	CFLoatPropertyGridProperty*  pMeasObj_Trans;
	CFLoatPropertyGridProperty*  pMeasObj_AmbTemp;	

	CMFCPropertyGridProperty* pScale;
	CMFCPropertyGridProperty* pColorBar;
	CMFCPropertyGridProperty* pNoColor;
	CFLoatPropertyGridProperty*  pScale_MaxTemp;
	CFLoatPropertyGridProperty*  pScale_MinTemp;
	CFLoatPropertyGridProperty* pScale_IsoTherm;

	CMFCPropertyGridProperty* pDataSave;
	CCheckBoxPropertyGridProperty* pRawDataSaveEnable;
	CMFCPropertyGridFileProperty* pRawDataSaveFilePath;
	CMFCPropertyGridFileProperty* pResultDataSaveFilePath;
	CMFCPropertyGridProperty* pRawDataSaveFileName;

	CMFCPropertyGridProperty* pFocus;
	CMFCPropertyGridProperty* pFocusPosition;
	///CMFCPropertyGridProperty* pFocusSetPosition;

	CMFCPropertyGridProperty* pMeasure;
	CFLoatPropertyGridProperty* pMeasureHFOV;
	CFLoatPropertyGridProperty* pMeasureVFOV;
	CFLoatPropertyGridProperty* pMeasureIncidentAngle;
	CFLoatPropertyGridProperty* pMeasureDistance;

	CMFCPropertyGridProperty* pMeasureData;
	CFLoatPropertyGridProperty* pMeasureDataThreshold;
	CMFCPropertyGridProperty* pMeasureDataROICount;
	CMFCPropertyGridProperty* pMeasureDataBaseROISize;
	CMFCPropertyGridProperty* pMeasureROIBuffer;
	CMFCPropertyGridProperty* pMeasureROIBufferCnt;
	CMFCPropertyGridProperty* pMeasureDataFontSize;
	CCheckBoxPropertyGridProperty* pMeasureDataZoneInfoEnable;
	CCheckBoxPropertyGridProperty* pMeasureDataAutoETMode;

	CMFCPropertyGridProperty* pScreen;
	CMFCPropertyGridProperty* pScreenZoom;
	CCheckBoxPropertyGridProperty* pScreenCursor;
	CCheckBoxPropertyGridProperty* pScreenROI;
	CMFCPropertyGridProperty* pScreenPOI;
	CMFCPropertyGridProperty* pScreenBROI;
	CCheckBoxPropertyGridProperty* pScreenMaxPointer;
	CCheckBoxPropertyGridProperty* pScreenMinPointer;

	CMFCPropertyGridProperty* pSpread;
	CFLoatPropertyGridProperty* pSpreadDetectRange;
	CFLoatPropertyGridProperty* pSpreadLimitTempr;


	CMFCPropertyGridProperty* pResult;
	CMFCPropertyGridProperty* pResultValue;
	CMFCPropertyGridProperty* pResultTemp;


	//////////////////////////////////////////////////
public:
	unsigned long m_NoDR, m_CurrDR;
	
	//unsigned short m_nBar;
	
	
		

// 구현입니다.
public:
	virtual ~CPropertiesWnd();

protected:
	CString NameOfPOI[4];
	CString NameofOOI[4];
	//CString ZoomMode[7];

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnSetFocus(CWnd* pOldWnd);	
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);
	
	DECLARE_MESSAGE_MAP()
		
	CPyroSoftMDoc* GetDocument();

public:		
	void InitPropList();	
	void ChangeProperties(CPyroSoftMDoc* pDoc);
	void UpdateDataSet();
	void InitFreqeuncy(CPyroSoftMDoc *pDoc, unsigned short avg);

	int IndexOfColorBar(CString s);
	int IndexOfNoColor(CString s);
	unsigned short valueOfAverage(CString str, float FPS);
	int valudeOfFrequency(unsigned short avg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void OnChangedZoomOnly();
};

