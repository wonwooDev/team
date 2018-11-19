#include "stdafx.h"
#include "OOI.h"

#define QUADRANT1				1		
#define QUADRANT2				2
#define QUADRANT3				3
#define QUADRANT4				4
#define	ELLIPSESTART			5

////////////////////	Strategy Pattern	//////////////////////
COOI::COOI()
{

}

COOI::~COOI()
{

}

void COOI::PerformAttack()
{
	//stretch->Stretch();
}

int COOI::GetPosXY(int XY_pos) {
	int result;

	switch (XY_pos)
	{
	case X_LEFT:
		result = this->lx;
		break;
	case X_RIGHT:
		result = this->rx;
		break;
	case Y_TOP:
		result = this->ty;
		break;
	case Y_BOTTOM:
		result = this->by;
		break;
	default:
		result = 0;
		break;
	}
	return result;
}

bool COOI::SetPosXY(int XY_pos, int val) {
	bool result = TRUE;

	switch (XY_pos)
	{
	case X_LEFT:
		this->lx = val;
		break;
	case X_RIGHT:
		this->rx = val;
		break;
	case Y_TOP:
		this->ty = val;
		break;
	case Y_BOTTOM:
		this->by = val;
		break;
	default:
		result = FALSE;
		break;
	}
	return result;
}

bool COOI::GetDrawDone()
{
	return this->bDrawDone;
}

void COOI::SetDrawDone(bool bDrawFlg)
{
	this->bDrawDone = bDrawFlg;
}

bool COOI::GetOppDrawDone()
{
	return this->bOppDrawDone;
}

void COOI::SetOppDrawDone(bool bDrawFlg)
{
	this->bOppDrawDone = bDrawFlg;
}

int COOI::GetCount()
{
	return this->count;
}

bool COOI::SetCount(int cnt)
{
	bool result = TRUE;

	if (cnt < 0)
		result = FALSE;
	else
		this->count = cnt;

	return result;
}

int COOI::GetOppCount()
{
	return this->opp_count;
}

bool COOI::SetOppCount(int cnt)
{
	bool result = TRUE;

	if (cnt < 0)
		result = FALSE;
	else
		this->opp_count = cnt;

	return result;
}

bool COOI::GetDrawFirst()
{
	return this->bDrawFirst;
}

void COOI::SetDrawFirst(bool bDrawFirstFlg)
{
	this->bDrawFirst = bDrawFirstFlg;
}

bool COOI::GetOppDrawFirst()
{
	return this->bOppDrawFirst;
}

void COOI::SetOppDrawFirst(bool bDrawFirstFlg)
{
	this->bOppDrawFirst = bDrawFirstFlg;
}

void COOI::InputXYVal(int& lx, int& rx, int& ty, int& by)
{
	lx = this->lx;
	rx = this->rx;
	ty = this->ty;
	by = this->by;
}

int COOI::GetWidth() {
	return this->lx > this->rx ? lx - rx : rx - lx;
}

int COOI::GetHeight() {
	return this->ty > this->by ? ty - by : by - ty;
}

void COOI::InitXYPos() {
	this->rx = 0, this->lx = 0, this->ty = 0, this->by = 0;
}

int COOI::GetMinSize() {
	return this->minSize;
}

bool COOI::SelectObj(RECT* CBaseRect, unsigned short ux, unsigned short uy, CPoint m_XY, float zoom, POINT ClickedDistance)
{
	bool result = FALSE;
	int OOI_lx, OOI_rx, OOI_ty, OOI_by;

	InputXYVal(OOI_lx, OOI_rx, OOI_ty, OOI_by);

	if (this->lx < ux &&  this->ty > uy)
	{
		result = SetInvaliRect(CBaseRect, QUADRANT1, 3, 50, 10, 30, m_XY, zoom, ClickedDistance);
	}
	else if (this->lx < ux &&  this->ty < uy)
	{
		result = SetInvaliRect(CBaseRect, QUADRANT2, 3, 3, 10, 30, m_XY, zoom, ClickedDistance);
	}
	else if (this->lx > ux &&  this->ty < uy)
	{
		result = SetInvaliRect(CBaseRect, QUADRANT3, 3, 3, 10, 30, m_XY, zoom, ClickedDistance);
	}
	else if (this->lx > ux &&  this->ty > uy)
	{
		result = SetInvaliRect(CBaseRect, QUADRANT4, 3, 50, 10, 30, m_XY, zoom, ClickedDistance);
	}
	return result;
}

