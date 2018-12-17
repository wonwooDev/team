#pragma once

#define	X_LEFT					0
#define	X_RIGHT					1
#define	Y_BOTTOM				2
#define Y_TOP					3

#define LEFT_LINE				1
#define RIGHT_LINE				2
#define TOP_LINE				3
#define BOTTOM_LINE				4
#define HORIZONTALITY			5
#define VERTICALITY				6

////////////////////	Strategy Pattern	//////////////////////
class COOI;

//	Move class
class CMove
{
public:
	COOI * pOOI;
	
	virtual bool Move(int lx_temp, int ty_temp, int rx_temp, int by_temp) = 0;
};

class CRectMove : public CMove
{
protected:

public:
	CRectMove(COOI* pOOI);

	virtual bool Move(int lx_temp, int ty_temp, int rx_temp, int by_temp);
};

class CLineMove : public CMove
{
protected:

public:
	CLineMove(COOI* pOOI);

	virtual bool Move(int lx_temp, int ty_temp, int rx_temp, int by_temp);
};

class CPointMove : public CMove
{
protected:

public:
	CPointMove(COOI* pOOI);

	virtual bool Move(int lx_temp, int ty_temp, int rx_temp, int by_temp);
};

class CMoveNoway : public CMove
{
protected:

public:
	CMoveNoway(COOI* pOOI);

	virtual bool Move(int lx_temp, int ty_temp, int rx_temp, int by_temp);
};


//	Stretch class
class CStretch
{
public:
	COOI * pOOI;

	virtual POINT Stretch(int line_case, POINT clkDistance, unsigned short ux, unsigned short uy) = 0;
	virtual bool GetCatchLNP(int LNP_case) = 0;
	virtual bool SetCatchLNP(int LNP_case, bool flag) = 0;
	virtual bool InvalidateRegion(RECT* CBaseRect, int quadrant, double static_factor, double dynamic_factor, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance) = 0;
	virtual bool CheckLine(int line_case, int gap, unsigned short ux, unsigned short uy) = 0;
};

class CLineStretch : public CStretch
{
protected:
	bool m_bCatch_L_line, m_bCatch_R_line, m_bCatch_T_line, m_bCatch_B_line;

public:
	CLineStretch(COOI* pOOI);

	virtual POINT Stretch(int line_case, POINT clkDistance, unsigned short ux, unsigned short uy);
	virtual bool GetCatchLNP(int line_case);
	virtual bool SetCatchLNP(int line_case, bool flag);
	virtual bool InvalidateRegion(RECT* CBaseRect, int quadrant, double static_factor, double dynamic_factor, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance);
	virtual bool CheckLine(int line_case, int gap, unsigned short ux, unsigned short uy);
};

class CPointStretch : public CStretch
{
protected:
	bool m_bCatch_first_point, m_bCatch_second_point;

public:
	CPointStretch(COOI* pOOI);

	virtual POINT Stretch(int point_case, POINT clkDistance, unsigned short ux, unsigned short uy);
	virtual bool GetCatchLNP(int point_case);
	virtual bool SetCatchLNP(int point_case, bool flag);
	virtual bool InvalidateRegion(RECT* CBaseRect, int quadrant, double static_factor, double dynamic_factor, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance);
	virtual bool CheckLine(int line_case, int gap, unsigned short ux, unsigned short uy);
};

class CStretchNoway : public CStretch
{
protected:

public:
	CStretchNoway(COOI* pOOI);

	virtual POINT Stretch(int caseNoway, POINT clkDistance, unsigned short ux, unsigned short uy);
	virtual bool GetCatchLNP(int caseNoway);
	virtual bool SetCatchLNP(int caseNoway, bool flag);
	virtual bool InvalidateRegion(RECT* CBaseRect, int quadrant, double static_factor, double dynamic_factor, CPoint m_XY, int moveOffs_x, int moveOffs_y, float zoom, POINT ClickedDistance);
	virtual bool CheckLine(int line_case, int gap, unsigned short ux, unsigned short uy);
};


class COOI {
protected:
	bool bSide;
	bool bDrawDone;
	bool bOppDrawDone;
	bool bDrawFirst;
	bool bOppDrawFirst;
	bool bIsInside;

	int lx, rx, ty, by;
	int opp_lx, opp_rx, opp_ty, opp_by;
	int width, height;
	int minSize;
	int count;
	int opp_count;

public:
	//Character(Weapon* w);					// 생성자
	COOI();
	~COOI();

	CMove*			move;					// 움직임
	CStretch*		stretch;				// 늘이기

	virtual void	Display() = 0;				// 자식 클래스에서 구현 필수

	virtual bool	GetDirection();
	virtual void	SetDirection(bool side);

	virtual bool	GetDrawDone();
	virtual	void	SetDrawDone(bool bDrawFlg);

	virtual bool	GetOppDrawDone();
	virtual	void	SetOppDrawDone(bool bDrawFlg);

	virtual bool	GetDrawFirst();
	virtual void	SetDrawFirst(bool bDrawFirstFlg);

	virtual bool	GetOppDrawFirst();
	virtual void	SetOppDrawFirst(bool bDrawFirstFlg);

	virtual int		GetOppPosXY(int XY_pos);
	virtual bool	SetOppPosXY(int XY_pos, int val);

	virtual int		GetCount();
	virtual bool	SetCount(int cnt);

	virtual int		GetOppCount();
	virtual bool	SetOppCount(int cnt);

	virtual int		GetPosXY(int XY_pos);
	virtual	bool	SetPosXY(int XY_pos, int val);

	virtual int		GetWidth();
	virtual int		GetHeight();

	virtual int		GetMinSize();
	virtual	bool	SetMinSize(int val);

	virtual bool	GetInsideFlag();
	virtual void	SetInsideFlag(bool flag);

	virtual bool	SelectObj(RECT* CBaseRect, unsigned short ux, unsigned short uy, CPoint m_XY, int offs_x, int offs_y, float m_bmp_zoom, POINT ClickedDistance);
	virtual bool	SetInvaliRect(RECT* CBaseRect, int quadrant, double static_factor, double dynamic_factor, CPoint m_XY, int offs_x, int offs_y, float m_bmp_zoom, POINT ClickedDistance);
	virtual	void	InitXYPos();
	virtual void	InputXYVal(int& lx, int& rx, int& ty, int& by);
	virtual bool	IsInsideOOI(unsigned short ux, unsigned short uy);

};

class CBROI : public COOI
{
protected:

public:
	CBROI();

	void Display();
};

class CTROI : public COOI
{
protected:

public:
	CTROI();

	void Display();
};

class CPROI : public COOI
{
protected:

public:
	CPROI();

	void Display();
};

class CLROI : public COOI
{
protected:

public:
	CLROI();

	void Display();
};

class CEROI : public COOI
{
protected:

public:
	CEROI();

	void Display();
};

////////////////////	Strategy Pattern	//////////////////////