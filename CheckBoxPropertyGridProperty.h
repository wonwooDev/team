#pragma once


// CCheckBoxPropertyGridProperty

class CCheckBoxPropertyGridProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CCheckBoxPropertyGridProperty)

public:
	CCheckBoxPropertyGridProperty(const CString& strName, 
										bool bCheck, 
										const COleVariant &varValue,
										LPCTSTR lpszDescr = NULL, 
										DWORD_PTR dwData = 0,
										LPCTSTR lpszEditMask = NULL, 
										LPCTSTR lpszEditTemplate = NULL, 
										LPCTSTR lpszValidChars = NULL);

	~CCheckBoxPropertyGridProperty();

	virtual BOOL HasButton() const { return FALSE; }
	virtual void OnDrawValue(CDC* pDC, CRect rect);
	virtual BOOL OnClickValue(UINT uiMsg, CPoint point);
	
	virtual void OnDrawCheckBox(CDC * pDC, CRect rectCheck, bool bChecked);

	virtual BOOL OnDblClk(CPoint point);

	virtual BOOL OnSetCursor() const  { return FALSE; }

	virtual BOOL OnEdit(LPPOINT lptClick);

	virtual BOOL PushChar(UINT nChar);
		
	bool GetValue() {return m_bCheck;}
	void SetValue(bool check);


protected:
	bool	m_bCheck;
	CRect	m_rectCheck;
};