bool COOI::SetInvaliRect(RECT* CBaseRect, int quadrant, int sx, int sy, int ex, int ey, CPoint m_XY, float zoom, POINT ClickedDistance)
{
	bool result = TRUE;
	int OOI_lx, OOI_rx, OOI_ty, OOI_by;

	InputXYVal(OOI_lx, OOI_rx, OOI_ty, OOI_by);

	switch (quadrant)
	{
	case QUADRANT1:
		CBaseRect->left = OOI_lx * zoom - sx;
		CBaseRect->top = m_XY.y - ey;
		CBaseRect->right = m_XY.x + ex;
		CBaseRect->bottom = OOI_ty * zoom + sy;
		break;
	case QUADRANT2:
		CBaseRect->left = OOI_lx * zoom - sx;
		CBaseRect->top = OOI_ty * zoom - sy;
		CBaseRect->right = m_XY.x + ex;
		CBaseRect->bottom = m_XY.y + ey;
		break;
	case QUADRANT3:
		CBaseRect->left = m_XY.x - ex;
		CBaseRect->top = OOI_ty * zoom - sy;
		CBaseRect->right = OOI_lx * zoom + sx;
		CBaseRect->bottom = m_XY.y + ey;
		break;
	case QUADRANT4:
		CBaseRect->left = m_XY.x - ex;
		CBaseRect->top = m_XY.y - ey;
		CBaseRect->right = OOI_lx * zoom + sx;
		CBaseRect->bottom = OOI_ty * zoom + sy;
		break;
	case ELLIPSESTART:
		CBaseRect->left = (ClickedDistance.x - 2) * zoom - sx;
		CBaseRect->top = (ClickedDistance.y - 2) * zoom - sy;
		CBaseRect->right = (ClickedDistance.x + 2) * zoom + ex;
		CBaseRect->bottom = (ClickedDistance.y + 2) * zoom + ey;
		break;
	default:
		result = FALSE;
		break;
	}
	return result;
}



bool COOI::SetMinSize(int val) {
	bool result = TRUE;

	if (val > 0)
		this->minSize = val;
	else
		result = FALSE;

	return result;
}

////	OOI 자식들의 생성자's		////
CBROI::CBROI()
{
	stretch = new CLineStretch(this);
	move = new CRectMove(this);
}

CTROI::CTROI()
{
	stretch = new CStretchNoway(this);
	move = new CMoveNoway(this);
}

CPROI::CPROI()
{
	stretch = new CPointStretch(this);
	move = new CPointMove(this);
}

CLROI::CLROI()
{
	stretch = new CPointStretch(this);
	move = new CLineMove(this);
}

CEROI::CEROI()
{
	stretch = new CLineStretch(this);
	move = new CRectMove(this);
}

////	Stretch 자식들의 생성자's	////
CLineStretch::CLineStretch(COOI* pOOI) {
	this->pOOI = pOOI;
	
	m_bCatch_L_line = false;
	m_bCatch_R_line = false;
	m_bCatch_B_line = false;
	m_bCatch_T_line = false;
}

CPointStretch::CPointStretch(COOI* pOOI) {
	this->pOOI = pOOI;

	m_bCatch_first_point = false;
	m_bCatch_second_point = false;
}

CStretchNoway::CStretchNoway(COOI* pOOI) {
	this->pOOI = pOOI;
}


////	Move 자식들의 생성자's	////
CRectMove::CRectMove(COOI* pOOI)
{
	this->pOOI = pOOI;
}

CLineMove::CLineMove(COOI* pOOI)
{
	this->pOOI = pOOI;
}

