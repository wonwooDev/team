// RefDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PyroSoftM.h"
#include "ZoneInfDlg.h"
#include "afxdialogex.h"


// CZoneInfDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CZoneInfDlg, CDialogEx)

CZoneInfDlg::CZoneInfDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REF_INF, pParent)
{

}

CZoneInfDlg::~CZoneInfDlg()
{
}

void CZoneInfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZONE1_DISTANCE, m_Zone_Distance[0]);
	DDX_Control(pDX, IDC_ZONE2_DISTANCE, m_Zone_Distance[1]);
	DDX_Control(pDX, IDC_ZONE3_DISTANCE, m_Zone_Distance[2]);
	DDX_Control(pDX, IDC_ZONE4_DISTANCE, m_Zone_Distance[3]);
	DDX_Control(pDX, IDC_ZONE5_DISTANCE, m_Zone_Distance[4]);
	DDX_Control(pDX, IDC_ZONE6_DISTANCE, m_Zone_Distance[5]);
	DDX_Control(pDX, IDC_ZONE7_DISTANCE, m_Zone_Distance[6]);
	DDX_Control(pDX, IDC_ZONE8_DISTANCE, m_Zone_Distance[7]);
	DDX_Control(pDX, IDC_ZONE9_DISTANCE, m_Zone_Distance[8]);
	DDX_Control(pDX, IDC_ZONE10_DISTANCE, m_Zone_Distance[9]);
	DDX_Control(pDX, IDC_ZONE11_DISTANCE, m_Zone_Distance[10]);
	DDX_Control(pDX, IDC_ZONE12_DISTANCE, m_Zone_Distance[11]);
	DDX_Control(pDX, IDC_ZONE13_DISTANCE, m_Zone_Distance[12]);
	DDX_Control(pDX, IDC_ZONE14_DISTANCE, m_Zone_Distance[13]);
	DDX_Control(pDX, IDC_ZONE15_DISTANCE, m_Zone_Distance[14]);
	DDX_Control(pDX, IDC_ZONE16_DISTANCE, m_Zone_Distance[15]);
	DDX_Control(pDX, IDC_ZONE17_DISTANCE, m_Zone_Distance[16]);
	DDX_Control(pDX, IDC_ZONE18_DISTANCE, m_Zone_Distance[17]);
	DDX_Control(pDX, IDC_ZONE19_DISTANCE, m_Zone_Distance[18]);
	DDX_Control(pDX, IDC_ZONE20_DISTANCE, m_Zone_Distance[19]);
	DDX_Control(pDX, IDC_ZONE21_DISTANCE, m_Zone_Distance[20]);
	DDX_Control(pDX, IDC_ZONE22_DISTANCE, m_Zone_Distance[21]);
	DDX_Control(pDX, IDC_ZONE23_DISTANCE, m_Zone_Distance[22]);
	DDX_Control(pDX, IDC_ZONE24_DISTANCE, m_Zone_Distance[23]);
	DDX_Control(pDX, IDC_ZONE25_DISTANCE, m_Zone_Distance[24]);
	DDX_Control(pDX, IDC_ZONE26_DISTANCE, m_Zone_Distance[25]);
	DDX_Control(pDX, IDC_ZONE27_DISTANCE, m_Zone_Distance[26]);
	DDX_Control(pDX, IDC_ZONE28_DISTANCE, m_Zone_Distance[27]);
	DDX_Control(pDX, IDC_ZONE29_DISTANCE, m_Zone_Distance[28]);
	DDX_Control(pDX, IDC_ZONE30_DISTANCE, m_Zone_Distance[29]);
	DDX_Control(pDX, IDC_ZONE31_DISTANCE, m_Zone_Distance[30]);
	DDX_Control(pDX, IDC_ZONE32_DISTANCE, m_Zone_Distance[31]);

	DDX_Control(pDX, IDC_ZONE1_SPEED, m_Zone_Speed[0]);
	DDX_Control(pDX, IDC_ZONE2_SPEED, m_Zone_Speed[1]);
	DDX_Control(pDX, IDC_ZONE3_SPEED, m_Zone_Speed[2]);
	DDX_Control(pDX, IDC_ZONE4_SPEED, m_Zone_Speed[3]);
	DDX_Control(pDX, IDC_ZONE5_SPEED, m_Zone_Speed[4]);
	DDX_Control(pDX, IDC_ZONE6_SPEED, m_Zone_Speed[5]);
	DDX_Control(pDX, IDC_ZONE7_SPEED, m_Zone_Speed[6]);
	DDX_Control(pDX, IDC_ZONE8_SPEED, m_Zone_Speed[7]);
	DDX_Control(pDX, IDC_ZONE9_SPEED, m_Zone_Speed[8]);
	DDX_Control(pDX, IDC_ZONE10_SPEED, m_Zone_Speed[9]);
	DDX_Control(pDX, IDC_ZONE11_SPEED, m_Zone_Speed[10]);
	DDX_Control(pDX, IDC_ZONE12_SPEED, m_Zone_Speed[11]);
	DDX_Control(pDX, IDC_ZONE13_SPEED, m_Zone_Speed[12]);
	DDX_Control(pDX, IDC_ZONE14_SPEED, m_Zone_Speed[13]);
	DDX_Control(pDX, IDC_ZONE15_SPEED, m_Zone_Speed[14]);
	DDX_Control(pDX, IDC_ZONE16_SPEED, m_Zone_Speed[15]);
	DDX_Control(pDX, IDC_ZONE17_SPEED, m_Zone_Speed[16]);
	DDX_Control(pDX, IDC_ZONE18_SPEED, m_Zone_Speed[17]);
	DDX_Control(pDX, IDC_ZONE19_SPEED, m_Zone_Speed[18]);
	DDX_Control(pDX, IDC_ZONE20_SPEED, m_Zone_Speed[19]);
	DDX_Control(pDX, IDC_ZONE21_SPEED, m_Zone_Speed[20]);
	DDX_Control(pDX, IDC_ZONE22_SPEED, m_Zone_Speed[21]);
	DDX_Control(pDX, IDC_ZONE23_SPEED, m_Zone_Speed[22]);
	DDX_Control(pDX, IDC_ZONE24_SPEED, m_Zone_Speed[23]);
	DDX_Control(pDX, IDC_ZONE25_SPEED, m_Zone_Speed[24]);
	DDX_Control(pDX, IDC_ZONE26_SPEED, m_Zone_Speed[25]);
	DDX_Control(pDX, IDC_ZONE27_SPEED, m_Zone_Speed[26]);
	DDX_Control(pDX, IDC_ZONE28_SPEED, m_Zone_Speed[27]);
	DDX_Control(pDX, IDC_ZONE29_SPEED, m_Zone_Speed[28]);
	DDX_Control(pDX, IDC_ZONE30_SPEED, m_Zone_Speed[29]);
	DDX_Control(pDX, IDC_ZONE31_SPEED, m_Zone_Speed[30]);
	DDX_Control(pDX, IDC_ZONE32_SPEED, m_Zone_Speed[31]);

	DDX_Control(pDX, IDC_ZONE1_TEMP, m_Zone_Temp[0]);
	DDX_Control(pDX, IDC_ZONE2_TEMP, m_Zone_Temp[1]);
	DDX_Control(pDX, IDC_ZONE3_TEMP, m_Zone_Temp[2]);
	DDX_Control(pDX, IDC_ZONE4_TEMP, m_Zone_Temp[3]);
	DDX_Control(pDX, IDC_ZONE5_TEMP, m_Zone_Temp[4]);
	DDX_Control(pDX, IDC_ZONE6_TEMP, m_Zone_Temp[5]);
	DDX_Control(pDX, IDC_ZONE7_TEMP, m_Zone_Temp[6]);
	DDX_Control(pDX, IDC_ZONE8_TEMP, m_Zone_Temp[7]);
	DDX_Control(pDX, IDC_ZONE9_TEMP, m_Zone_Temp[8]);
	DDX_Control(pDX, IDC_ZONE10_TEMP, m_Zone_Temp[9]);
	DDX_Control(pDX, IDC_ZONE11_TEMP, m_Zone_Temp[10]);
	DDX_Control(pDX, IDC_ZONE12_TEMP, m_Zone_Temp[11]);
	DDX_Control(pDX, IDC_ZONE13_TEMP, m_Zone_Temp[12]);
	DDX_Control(pDX, IDC_ZONE14_TEMP, m_Zone_Temp[13]);
	DDX_Control(pDX, IDC_ZONE15_TEMP, m_Zone_Temp[14]);
	DDX_Control(pDX, IDC_ZONE16_TEMP, m_Zone_Temp[15]);
	DDX_Control(pDX, IDC_ZONE17_TEMP, m_Zone_Temp[16]);
	DDX_Control(pDX, IDC_ZONE18_TEMP, m_Zone_Temp[17]);
	DDX_Control(pDX, IDC_ZONE19_TEMP, m_Zone_Temp[18]);
	DDX_Control(pDX, IDC_ZONE20_TEMP, m_Zone_Temp[19]);
	DDX_Control(pDX, IDC_ZONE21_TEMP, m_Zone_Temp[20]);
	DDX_Control(pDX, IDC_ZONE22_TEMP, m_Zone_Temp[21]);
	DDX_Control(pDX, IDC_ZONE23_TEMP, m_Zone_Temp[22]);
	DDX_Control(pDX, IDC_ZONE24_TEMP, m_Zone_Temp[23]);
	DDX_Control(pDX, IDC_ZONE25_TEMP, m_Zone_Temp[24]);
	DDX_Control(pDX, IDC_ZONE26_TEMP, m_Zone_Temp[25]);
	DDX_Control(pDX, IDC_ZONE27_TEMP, m_Zone_Temp[26]);
	DDX_Control(pDX, IDC_ZONE28_TEMP, m_Zone_Temp[27]);
	DDX_Control(pDX, IDC_ZONE29_TEMP, m_Zone_Temp[28]);
	DDX_Control(pDX, IDC_ZONE30_TEMP, m_Zone_Temp[29]);
	DDX_Control(pDX, IDC_ZONE31_TEMP, m_Zone_Temp[30]);
	DDX_Control(pDX, IDC_ZONE32_TEMP, m_Zone_Temp[31]);

	DDX_Control(pDX, IDC_START_POS, m_Start_Position);
	DDX_Control(pDX, IDC_END_POS, m_End_Position);
}


