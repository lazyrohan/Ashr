/*
Global tools defination
*/

#pragma once

//Position point
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

//Vector 
struct Vector2
{
	double length;
	double angle;

	Vector2()
	{
		length = 0.0f;
		angle = 0.0f;
	};

	Vector2(double _length,double _angle)
	{
		length = _length;
		angle = _angle;
	};
};