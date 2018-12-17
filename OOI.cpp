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

bool COOI::IsInsideOOI(unsigned short ux, unsigned short uy) {
		bool insideBaseROI = false;
	
		if (this->lx < ux - 1 && this->rx - 1 > ux && this->ty - 1 < uy && this->by - 1 > uy)
			insideBaseROI = true;
	
		return insideBaseROI;
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

bool COOI::GetInsideFlag() {
	return this->bIsInside;
}

void COOI::SetInsideFlag(bool flag) {
	this->bIsInside = flag;
}

bool COOI::SelectObj(RECT* CBaseRect, unsigned short ux, unsigned short uy, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance)
{
	bool result = FALSE;
	int OOI_lx, OOI_rx, OOI_ty, OOI_by;

	InputXYVal(OOI_lx, OOI_rx, OOI_ty, OOI_by);

	if (this->lx < ux &&  this->ty > uy)
	{
		result = SetInvaliRect(CBaseRect, QUADRANT1, 0.1, 0.1, m_XY, moveOffs_x, moveOffs_y, zoom, ClickedDistance);
	}
	else if (this->lx > ux &&  this->ty > uy)
	{
		result = SetInvaliRect(CBaseRect, QUADRANT2, 0.1, 0.1, m_XY, moveOffs_x, moveOffs_y, zoom, ClickedDistance);
	}
	else if (this->lx > ux &&  this->ty < uy)
	{
		result = SetInvaliRect(CBaseRect, QUADRANT3, 0.1, 0.1, m_XY, moveOffs_x, moveOffs_y, zoom, ClickedDistance);
	}
	else if (this->lx < ux &&  this->ty < uy)
	{
		result = SetInvaliRect(CBaseRect, QUADRANT4, 0.1, 0.1, m_XY, moveOffs_x, moveOffs_y, zoom, ClickedDistance);
	}
	return result;
}


bool COOI::SetInvaliRect(RECT* CBaseRect, int quadrant, double static_factor, double dynamic_factor, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance)
{
	bool result = TRUE;
	int OOI_lx, OOI_rx, OOI_ty, OOI_by;
	double tmp1, tmp2;

	InputXYVal(OOI_lx, OOI_rx, OOI_ty, OOI_by);

	OOI_lx += 1; OOI_rx += 1;
	OOI_ty += 1; OOI_by += 1;

	switch (quadrant)
	{
	case QUADRANT1:
		tmp1 = OOI_lx * zoom ;
		tmp2 = m_XY.x;
		CBaseRect->left = OOI_lx * zoom * (1 - static_factor) + moveOffs_x;
		CBaseRect->top = m_XY.y * (1 - dynamic_factor);
		CBaseRect->right = m_XY.x * (1 + dynamic_factor);
		CBaseRect->bottom = OOI_ty * zoom * (1 + static_factor) + moveOffs_y;
		break;
	case QUADRANT2:
		CBaseRect->left = m_XY.x * (1 - dynamic_factor);
		CBaseRect->top = m_XY.y * (1 - dynamic_factor);
		CBaseRect->right = OOI_lx * zoom * (1 + static_factor) + moveOffs_x;
		CBaseRect->bottom =  OOI_ty * zoom * (1 + static_factor) + moveOffs_y;
		break;
	case QUADRANT3:
		CBaseRect->left = m_XY.x * (1 - dynamic_factor);
		CBaseRect->top = OOI_ty * zoom * (1 - static_factor) + moveOffs_y;
		CBaseRect->right = OOI_lx * zoom * (1 + static_factor) + moveOffs_x;
		CBaseRect->bottom = m_XY.y * (1 + dynamic_factor);
		break;
	case QUADRANT4:
		CBaseRect->left = OOI_lx * zoom * (1 - static_factor) + moveOffs_x;
		CBaseRect->top = OOI_ty * zoom * (1 - static_factor) + moveOffs_y;
		CBaseRect->right = m_XY.x * (1 + dynamic_factor);
		CBaseRect->bottom =  m_XY.y * (1 + dynamic_factor);

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
CRectMove::CRectMove(COOI* pOOI){
	this->pOOI = pOOI;
}

CLineMove::CLineMove(COOI* pOOI){
	this->pOOI = pOOI;
}

CPointMove::CPointMove(COOI* pOOI){
	this->pOOI = pOOI;
}

CMoveNoway::CMoveNoway(COOI* pOOI){
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
	int OOI_lx, OOI_rx, OOI_ty, OOI_by;
	int distanceX, distanceY;

	pOOI->InputXYVal(OOI_lx, OOI_rx, OOI_ty, OOI_by);

	distanceX = clkDistance.x - ux;
	distanceY = clkDistance.y - uy;

	switch (line_case)
	{
	case BASE_ROI_LEFT:
		tempLinePos = OOI_lx - (clkDistance.x - ux);

		if (tempLinePos <= 0)
		{
			SetCatchLNP(LEFT_LINE, FALSE);
			//MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (OOI_lx - distanceX > OOI_rx - pOOI->GetMinSize())
			{
				break;
			}
			else if (OOI_lx - distanceX > OOI_rx)
			{
				SetCatchLNP(LEFT_LINE, FALSE);
				SetCatchLNP(RIGHT_LINE, TRUE);
			}
			else
				pOOI->SetPosXY(X_LEFT, OOI_lx - distanceX);

			clkDistance.x = ux;
		}
		break;
	case BASE_ROI_RIGHT:
		tempLinePos = OOI_rx - distanceX;

		if (tempLinePos <= 0)
		{
			SetCatchLNP(RIGHT_LINE, FALSE);
			//MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (OOI_rx - distanceX < OOI_lx + pOOI->GetMinSize())
			{
				break;
			}
			else if (OOI_rx - distanceX < OOI_lx)
			{
				SetCatchLNP(RIGHT_LINE, FALSE);
				SetCatchLNP(LEFT_LINE, TRUE);
			}
			else
				pOOI->SetPosXY(X_RIGHT, OOI_rx - distanceX);

			clkDistance.x = ux;
		}
		break;
	case BASE_ROI_TOP:
		tempLinePos = OOI_ty - distanceY;

		if (tempLinePos <= 0)
		{
			SetCatchLNP(TOP_LINE, FALSE);
			//MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (OOI_ty - distanceY > OOI_by - pOOI->GetMinSize())
			{
				break;
			}
			else if (OOI_ty - distanceY > OOI_by)
			{
				SetCatchLNP(TOP_LINE, FALSE);
				SetCatchLNP(BOTTOM_LINE, TRUE);
			}
			else
				pOOI->SetPosXY(Y_TOP, OOI_ty - distanceY);

			clkDistance.y = uy;
		}
		break;
	case BASE_ROI_BOTTOM:
		tempLinePos = OOI_by - distanceY;

		if (tempLinePos <= 0)
		{
			SetCatchLNP(BOTTOM_LINE, FALSE);
			//MessageBox(_T("관심영역을 화면 밖으로 보낼 수 없습니다."), _T("Regular Warning"), MB_ICONWARNING);
		}
		else
		{
			if (OOI_by - distanceY < OOI_ty + pOOI->GetMinSize())
			{
				break;
			}
			else if (OOI_by - distanceY < OOI_ty)
			{
				SetCatchLNP(BOTTOM_LINE, FALSE);
				SetCatchLNP(TOP_LINE, TRUE);
			}
			else
				pOOI->SetPosXY(Y_BOTTOM, OOI_by - distanceY);

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

bool CLineStretch::InvalidateRegion(RECT* CBaseRect, int lineCase, double static_factor, double dynamic_factor, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance)
{
	bool result = TRUE;
	int OOI_lx, OOI_rx, OOI_ty, OOI_by;
	double tmp1, tmp2;

	OOI_lx = pOOI->GetPosXY(X_LEFT);
	OOI_rx = pOOI->GetPosXY(X_RIGHT);
	OOI_ty = pOOI->GetPosXY(Y_TOP);
	OOI_by = pOOI->GetPosXY(Y_BOTTOM);

	OOI_lx += 1; OOI_rx += 1;
	OOI_ty += 1; OOI_by += 1;

	switch (lineCase)
	{
	case HORIZONTALITY:
		CBaseRect->left = m_XY.x * (1 - dynamic_factor);
		CBaseRect->top = OOI_ty * zoom * (1 - static_factor) + moveOffs_y;
		CBaseRect->right = m_XY.x * (1 + dynamic_factor);
		CBaseRect->bottom = OOI_by * zoom * (1 + static_factor) + moveOffs_y;
		break;
	case VERTICALITY:
		CBaseRect->left = OOI_lx * zoom * (1 - static_factor) + moveOffs_x;
		CBaseRect->top = m_XY.y * (1 - dynamic_factor);
		CBaseRect->right = OOI_rx * zoom * (1 + static_factor) + moveOffs_x;
		CBaseRect->bottom = m_XY.y * (1 + dynamic_factor);
		break;
	default:
		result = FALSE;
		break;
	}

	return result;
}

bool CLineStretch::CheckLine(int line_case, int gap, unsigned short ux, unsigned short uy)
{
	bool result = false;
	int OOI_lx, OOI_rx, OOI_ty, OOI_by;

	pOOI->InputXYVal(OOI_lx, OOI_rx, OOI_ty, OOI_by);

	switch (line_case)
	{
	case BASE_ROI_LEFT:
		if (OOI_ty - 2 < uy - 1 && OOI_by + gap > uy && OOI_lx - gap < ux && OOI_lx + gap > ux)
			result = true;
		break;
	case BASE_ROI_RIGHT:
		if (OOI_ty - 2 < uy - 1 && OOI_by + gap > uy && OOI_rx - gap < ux && OOI_rx + gap > ux)
			result = true;
		break;
	case BASE_ROI_TOP:
		if (OOI_lx - 2 < ux - 1 && OOI_rx + gap > ux && OOI_ty - gap < uy && OOI_ty + gap > uy)
			result = true;
		break;
	case BASE_ROI_BOTTOM:
		if (OOI_lx - 2 < ux - 1 && OOI_rx + gap > ux && OOI_by - gap < uy && OOI_by + gap > uy)
			result = true;
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


bool CPointStretch::InvalidateRegion(RECT* CBaseRect, int quadrant, double static_factor, double dynamic_factor, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance)
{
	return 0;
}

bool CPointStretch::CheckLine(int line_case, int gap, unsigned short ux, unsigned short uy)
{
	bool result = false;

	return result;
}


POINT CStretchNoway::Stretch(int caseNoway, POINT clkDistance, unsigned short ux, unsigned short uy) 
{
	POINT pt;
	return pt;
}

bool CStretchNoway::GetCatchLNP(int point_case) 
{
	return 0;
}

bool CStretchNoway::SetCatchLNP(int point_case, bool flag) 
{
	return 0;
}


bool CStretchNoway::InvalidateRegion(RECT* CBaseRect, int quadrant, double static_factor, double dynamic_factor, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance)
{
	return 0;
}

bool CStretchNoway::CheckLine(int line_case, int gap, unsigned short ux, unsigned short uy)
{
	bool result = false;

	return result;
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


bool CMoveNoway::Move(int lx_temp, int ty_temp, int rx_temp, int by_temp) {
	return 0;
}


////////////////////	Strategy Pattern	//////////////////////