BEGIN_MESSAGE_MAP(CZoneInfDlg, CDialogEx)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_APPLY_ZONE, &CZoneInfDlg::OnBnClickedApplyZone)
	ON_BN_CLICKED(IDC_LOAD_RECIPE1, &CZoneInfDlg::OnBnClickedLoadRecipe)
	ON_BN_CLICKED(IDC_SAVE_ZONE, &CZoneInfDlg::OnBnClickedSaveZone)
	ON_BN_CLICKED(IDC_CLOSE_ZONE, &CZoneInfDlg::OnBnClickedCloseZone)
END_MESSAGE_MAP()


// CZoneInfDlg 메시지 처리기입니다.

BOOL CZoneInfDlg::PreTranslateMessage(MSG* pMsg)
{
	//엔터키를 누르면 UPdate할 수 있도록 
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
		return FALSE;
	}
	else
		return CDialogEx::PreTranslateMessage(pMsg);
}



int CZoneInfDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_EmptyStringFlag = false;
	
	return 0;
}

void CZoneInfDlg::InitRefDlg()
{
}

/////////////////////////////////////////////////////////////////////////////

void CZoneInfDlg::PrintZoneInf()
{
	CString str;

	for (int i = 0; i < pDoc->m_NumOfZone; i++)
	{
		str.Format(_T("%d"), pDoc->m_ZoneDistance[i]);
		m_Zone_Distance[i].SetWindowText(str);

		str.Format(_T("%.1f"), pDoc->m_ZoneSpeed[i]);
		m_Zone_Speed[i].SetWindowText(str);

		str.Format(_T("%.1f"), pDoc->m_ZoneTemp[i]);
		m_Zone_Temp[i].SetWindowText(str);
	}

	str.Format(_T("%d"), pDoc->m_StartingPos);
	m_Start_Position.SetWindowText(str);

	str.Format(_T("%d"), pDoc->m_EndingPos);
	m_End_Position.SetWindowText(str);
}

