// World.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CWorldPrim.h"

using namespace std;

int main()
{
	AshrWorld::CWorldPrim neww;
	neww.Initialize(10,10,100);
	neww.Run();

	cout << endl;
	for(auto i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
			cout << "|---|---|"<<endl;
		}
		cout << endl;
	}

}

