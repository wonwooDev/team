// StatusView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PyroSoftM.h"
#include "ROIGridView.h"


// CStatusView

IMPLEMENT_DYNCREATE(CROIGridView, CView)

CROIGridView::CROIGridView()
{
	spreadCnt = 0;
}

CROIGridView::~CROIGridView()
{
}

BEGIN_MESSAGE_MAP(CROIGridView, CView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CStatusView 그리기입니다.

void CROIGridView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}

// CStatusView 진단입니다.

#ifdef _DEBUG
void CROIGridView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CROIGridView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG

// CStatusView 메시지 처리기입니다.
int CROIGridView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (m_ROIGridDlg.GetSafeHwnd() == 0)
	{
		if (!m_ROIGridDlg.Create(IDD_STATUS_INF, this))
			return FALSE;

		m_ROIGridDlg.ModifyStyle(0, WS_VISIBLE, 0);

		AdjustLayout();
	}

	return 0;
}

void CROIGridView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int dlgH = 0;

	if (theApp.adjust_count > 4)
		theApp.nStatusDlgHeight = rectClient.Width();

	m_ROIGridDlg.SetWindowPos(NULL,
		rectClient.left,
		rectClient.top + dlgH,
		rectClient.Width(),
		rectClient.Height() - dlgH,
		SWP_NOACTIVATE | SWP_NOZORDER);
	//SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE);
}

void CROIGridView::InitStatusData()
{
	CRect rectClient;
	int colWidth;
	GetClientRect(rectClient);

	m_ROIGridDlg.m_ROI_GridView.Clear();

	m_ROIGridDlg.m_ROI_GridView.put_AllowUserResizing(3);				// Allow column, row mouse resizing
	m_ROIGridDlg.m_ROI_GridView.put_Row(1);								// trace row
	m_ROIGridDlg.m_ROI_GridView.put_Cols(pDoc->m_ROICount + 2);

	colWidth = (rectClient.Width() - 73) * 15 / pDoc->m_ROICount;

	SetColProperty(0, pDoc->m_ROICount + 1, "ROI #", colWidth, 0);
	m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(0, pDoc->m_ROICount + 1, _T("Spread"));
	m_ROIGridDlg.m_ROI_GridView.put_ColAlignment(pDoc->m_ROICount + 1, 4);

	// column #1 naming
	m_ROIGridDlg.m_ROI_GridView.put_Rows(2);
	m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(0, 0, _T(""));
	m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(1, 0, _T("Max Temp"));

	// height adjust
	m_ROIGridDlg.m_ROI_GridView.put_RowHeight(1, (rectClient.Height() - 42) / 3 * 15);

	// 중앙 정렬한 column, row를 제외한 나머지 cell은 기본 표기 방법으로 변경
	for (int i = 1; i < 2; i++)
	{
		m_ROIGridDlg.m_ROI_GridView.put_Row(i);

		for (int j = 1; j < pDoc->m_ROICount + 1; j++)
		{
			m_ROIGridDlg.m_ROI_GridView.put_Col(j);
			m_ROIGridDlg.m_ROI_GridView.put_CellAlignment(9);	// flexAlignGeneral
		}
	}
}

void CROIGridView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (lHint<2) return;

	if (lHint < 4)
		UpdateStatusData();

	switch (lHint)
	{
	case 6:			// For IRDX mode
		UpdateStatusData();
	}
}

