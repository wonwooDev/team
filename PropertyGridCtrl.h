#pragma once


// CPropertyGridCtrl

class CPropertyGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyGridCtrl)

public:
	CPropertyGridCtrl();
	virtual ~CPropertyGridCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void SetLeftColumnWidth(int cx);
	virtual void AdjustLayout();
};


