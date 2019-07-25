/*
Start point of world
*/

#pragma once
#include<iostream>
#include <vector>
#include <chrono>
#include <random>
#include<sstream>

#include "CRootObject.h"

using namespace std;

namespace AshrWorld
{
	class CWorldPrim :
		public CRootObject
	{
	public:
		CWorldPrim();
		~CWorldPrim();
		virtual void Initialize(unsigned worldSizeW = 100, unsigned worldSizeH = 100, unsigned characterNum = 10);
		virtual void Run();
		virtual void Stop();

	private:
		void WorldGen();
		void CharacterGen();
		void ResourceGen();
		void CreaterGen();
		void OreGen();

	public:
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
	private:
		const unsigned MaxCharacterNum = 100;
		unsigned WorldSizeW;
		unsigned WorldSizeH;
		unsigned WorldCellNum;
		unsigned CharacterNum;
		vector<CellObject> WorldObjs;
	};
}


