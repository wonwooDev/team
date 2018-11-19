////////////////////////////////////////////////////////////////////////////
/** @file DlgAddNewDevice.cpp implementation of the CDlgAddNewDevice class
*/
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PyroSoftM.h"
#include "DlgAddNewDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgAddNewDevice 


CDlgAddNewDevice::CDlgAddNewDevice(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddNewDevice::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddNewDevice)
	m_SelDevice = -1;
	m_SelInterface = -1;
	m_TemplateFile = _T("");
	m_UseTemplate = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgAddNewDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddNewDevice)
	DDX_CBIndex(pDX, IDC_COMBO_DEVICES, m_SelDevice);
	DDX_CBIndex(pDX, IDC_COMBO_INTERFACE, m_SelInterface);
	DDX_Text(pDX, IDC_EDIT_TEMPL, m_TemplateFile);
	DDX_Check(pDX, IDC_CHECK_TEMPL, m_UseTemplate);
	//}}AFX_DATA_MAP
	if (!pDX->m_bSaveAndValidate)
		GetDlgItem(IDC_BUTTON_TEMPL)->EnableWindow(m_UseTemplate);
}


BEGIN_MESSAGE_MAP(CDlgAddNewDevice, CDialog)
	//{{AFX_MSG_MAP(CDlgAddNewDevice)
	ON_BN_CLICKED(IDC_BUTTON_TEMPL, OnButtonTempl)
	ON_BN_CLICKED(IDC_CHECK_TEMPL, OnCheckTempl)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICES, OnSelchangeComboDevices)
	ON_CBN_SELCHANGE(IDC_COMBO_INTERFACE, OnSelchangeComboInterface)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CDlgAddNewDevice::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_COMBO_DEVICES);
	for (int i=0; i<m_DeviceStrings.GetSize(); i++)
		pBox->AddString(m_DeviceStrings.GetAt(i));
	pBox->SetCurSel(0);
	
	pBox = (CComboBox*) GetDlgItem(IDC_COMBO_INTERFACE);
	for (int i=0; i<m_InterfaceStrings.GetSize(); i++)
		pBox->AddString(m_InterfaceStrings.GetAt(i));
	pBox->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur?kgeben
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAddNewDevice::OnButtonTempl() 
{
	// File open dialog for template file
	CFileDialog	dlg(TRUE, ".irdx", m_TemplateFile);

	CString title;
	VERIFY(title.LoadString(IDS_OPEN_TEMPLFILE));

	CString strFilter;
	CString strExt;

	strExt.LoadString(IDS_IRDX_FILES);
	strExt += (TCHAR)'\0';	
	strExt += _T("*.irdx");
	strExt += (TCHAR)'\0';	
	strFilter += strExt;
	dlg.m_ofn.nMaxCustFilter += 1;
	dlg.m_ofn.lpstrTitle	= title;
	dlg.m_ofn.lpstrFilter	= strFilter;

	if (dlg.DoModal() != IDOK)
		return;

	m_TemplateFile = dlg.GetPathName();
	UpdateData(FALSE);	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAddNewDevice::OnCheckTempl() 
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAddNewDevice::OnSelchangeComboDevices() 
{
	UpdateData(TRUE);
	m_SelInterface = m_SelDevice;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAddNewDevice::OnSelchangeComboInterface() 
{
	UpdateData(TRUE);
	m_SelDevice = m_SelInterface;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////