CPointMove::CPointMove(COOI* pOOI)
{
	this->pOOI = pOOI;
}

CROIMove::CROIMove(COOI* pOOI)
{
	this->pOOI = pOOI;
}

CMoveNoway::CMoveNoway(COOI* pOOI)
{
	this->pOOI = pOOI;
}


bool COOI::GetDirection()
{
	return this->bSide;
}

void COOI::SetDirection(bool side)
{
	this->bSide = side;
}

int COOI::GetOppPosXY(int XY_pos)
{
	int result;

	switch (XY_pos)
	{
	case X_LEFT:
		result = this->opp_lx;
		break;
	case X_RIGHT:
		result = this->opp_rx;
		break;
	case Y_TOP:
		result = this->opp_ty;
		break;
	case Y_BOTTOM:
		result = this->opp_by;
		break;
	default:
		result = 0;
		break;
	}
	return result;
}

bool COOI::SetOppPosXY(int XY_pos, int val)
{
	bool result = TRUE;

	switch (XY_pos)
	{
	case X_LEFT:
		this->opp_lx = val;
		break;
	case X_RIGHT:
		this->opp_rx = val;
		break;
	case Y_TOP:
		this->opp_ty = val;
		break;
	case Y_BOTTOM:
		this->opp_by = val;
		break;
	default:
		result = FALSE;
		break;
	}
	return result;
}

void CBROI::Display()
{
	AfxMessageBox("This is a BROI!!");
}

void CTROI::Display()
{
	AfxMessageBox("This is a ROI!!");
}

void CPROI::Display()
{
	AfxMessageBox("This is a POI!!");
}

void CLROI::Display()
{
	AfxMessageBox("This is a LOI!!");
}

void CEROI::Display()
{
	AfxMessageBox("This is a CEROI");
}

POINT CLineStretch::Stretch(int line_case, POINT clkDistance, unsigned short ux, unsigned short uy)
{
	int tempLinePos;
	int BROI_lx, BROI_rx, BROI_ty, BROI_by;
	int distanceX, distanceY;

	BROI_lx = pOOI->GetPosXY(X_LEFT);
	BROI_rx = pOOI->GetPosXY(X_RIGHT);
	BROI_ty = pOOI->GetPosXY(Y_TOP);
	BROI_by = pOOI->GetPosXY(Y_BOTTOM);

	distanceX = clkDistance.x - ux;
	distanceY = clkDistance.y - uy;

	switch (line_case)
	{
	case BASE_ROI_LEFT:
		tempLinePos = BROI_lx - (clkDistance.x - ux);

		if (tempLinePos <= 0)
		{
			SetCatchLNP(LEFT_LINE, FALSE);
			//MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (BROI_lx - distanceX > BROI_rx - pOOI->GetMinSize())
			{
				break;
			}
			else if (BROI_lx - distanceX > BROI_rx)
			{
				SetCatchLNP(LEFT_LINE, FALSE);
				SetCatchLNP(RIGHT_LINE, TRUE);
			}
			else
				pOOI->SetPosXY(X_LEFT, BROI_lx - distanceX);

			clkDistance.x = ux;
		}
		break;
	case BASE_ROI_RIGHT:
		tempLinePos = BROI_rx - distanceX;

		if (tempLinePos <= 0)
		{
			SetCatchLNP(RIGHT_LINE, FALSE);
			//MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (BROI_rx - distanceX < BROI_lx + pOOI->GetMinSize())
			{
				break;
			}
			else if (BROI_rx - distanceX < BROI_lx)
			{
				SetCatchLNP(RIGHT_LINE, FALSE);
				SetCatchLNP(LEFT_LINE, TRUE);
			}
			else
				pOOI->SetPosXY(X_RIGHT, BROI_rx - distanceX);

			clkDistance.x = ux;
		}
		break;
	case BASE_ROI_TOP:
		tempLinePos = BROI_ty - distanceY;

		if (tempLinePos <= 0)
		{
			SetCatchLNP(TOP_LINE, FALSE);
			//MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (BROI_ty - distanceY > BROI_by - pOOI->GetMinSize())
			{
				break;
			}
			else if (BROI_ty - distanceY > BROI_by)
			{
				SetCatchLNP(TOP_LINE, FALSE);
				SetCatchLNP(BOTTOM_LINE, TRUE);
			}
			else
				pOOI->SetPosXY(Y_TOP, BROI_ty - distanceY);

			clkDistance.y = uy;
		}
		break;
	case BASE_ROI_BOTTOM:
		tempLinePos = BROI_by - distanceY;

		if (tempLinePos <= 0)
		{
			SetCatchLNP(BOTTOM_LINE, FALSE);
			//MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (BROI_by - distanceY < BROI_ty + pOOI->GetMinSize())
			{
				break;
			}
			else if (BROI_by - distanceY < BROI_ty)
			{
				SetCatchLNP(BOTTOM_LINE, FALSE);
				SetCatchLNP(TOP_LINE, TRUE);
			}
			else
				pOOI->SetPosXY(Y_BOTTOM, BROI_by - distanceY);

			clkDistance.y = uy;
		}
		break;
	}

	return clkDistance;
}

