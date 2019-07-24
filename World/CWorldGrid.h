#pragma once
#include "CWorldCell.h"
class CWorldGrid :
	public CWorldCell
{
public:
	CWorldGrid();
	~CWorldGrid();

private:
	unsigned GridCell;

};

