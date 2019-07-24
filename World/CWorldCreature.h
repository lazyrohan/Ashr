#pragma once

#include<algorithm>
#include<random>
#include <chrono>
#include <string>
#include "CRootObject.h"

using namespace std;

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

class CWorldCreature :
	public CRootObject
{
public:
	CWorldCreature();
	~CWorldCreature();

	//Properties
public:
	double life;
	string name;
private:
	Point2 curPos;
	const string namelist[10] = {"刘小季","赵阿瞒","李二凤","赵大黑","朱二八","钱二狗","孙大柱","周狗蛋","吴大剩","郑小驴"};
	//Function
public:
	virtual void Grow()=0;
	virtual void Die() = 0;
	virtual void Move(Point2 targetPos,double speed,double time);
	Point2 GetCurPos();
	string RandomName();

private:
	double GrowRatio(unsigned time);
	double MoveRatio(unsigned time);
};