bool CLineStretch::GetCatchLNP(int line_case)
{
	bool result;

	switch (line_case)
	{
	case LEFT_LINE:
		result = m_bCatch_L_line;
		break;
	case RIGHT_LINE:
		result = m_bCatch_R_line;
		break;
	case TOP_LINE:
		result = m_bCatch_T_line;
		break;
	case BOTTOM_LINE:
		result = m_bCatch_B_line;
		break;
	default:
		result = FALSE;
		break;
	}
	return result;
}

bool CLineStretch::SetCatchLNP(int line_case, bool flag)
{
	bool result = TRUE;

	switch (line_case)
	{
	case LEFT_LINE:
		m_bCatch_L_line = flag;
		break;
	case RIGHT_LINE:
		m_bCatch_R_line = flag;
		break;
	case TOP_LINE:
		m_bCatch_T_line = flag;
		break;
	case BOTTOM_LINE:
		m_bCatch_B_line = flag;
		break;
	default:
		result = FALSE;
		break;
	}
	return result;
}


POINT CPointStretch::Stretch(int point_case, POINT clkDistance, unsigned short ux, unsigned short uy)
{
	POINT pt;
	return pt;
}

bool CPointStretch::GetCatchLNP(int point_case) {
	return 0;
}

bool CPointStretch::SetCatchLNP(int point_case, bool flag) {
	return 0;
}

POINT CStretchNoway::Stretch(int caseNoway, POINT clkDistance, unsigned short ux, unsigned short uy) {
	POINT pt;
	return pt;
}

bool CStretchNoway::GetCatchLNP(int point_case) {
	return 0;
}

bool CStretchNoway::SetCatchLNP(int point_case, bool flag) {
	return 0;
}


bool CRectMove::Move(int lx_temp, int ty_temp, int rx_temp, int by_temp) {
	if (lx_temp < 1 || rx_temp < 1 || ty_temp < 1 || by_temp < 1)
		return FALSE;

	pOOI->SetPosXY(X_LEFT, lx_temp);
	pOOI->SetPosXY(X_RIGHT, rx_temp);
	pOOI->SetPosXY(Y_TOP, ty_temp);
	pOOI->SetPosXY(Y_BOTTOM, by_temp);

	return TRUE;
}

bool CLineMove::Move(int lx_temp, int ty_temp, int rx_temp, int by_temp) {
	return 0;
}

bool CPointMove::Move(int lx_temp, int ty_temp, int rx_temp, int by_temp) {
	return 0;
}

bool CROIMove::Move(int lx_temp, int ty_temp, int rx_temp, int by_temp) {
	return 0;
}

bool CMoveNoway::Move(int lx_temp, int ty_temp, int rx_temp, int by_temp) {
	return 0;
}

////////////////////	Strategy Pattern	//////////////////////