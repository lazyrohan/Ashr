#include "CCreature.h"

CCreature::CCreature():
life(0.0f)
{
}

CCreature::~CCreature()
{
}

void CCreature::Move(Point2 targetPos, double speed,double time)
{
	double angle = atan((targetPos.x - curPos.x) / (targetPos.y - curPos.y));
	curPos = Point2(curPos.x + speed * cos(angle)*time, curPos.y + speed * sin(angle)*time);
}

Point2 CCreature::GetCurPos()
{
	return curPos;
}

