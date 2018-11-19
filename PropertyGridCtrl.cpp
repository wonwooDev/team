// PropertyGridCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PyroSoftM.h"
#include "PropertyGridCtrl.h"


// CPropertyGridCtrl

IMPLEMENT_DYNAMIC(CPropertyGridCtrl, CMFCPropertyGridCtrl)

CPropertyGridCtrl::CPropertyGridCtrl()
{

}

CPropertyGridCtrl::~CPropertyGridCtrl()
{
}


BEGIN_MESSAGE_MAP(CPropertyGridCtrl, CMFCPropertyGridCtrl)
END_MESSAGE_MAP()



// CPropertyGridCtrl 메시지 처리기입니다.
void CPropertyGridCtrl::SetLeftColumnWidth(int cx)
{
	m_nLeftColumnWidth = cx;
    AdjustLayout();
}

void CPropertyGridCtrl::AdjustLayout()
{
	//m_nLeftColumnWidth= 200;

	return CMFCPropertyGridCtrl::AdjustLayout();
}
