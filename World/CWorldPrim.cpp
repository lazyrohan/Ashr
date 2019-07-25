#include "CWorldPrim.h"

AshrWorld::CWorldPrim::CWorldPrim():
	WorldSizeW(0),
	WorldSizeH(0),
	WorldCellNum(0),
	CharacterNum(0)
{
}

AshrWorld::CWorldPrim::~CWorldPrim()
{
}

void AshrWorld::CWorldPrim::Initialize(unsigned worldSizeW, unsigned worldSizeH, unsigned characterNum)
{
	WorldSizeW = worldSizeW;
	WorldSizeH = worldSizeH;
	WorldCellNum = worldSizeW * worldSizeH;
	CharacterNum = characterNum>MaxCharacterNum?MaxCharacterNum:characterNum;
	WorldObjs.resize(WorldCellNum);
	WorldGen();
}

void AshrWorld::CWorldPrim::Run()
{
	for (unsigned i=0;i<WorldCellNum;i++)
	{
		cout<<WorldObjs[i].DataStr();
		if (i % WorldSizeW == 0)
			cout << endl;
	}
}

void AshrWorld::CWorldPrim::Stop()
{
}

void AshrWorld::CWorldPrim::WorldGen()
{
	//WorldGrid
	//GridCell
	CharacterGen();
	ResourceGen();
	OreGen();
}

void AshrWorld::CWorldPrim::CharacterGen()
{
	//Random place character
	mt19937_64 gen(chrono::high_resolution_clock::now().time_since_epoch().count());
	uniform_int_distribution<unsigned> dis(0,WorldCellNum-1);
	for (unsigned i=0;i<CharacterNum;i++)
	{
		unsigned temp = dis(gen);
		WorldObjs[temp].characterIDs.push_back(i);
		WorldObjs[temp].characterAmount++;
	}
	
}

void AshrWorld::CWorldPrim::ResourceGen()
{
	mt19937_64 gen(chrono::high_resolution_clock::now().time_since_epoch().count());
	normal_distribution<double> dis(0.0f, 2.0f);
	for (unsigned i = 0; i < WorldCellNum; i++)
	{
		WorldObjs[i].foodAmont =abs(dis(gen));
	}
}

void AshrWorld::CWorldPrim::CreaterGen()
{
}

void AshrWorld::CWorldPrim::OreGen()
{
	mt19937_64 gen(chrono::high_resolution_clock::now().time_since_epoch().count());
	normal_distribution<double> dis(0.0f, 3.0f);
	for (unsigned i = 0; i < WorldCellNum; i++)
	{
		WorldObjs[i].oreAmount=abs(dis(gen));
	}
}