void CZoneInfDlg::OnBnClickedApplyZone()
{
	CString str;
	int check_val = 0;
	int tmp_StartingPos, tmp_EndingPos;
	float tmp_ZoneDist[MAX_ZONE];
	float tmp_ZoneSpeed[MAX_ZONE];
	float tmp_ZoneTemp[MAX_ZONE];

	for (int i = 0; i < pDoc->m_NumOfZone; i++)
	{
		m_Zone_Distance[i].GetWindowTextA(str);
		tmp_ZoneDist[i] = pDoc->m_ZoneDistance[i];
		pDoc->m_ZoneDistance[i] = _ttoi(str);
		CheckEmptyValue(str);

		m_Zone_Speed[i].GetWindowTextA(str);
		tmp_ZoneSpeed[i] = pDoc->m_ZoneSpeed[i];
		pDoc->m_ZoneSpeed[i] = _ttof(str);
		CheckEmptyValue(str);

		m_Zone_Temp[i].GetWindowTextA(str);
		tmp_ZoneTemp[i] = pDoc->m_ZoneTemp[i];
		pDoc->m_ZoneTemp[i] = _ttof(str);
		CheckEmptyValue(str);
	}

	m_Start_Position.GetWindowTextA(str);
	tmp_StartingPos = pDoc->m_StartingPos;
	pDoc->m_StartingPos = _ttoi(str);
	CheckEmptyValue(str);

	m_End_Position.GetWindowTextA(str);
	tmp_EndingPos = pDoc->m_EndingPos;
	pDoc->m_EndingPos = _ttoi(str);
	CheckEmptyValue(str);

	if (m_EmptyStringFlag)
	{
		for (int i = 0; i < pDoc->m_NumOfZone; i++)		// 이전 값으로 적용
		{
			pDoc->m_ZoneDistance[i] = tmp_ZoneDist[i];
			pDoc->m_ZoneSpeed[i] = tmp_ZoneSpeed[i];
			pDoc->m_ZoneTemp[i] = tmp_ZoneTemp[i];
			pDoc->m_StartingPos = tmp_StartingPos;
			pDoc->m_EndingPos = tmp_EndingPos;
		}
		AfxMessageBox("존재하지 않는 값이 존재합니다.");	// -승태임무
		return;
	}
}

