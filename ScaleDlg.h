#pragma once

#include "Resource.h"

#include "PyroSoftMDoc.h"
#include "afxwin.h"

// CScaleDlg 대화 상자입니다.

class CScaleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScaleDlg)

public: 
	CPyroSoftMDoc* pDoc;
	CPyroSoftMDoc* GetDocument();

public:
	CScaleDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CScaleDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SCALE };

public:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		
};
