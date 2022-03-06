#pragma once

#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

class Info
{
public:
	static void GetMemory(unsigned long& total, unsigned long& vmsize, unsigned long& rsize, unsigned long& rshrd, unsigned long& rprvt);
};