void CROIGridView::UpdateStatusData()
{
	UpdateGridControlSet();

	// ---- 171211 VERIFYING RESULTDATA ELEMENTS ---- //
	//int arrayElementCnt = sizeof(pDoc->m_ResultData.TMin) / sizeof(pDoc->m_ResultData.TMin[0]);
	int arrayElementCnt = sizeof(pDoc->m_ResultData.TMax) / sizeof(pDoc->m_ResultData.TMax[0]);

	if (pDoc->m_ROICount != arrayElementCnt) {
		for (int i = pDoc->m_ROI_loop_count; i < arrayElementCnt; i++) {
			pDoc->m_ResultData.TMax[i] = 0.0f;
		}
	}

	// ---- 171211 VERIFYING RESULTDATA ELEMENTS ---- //
	if (pDoc->m_POI_count > 0)		// POI가 있을 시 POI&ROI 온도값 출력
	{
		// Print POI
		PrintTemp(0, pDoc->m_POI_count, pDoc->POI_TemperatureArray, 1);

		// Print ROI
		PrintTemp(0, pDoc->m_ROICount, pDoc->m_ResultData.TMax, 2, pDoc->m_POI_count + 1);

		// Print Spread
		CString str;
		str = CheckSpread();
		m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(2, pDoc->m_ROICount + pDoc->m_POI_count + 1, str);
	}
	else							// POI가 없을 시 ROI 온도값 출력
	{
		// Print ROI
		PrintTemp(pDoc->m_POI_count, pDoc->m_ROICount, pDoc->m_ResultData.TMax, 1);

		for (int i = 0; i < pDoc->m_ROI_loop_count; i++)
		{
			if (pDoc->m_ResultData.TMax[i] > 0 && pDoc->m_ResultData.TMax[i] > pDoc->m_spreadDetctRange)
			{
				spreadCnt++;
			}
		}

		CString str;
		str = CheckSpread();

		if (spreadCnt != 0 && spreadCnt == pDoc->m_ROI_loop_count)	// 0이 아닐 조건 추가
		{
			pDoc->m_bCheckSpread = true;

			// Spread 출력
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(1, pDoc->m_ROICount + 1, str);

			m_ROIGridDlg.m_ROI_GridView.put_Col(pDoc->m_ROICount + 1);
			m_ROIGridDlg.m_ROI_GridView.put_CellForeColor(0x0000FF);	// put_CellForeColor( RED );
		}
		else
		{
			pDoc->m_bCheckSpread = false;

			// Spread 출력
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(1, pDoc->m_ROICount + 1, "");

			m_ROIGridDlg.m_ROI_GridView.put_Col(pDoc->m_ROICount + 1);
			m_ROIGridDlg.m_ROI_GridView.put_CellForeColor(0x000000);	// put_CellForeColor( BLACK );
		}
	}

	spreadCnt = 0;
}

