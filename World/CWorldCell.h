/*
Cell object to define basic world physical unit, can't be split anymore
*/
#pragma once

#include <vector>

#include "CRootObject.h"
using namespace std;

namespace AshrWorld
{

	class CWorldCell :
		public CRootObject
	{
	public:
		CWorldCell();
		~CWorldCell();

		//Perperties
	public:
		//All in pxiels
		const double width=12.0f;
		const double height=12.0f;
		//Center Position
		Point2 centerPos;

		//Resource list
		double oreAmount;
		double foodAmount;
		virtual void Run();
		virtual void Stop();

	};

}


