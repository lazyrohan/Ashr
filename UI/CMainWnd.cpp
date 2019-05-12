#include "CMainWnd.h"



CMainWnd::CMainWnd()
{
}


CMainWnd::~CMainWnd()
{
}

HRESULT CMainWnd::Initialize(HWND hParentWnd)
{
	HRESULT hr = S_OK;

	hr = CreateWnd(L"Main");


	return hr;
}