void CROIGridView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void CROIGridView::OnSize(UINT nType, int cx, int cy)
{
	if (GetSafeHwnd() == NULL) return;

	CView::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CROIGridView::SetColProperty(int startCnt, int endCnt, CString familyName, int colWidth, long col_index)
{
	CString dummy;

	for (int i = startCnt; i < endCnt; i++)
	{
		dummy.Format("%d", i);

		m_ROIGridDlg.m_ROI_GridView.put_ColAlignment(col_index + i, 4);
		m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(0, col_index + i, familyName + dummy);

		if (colWidth <= m_ROIGridDlg.controlWidth * 1.8)
			m_ROIGridDlg.m_ROI_GridView.put_ColWidth(col_index + i, m_ROIGridDlg.controlWidth * 1.8);
		else
		{
		//	if (i == 0)
		//		m_ROIGridDlg.m_ROI_GridView.put_ColWidth(col_index + i, m_ROIGridDlg.controlWidth * 1.8);
		//	else
				m_ROIGridDlg.m_ROI_GridView.put_ColWidth(col_index + i, colWidth);
		}
	}
}

void CROIGridView::UpdateGridControlSet()
{
	CString dummy = _T("");
	int indexDummy = 0;
	CRect rectClient;

	GetClientRect(rectClient);

	if (pDoc->columnUpdateFlag == true)
	{
		//int colWidth = (rectClient.Width() - 73) * 15 / pDoc->m_ROICount;
		int colWidth = (rectClient.Width() * 12) / (pDoc->m_POI_count + pDoc->m_ROICount + 1);
		int columnTemp = (pDoc->m_POI_count) + (pDoc->m_ROICount) + 2;

		// For redraw of grid control
		m_ROIGridDlg.m_ROI_GridView.Clear();
		m_ROIGridDlg.m_ROI_GridView.put_Row(1);

		// ---- 171128 IMPLEMENTED ---- //
		if (pDoc->m_POI_count > 0) {				// POI가 하나 이상일 경우에만 동작,
			m_ROIGridDlg.m_ROI_GridView.put_Cols(columnTemp);			// trace column

			SetColProperty(0, pDoc->RunningIndex.GetCount(), "POI #", colWidth, 1);

			SetColProperty(1, pDoc->m_ROICount + 1, "ROI #", colWidth, pDoc->m_POI_count);

			m_ROIGridDlg.m_ROI_GridView.put_ColAlignment(pDoc->m_POI_count + pDoc->m_ROICount + 1, 4);
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(0, pDoc->m_POI_count + pDoc->m_ROICount + 1, _T("Spread"));

			for (int i = 1; i < pDoc->m_POI_count + 1; i++)
			{
				m_ROIGridDlg.m_ROI_GridView.put_Row(1);
				m_ROIGridDlg.m_ROI_GridView.put_Col(i);

				// POI 온도를 보는 부분의 셀의 색만 gray
				m_ROIGridDlg.m_ROI_GridView.put_CellBackColor(0xCECECE);	//0xBBGGRR
			}
		}
		else
		{
			m_ROIGridDlg.m_ROI_GridView.put_Cols(pDoc->m_ROICount + 2);	// trace column for roiCount

			SetColProperty(1, pDoc->m_ROICount + 1, "ROI #", colWidth, 0);
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(0, pDoc->m_ROICount + 1, _T("Spread"));
		}
		// ---- 171129 IMPLEMENTED ---- //

		// column #1 naming
		if (pDoc->m_POI_count >= 1)
		{
			unsigned long k = 0x00400000;
			m_ROIGridDlg.m_ROI_GridView.put_Rows(3);
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(0, 0, _T(""));
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(1, 0, _T("POI Temp"));
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(2, 0, _T("Max Temp"));

			

			for (int i = 1; i < 3; i++)
				m_ROIGridDlg.m_ROI_GridView.put_RowHeight(i, (rectClient.Height() - 40) / 4 * 15);

			// 첫번째 column, row에만 text alignment 적용
			for (int i = 1; i < 3; i++)
			{
				m_ROIGridDlg.m_ROI_GridView.put_Row(i);

				for (int j = 1; j < columnTemp; j++)
				{
					m_ROIGridDlg.m_ROI_GridView.put_Col(j);
					m_ROIGridDlg.m_ROI_GridView.put_CellAlignment(9);
				}
			}
		}
		else
		{
			m_ROIGridDlg.m_ROI_GridView.put_Rows(2);
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(0, 0, _T(""));
			m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(1, 0, _T("Max Temp"));

			for (int i = 1; i < 2; i++)
				m_ROIGridDlg.m_ROI_GridView.put_RowHeight(i, (rectClient.Height() - 42) / 3 * 15);


			// 첫번째 column, row에만 text alignment 적용
			for (int i = 1; i < 2; i++)
			{
				m_ROIGridDlg.m_ROI_GridView.put_Row(i);

				for (int j = 1; j < columnTemp; j++)
				{
					m_ROIGridDlg.m_ROI_GridView.put_Col(j);
					m_ROIGridDlg.m_ROI_GridView.put_CellAlignment(9);
				}
			}
		}
	}
	// control redraw end
	pDoc->columnUpdateFlag = false;
}

void CROIGridView::PrintTemp(int startCnt, int endCnt, float* target_temp, long row)
{
	CString str;

	for (int i = startCnt; i < endCnt; i++) {
		if (target_temp[i] > 0 && target_temp[i] <= 2000)
			str.Format("%.1f", target_temp[i]);
		else
			str.Format("%.1f", 0.0f);

		m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(row, i + 1, str);
	}
}

void CROIGridView::PrintTemp(int startCnt, int endCnt, float* target_temp, long row, int dummyCnt)
{
	CString str;

	for (int i = startCnt; i < endCnt; i++) {
		if (target_temp[i] > 0 && target_temp[i] <= 2000)
			str.Format("%.1f", target_temp[i]);
		else
			str.Format("%.1f", 0.0f);

		m_ROIGridDlg.m_ROI_GridView.put_TextMatrix(row, dummyCnt, str);
		dummyCnt++;
	}
}

CString CROIGridView::CheckSpread() 
{
	float minimum, maximum;

	minimum = maximum = pDoc->m_ResultData.TMax[0];

	for (int k = 0; k < pDoc->m_ROICount; k++) {
		if ((pDoc->m_ResultData.TMax[k] > 0) && (minimum > pDoc->m_ResultData.TMax[k])) {
			minimum = pDoc->m_ResultData.TMax[k];
		}
		if ((pDoc->m_ResultData.TMax[k] > 0) && (maximum < pDoc->m_ResultData.TMax[k])) {
			maximum = pDoc->m_ResultData.TMax[k];
		}
	}
	pDoc->m_spreadTempr = maximum - minimum;
	CString dummy;
	dummy.Format("%.1f", pDoc->m_spreadTempr);

	return dummy;
}