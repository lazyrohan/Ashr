/*
Root object of all world object

*/

#pragma once

#include "GlobalUnity.h"

namespace AshrWorld
{
	class CRootObject
	{
	public:
		CRootObject();
		~CRootObject();

		//Common Interface
			//virtual void Initialize()=0;
		virtual void Run() = 0;
		virtual void Stop() = 0;

		//Virables
	public:
		unsigned ID;

	};

}


