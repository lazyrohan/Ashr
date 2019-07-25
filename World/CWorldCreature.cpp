#include "CWorldCreature.h"

AshrWorld::CWorldCreature::CWorldCreature() :
	life(0.0f)
{
	name = RandomName();
}

AshrWorld::CWorldCreature::~CWorldCreature()
{
}

void AshrWorld::CWorldCreature::Move(Point2 targetPos, double speed, double time)
{
	double angle = atan((targetPos.x - curPos.x) / (targetPos.y - curPos.y));
	curPos = Point2(curPos.x + speed * cos(angle) * time, curPos.y + speed * sin(angle) * time);
}

Point2 AshrWorld::CWorldCreature::GetCurPos()
{
	return curPos;
}

string AshrWorld::CWorldCreature::RandomName()
{
	mt19937_64 gen(chrono::high_resolution_clock::now().time_since_epoch().count());
	uniform_int_distribution<unsigned> dis(0, 9);
	return namelist[dis(gen)];
}

double AshrWorld::CWorldCreature::GrowRatio(unsigned time)
{
	if ((time >= 18 && time <= 24) || (time >= 0 && time <= 8))
		return 0.0f;
	else
		if (time >= 10 && time <= 16)
			return 0.02f;
		else
			return 0.005f;
}

double AshrWorld::CWorldCreature::MoveRatio(unsigned time)
{
	if ((time >= 18 && time <= 24) || (time >= 0 && time <= 8))
		return 0.5f;
	else
		if (time >= 10 && time <= 16)
			return 1.0f;
		else
			return 0.8f;
}

