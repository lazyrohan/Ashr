// World.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<filesystem>

//#include "CWorldPrim.h"

using namespace std;
using namespace std::filesystem;

int main()
{
	//AshrWorld::CWorldPrim neww;
	//neww.Initialize(10,10,100);
	//neww.Run();

	path p=L"D:\\00\\";
	directory_entry dir(p);
	recursive_directory_iterator files(p);
	for (auto itr : files)
	{
		wcout << itr.path().generic_wstring()<<endl;
	}
	cout << p.string();
}