void CZoneInfDlg::CheckEmptyValue(CString str)
{
	if (str == "")
		m_EmptyStringFlag = true;
}


void CZoneInfDlg::OnBnClickedLoadRecipe()
{
	int		lastD_check = 0;
	float	lastS_check = 0;
	float	lastT_check = 0;
	char	dummy[128];

	FILE *stream;
	CFileDialog fDlg(TRUE, _T("*.ini"),NULL, OFN_FILEMUSTEXIST, _T("INI Files(*.ini)|*.ini|"), NULL);

	//OnBnClickedApplyZone();
	
	if (fDlg.DoModal() == IDOK)
	{
		CString ConfigFileName = fDlg.GetPathName();
		stream = fopen((CStringA)ConfigFileName, "rt");

		// [Recipe Setting]
		fgets(dummy, 127, stream);
		fscanf(stream, "NumberOfRecipe=%d\n", &(pDoc->m_NumOfZone));
		fscanf(stream, "RowNum=%d\n", &(pDoc->m_RowNum));
		fscanf(stream, "ColNum=%d\n\n", &(pDoc->m_ColNum));

		// [Recipe Information]
		fgets(dummy, 127, stream);
		fscanf(stream, "StartingPoisition=%d\n", &(pDoc->m_StartingPos));
		//pDoc->m_StartingPos--;
		fscanf(stream, "EndingPosition=%d\n\n", &(pDoc->m_EndingPos));

		for (int j = 0; j < pDoc->m_RowNum; j++)
		{
			fgets(dummy, 127, stream);
			fscanf(stream, "거리(mm)\t");
			for (int i = 0; i<pDoc->m_ColNum; i++)
			{
				fscanf(stream, "%d\t", &(pDoc->m_ZoneDistance[j*(pDoc->m_ColNum) + i]));
			}
			(stream, "\n");
			fscanf(stream, "속도(mm/Min)\t");
			for (int i = 0; i<pDoc->m_ColNum; i++)
			{
				fscanf(stream, "%f\t", &(pDoc->m_ZoneSpeed[j*(pDoc->m_ColNum) + i]));
			}
			(stream, "\n");
			fscanf(stream, "온도(℃)\t");
			for (int i = 0; i<pDoc->m_ColNum; i++)
			{
				fscanf(stream, "%f\t", &(pDoc->m_ZoneTemp[j*(pDoc->m_ColNum) + i]));
			}
			(stream, "\n\n");
		}
		lastD_check = pDoc->m_ZoneDistance[pDoc->m_NumOfZone - 1];
		lastS_check = pDoc->m_ZoneSpeed[pDoc->m_NumOfZone - 1];
		lastT_check = pDoc->m_ZoneTemp[pDoc->m_NumOfZone - 1];

		fclose(stream);
	}

	//load할 시 들어간 값에 대하여 검사하도록 하고 유효값이 아닐 시 적용하지 아니한다. (혹은 0의 값으로 초기화 한다.)
	// 조건이 만족할 경우 Apply 해준다.
	if (pDoc->m_EndingPos >= 0 && pDoc->m_EndingPos <= 100000 && lastS_check >= 0 && lastS_check <= 100000 && lastD_check >= 0 && lastD_check <= 100000 &&
		lastT_check >= 0 && lastT_check <= 100000 && pDoc->m_StartingPos <= pDoc->m_EndingPos)
	{
		// PrintZoneInf와 OnBnClickedApplyZone순서 생각해 볼 것

		OnBnClickedApplyZone();
		PrintZoneInf();
		m_EmptyStringFlag = false;
	}
	else
	{
		ZeroInit();
		MessageBox(_T(" (*.ini) 파일 내용이 형식에 맞지 않거나 예상치 못한 값이 들어왔습니다."), _T("Regular Warning"), MB_ICONWARNING);
	}
}

