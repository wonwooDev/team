// MyPropertyGridProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PyroSoftM.h"
#include "CheckBoxPropertyGridProperty.h"


// CCheckBoxPropertyGridProperty

IMPLEMENT_DYNAMIC(CCheckBoxPropertyGridProperty, CMFCPropertyGridProperty)

CCheckBoxPropertyGridProperty::CCheckBoxPropertyGridProperty(const CString& strName, 
															bool bCheck, 
															const COleVariant &varValue,
															LPCTSTR lpszDescr, 
															DWORD_PTR dwData,
															LPCTSTR lpszEditMask, 
															LPCTSTR lpszEditTemplate, 
															LPCTSTR lpszValidChars)
:CMFCPropertyGridProperty(strName, (COleVariant) &varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
	m_bCheck = bCheck;

	m_rectCheck.SetRectEmpty();
}

CCheckBoxPropertyGridProperty::~CCheckBoxPropertyGridProperty()
{
}
/*
BOOL CCheckBoxPropertyGridProperty::HasButton() const
{
    return TRUE;
}
*/
void CCheckBoxPropertyGridProperty::OnDrawValue(CDC* pDC, CRect rect)
{
	m_rectCheck = rect;
	m_rectCheck.DeflateRect(1, 1);

	m_rectCheck.right = m_rectCheck.left + m_rectCheck.Height();

	OnDrawCheckBox(pDC, m_rectCheck, m_bCheck);
}

void CCheckBoxPropertyGridProperty::OnDrawCheckBox(CDC * pDC, CRect rect, bool bChecked)
{
	//COLORREF clrTextOld = pDC->GetTextColor();
	
	CMFCVisualManager::GetInstance()->OnDrawCheckBox(pDC, rect, FALSE, bChecked, m_bEnabled);

	//pDC->SetTextColor(clrTextOld);
}

BOOL CCheckBoxPropertyGridProperty::OnClickValue(UINT uiMsg, CPoint point)
{
	return CMFCPropertyGridProperty::OnClickValue(uiMsg, point);
}

BOOL CCheckBoxPropertyGridProperty::OnEdit(LPPOINT lptClick) 
{ 
	if(lptClick==NULL) return FALSE;
	
	if(m_bEnabled && m_rectCheck.PtInRect(*lptClick)) {

		m_bCheck = !m_bCheck;
				
		m_pWndList->InvalidateRect(m_rectCheck);		
				
		::PostMessage(AfxGetMainWnd()->m_hWnd, AFX_WM_PROPERTY_CHANGED, (WPARAM) 0, (LPARAM) this);

	}
	return TRUE; 
}

BOOL CCheckBoxPropertyGridProperty::OnDblClk(CPoint point)
{
	if (m_bEnabled && m_rectCheck.PtInRect(point))
	{
		return TRUE;
	}

	m_bCheck = !m_bCheck;

	m_pWndList->InvalidateRect(m_rectCheck);	

	::PostMessage(AfxGetMainWnd()->m_hWnd, AFX_WM_PROPERTY_CHANGED, (WPARAM) 0, (LPARAM) this);

	return TRUE;
}

BOOL CCheckBoxPropertyGridProperty::PushChar(UINT nChar)
{
	if (nChar == VK_SPACE)
	{
		OnDblClk(CPoint(-1, -1));
	}

	return TRUE;
}

void CCheckBoxPropertyGridProperty::SetValue(bool check)
{	
	m_bCheck = check;
	
	m_pWndList->InvalidateRect(m_rectCheck);	

	::PostMessage(AfxGetMainWnd()->m_hWnd, AFX_WM_PROPERTY_CHANGED, (WPARAM) 0, (LPARAM) this);
}