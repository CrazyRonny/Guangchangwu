
// GuangChangWu.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGuangChangWuApp:
// �йش����ʵ�֣������ GuangChangWu.cpp
//

class CGuangChangWuApp : public CWinApp
{
public:
	CGuangChangWuApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGuangChangWuApp theApp;