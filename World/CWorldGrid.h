#pragma once

#include "CWorldCell.h"

namespace AshrWorld
{
	class CWorldGrid :
		public CWorldCell
	{
	public:
		CWorldGrid();
		~CWorldGrid();

	private:
		unsigned GridCell;
		CWorldCell cellnum;
	};
}


