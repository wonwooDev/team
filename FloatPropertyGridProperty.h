#pragma once


// CFLoatPropertyGridProperty

class CFLoatPropertyGridProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CFLoatPropertyGridProperty)

public:
	CFLoatPropertyGridProperty(const CString& strName, const COleVariant& varValue);
	~CFLoatPropertyGridProperty();

	virtual CString FormatProperty() 
	{         
		CString str;         
		if (GetValue().vt == VT_R4) {     

			COleVariant rVariant = GetValue();	
			VARIANT var = rVariant.Detach();				
			float fVal = var.fltVal;

			str.Format("%.2f", fVal); // Or whatever you want
		}         
		else {             
			str = CMFCPropertyGridProperty::FormatProperty();         
		}           
		return str;     
	} 
};


