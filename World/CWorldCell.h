/*
Cell object to define basic world physical unit
*/
#pragma once


#include "CRootObject.h"

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
		//Size in pxiels
		double width;
		double height;
		//Center Position

		//resource
		struct CellObject
		{
			vector<unsigned> characterIDs;
			unsigned characterAmount;
			vector<unsigned> CreatureIDs;
			unsigned creatureAmount;
			double oreAmount;
			double foodAmont;

			CellObject()
			{
				characterAmount = 0;
				creatureAmount = 0;
				oreAmount = 0.0f;
				foodAmont = 0.0f;
			};

			~CellObject()
			{
				characterIDs.clear();
				CreatureIDs.clear();
			};

			string DataStr()
			{
				stringstream ss;
				ss << "I.D:";
				for (auto itr : characterIDs)
					ss << itr << " ";
				ss << "C.N:" << characterAmount << " "
					<< "F.N:" << foodAmont << " "
					<< "O.N:" << oreAmount << " / ";
				return ss.str();
			};
		};

	};
}


