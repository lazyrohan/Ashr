/*
Root child wnd for in-program interface.
All windows elements/items are self-drawed by DirectX 2D.
*/


#pragma once
#include "CRootWnd.h"

namespace Ashr
{
	namespace UI
	{
		class CAshrWnd :
			public CRootWnd<CAshrWnd>
		{
		public:
			CAshrWnd();
			virtual ~CAshrWnd();
		
		};
	}
}


