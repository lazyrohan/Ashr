// World.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CWorldPrim.h"


using namespace std;

int main()
{
	CWorldPrim neww;
	neww.Initialize(20,20,100);
	neww.Run();
	

}
