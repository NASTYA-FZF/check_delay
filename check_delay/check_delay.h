
// check_delay.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CcheckdelayApp:
// Сведения о реализации этого класса: check_delay.cpp
//

class CcheckdelayApp : public CWinApp
{
public:
	CcheckdelayApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CcheckdelayApp theApp;
