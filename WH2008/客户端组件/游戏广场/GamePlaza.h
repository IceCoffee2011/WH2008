#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "idispimp.h"
#include "Custsite.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//Ӧ�ó������
class CGamePlazaApp : public CWinApp
{
//	//��������
public:
//	CSkinRecordFile							m_SkinRecordFile;				//��������
	CImpIDispatch *							m_pDispOM;

	//��������
public:
	//���캯��
	CGamePlazaApp();
	~CGamePlazaApp();

	//���غ���
public:
	//��ʼ������
	virtual BOOL InitInstance();
	CString GetOnLineIP();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//Ӧ�ó������˵��
extern CGamePlazaApp theApp;

//////////////////////////////////////////////////////////////////////////