void CZoneInfDlg::ZeroInit()
{
	float tmpColNum = 0;

	pDoc->m_NumOfZone = 0;
	pDoc->m_RowNum = 0;
	pDoc->m_ColNum = 0;

	pDoc->m_StartingPos = 0;
	pDoc->m_EndingPos = 0;

	tmpColNum = pDoc->m_ColNum;
	for (int j = 0; j < pDoc->m_RowNum; j++)
	{
		if (j >= pDoc->m_RowNum - 1)
		{
			tmpColNum = pDoc->m_NumOfZone % pDoc->m_ColNum;
		}
		for (int i = 0; i<tmpColNum; i++)
		{
			pDoc->m_ZoneDistance[j*(pDoc->m_ColNum) + i] = 0;
			pDoc->m_ZoneSpeed[j*(pDoc->m_ColNum) + i] = 0;
		}
	}
}


void CZoneInfDlg::OnBnClickedSaveZone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int	tmpColNum = 0;
	CStdioFile file;
	CFileException cEx;

	OnBnClickedApplyZone();
	if (!m_EmptyStringFlag)
	{
		CFileDialog fDlg(FALSE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT, _T("TXT Files(*.ini)|*ini|"), NULL);

		m_EmptyStringFlag = false;

		if (fDlg.DoModal() == IDOK)
		{
			CString ConfigFileName = fDlg.GetPathName();
			CString tmpStr;

			if (ConfigFileName.Right(4) != ".ini")
				ConfigFileName += ".ini";

			file.Open(ConfigFileName, CFile::modeCreate | CFile::modeReadWrite, &cEx);

			file.WriteString("[Recipe Setting]\n");
			tmpStr.Format("NumberOfRecipe=%d\n", pDoc->m_NumOfZone);
			file.WriteString(tmpStr);
			tmpStr.Format("RowNum=%d\n", pDoc->m_RowNum);
			file.WriteString(tmpStr);
			tmpStr.Format("ColNum=%d\n\n", pDoc->m_ColNum);
			file.WriteString(tmpStr);

			file.WriteString("[Recipe Information]\n");
			tmpStr.Format("StartingPoisition=%d\n", pDoc->m_StartingPos);
			file.WriteString(tmpStr);
			tmpStr.Format("EndingPosition=%d\n\n", pDoc->m_EndingPos);
			file.WriteString(tmpStr);

			tmpColNum = pDoc->m_ColNum;
			for (int j = 0; j < pDoc->m_RowNum; j++)
			{
				if (j >= pDoc->m_RowNum - 1)
				{
					tmpColNum = pDoc->m_NumOfZone % pDoc->m_ColNum;
				}
				file.WriteString("\t");
				for (int i = 0; i<tmpColNum; i++)
				{
					tmpStr.Format("%d", (j*pDoc->m_ColNum + i + 1));
					file.WriteString("\tZone" + tmpStr);
				}
				file.WriteString("\n");

				file.WriteString("거리(mm)\t");
				for (int i = 0; i<tmpColNum; i++)
				{
					tmpStr.Format("%d\t", pDoc->m_ZoneDistance[j*(pDoc->m_ColNum) + i]);
					file.WriteString(tmpStr);
				}
				file.WriteString("\n");

				file.WriteString("속도(mm/Min)\t");
				for (int i = 0; i<tmpColNum; i++)
				{
					tmpStr.Format("%.1f\t", pDoc->m_ZoneSpeed[j*(pDoc->m_ColNum) + i]);
					file.WriteString(tmpStr);
				}
				file.WriteString("\n");

				file.WriteString("온도(℃)\t");
				for (int i = 0; i<tmpColNum; i++)
				{
					tmpStr.Format("%.1f\t", pDoc->m_ZoneTemp[j*(pDoc->m_ColNum) + i]);
					file.WriteString(tmpStr);
				}
				file.WriteString("\n\n");
			}

			file.Close();
		}
	}
}


void CZoneInfDlg::OnBnClickedCloseZone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
}



BOOL CZoneInfDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	PrintZoneInf();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
