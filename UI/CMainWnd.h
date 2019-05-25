/*
For program's top main window with native windows style
*/

#pragma once
#include "CRootWnd.h"

namespace Ashr
{
	namespace UI
	{
		class CMainWnd :
			public CRootWnd<CMainWnd>
		{
		public:
			CMainWnd();
			virtual ~CMainWnd();

		public:
			HRESULT Initialize(HWND hParentWnd = NULL);
		};
	}
}


