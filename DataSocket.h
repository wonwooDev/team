#pragma once

class CPyroSoftMApp;

// CDataSocket 명령 대상입니다.

class CDataSocket : public CSocket
{
public:
	CPyroSoftMApp* m_pApp;

	CDataSocket();
	CDataSocket(CPyroSoftMApp* pApp);

	virtual ~CDataSocket();
};


