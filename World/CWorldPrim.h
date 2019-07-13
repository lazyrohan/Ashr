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

class CWorldPrim :
	public CRootObject
{
public:
	CWorldPrim();
	~CWorldPrim();
   void Initialize(unsigned worldSizeW=100,unsigned worldSizeH=100,unsigned characterNum=10);
   void Run();
   void Stop();

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
		string DataStr()
		{
			stringstream ss;
			ss << "CN: " << characterAmount<<" "
				<< "FN: " << foodAmont<<" "
				<< "ON: " << oreAmount<<endl;
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

