#pragma once

#include<algorithm>
#include "CRootObject.h"

struct Point2
{
	double x;
	double y;

	Point2()
	{
		x = 0.0f;
		y = 0.0f;
	};
	Point2(double xw, double yh)
	{
		x = xw;
		y = yh;
	};
};

class CCreature :
	public CRootObject
{
public:
	CCreature();
	~CCreature();

	//Properties
public:
	double life;
private:
	Point2 curPos;

	//Function
public:
	virtual void Grow()=0;
	virtual void Die() = 0;
	virtual void Move(Point2 targetPos,double speed,double time);
	Point2 GetCurPos();

};

