#pragma once
// IOPCCallback_Imp.h: interface for the IOPCCallback_Imp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOPCCALLBACK_IMP_H__2BEE36DD_2487_4178_8E58_45EDD6AB99CF__INCLUDED_)
#define AFX_IOPCCALLBACK_IMP_H__2BEE36DD_2487_4178_8E58_45EDD6AB99CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "opcda.h"
#include "PyroSoftM.h"

class IOPCCallback_Imp : public IOPCDataCallback
{
public:
	IOPCCallback_Imp(CPyroSoftMApp*  pAppConnection, HWND hWndTraceView);
	virtual ~IOPCCallback_Imp();

	// the IUnknown Functions
	STDMETHODIMP         QueryInterface(REFIID iid, LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// Member Functions 
	virtual HRESULT STDMETHODCALLTYPE OnDataChange(
		/* [in] */ DWORD dwTransid,
		/* [in] */ OPCHANDLE hGroup,
		/* [in] */ HRESULT hrMasterquality,
		/* [in] */ HRESULT hrMastererror,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE __RPC_FAR *phClientItems,
		/* [size_is][in] */ VARIANT __RPC_FAR *pvValues,
		/* [size_is][in] */ WORD __RPC_FAR *pwQualities,
		/* [size_is][in] */ FILETIME __RPC_FAR *pftTimeStamps,
		/* [size_is][in] */ HRESULT __RPC_FAR *pErrors);

	virtual HRESULT STDMETHODCALLTYPE OnReadComplete(
		/* [in] */ DWORD dwTransid,
		/* [in] */ OPCHANDLE hGroup,
		/* [in] */ HRESULT hrMasterquality,
		/* [in] */ HRESULT hrMastererror,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE __RPC_FAR *phClientItems,
		/* [size_is][in] */ VARIANT __RPC_FAR *pvValues,
		/* [size_is][in] */ WORD __RPC_FAR *pwQualities,
		/* [size_is][in] */ FILETIME __RPC_FAR *pftTimeStamps,
		/* [size_is][in] */ HRESULT __RPC_FAR *pErrors);

	virtual HRESULT STDMETHODCALLTYPE OnWriteComplete(
		/* [in] */ DWORD dwTransid,
		/* [in] */ OPCHANDLE hGroup,
		/* [in] */ HRESULT hrMastererr,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE __RPC_FAR *pClienthandles,
		/* [size_is][in] */ HRESULT __RPC_FAR *pErrors);

	virtual HRESULT STDMETHODCALLTYPE OnCancelComplete(
		/* [in] */ DWORD dwTransid,
		/* [in] */ OPCHANDLE hGroup);

private:
	DWORD m_dwRefCount;
	CPyroSoftMApp  *m_pAppConnection;
	//  CAppTraceView      *m_pTraceView;
	HWND                m_hWndTraceView;
};

#endif // !defined(AFX_IOPCCALLBACK_IMP_H__2BEE36DD_2487_4178_8E58_45EDD6AB99CF__INCLUDED_)
