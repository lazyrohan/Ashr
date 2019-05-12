#pragma once
#include "CRootWnd.h"

using namespace Ashr::UI;

class CMainWnd :
	public CRootWnd<CMainWnd>
{
public:
	CMainWnd();
	virtual ~CMainWnd();

public:
	HRESULT Initialize(HWND hParentWnd = NULL);
};

