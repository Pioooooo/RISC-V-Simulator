#include <iostream>
#include <cstdio>

#include "Simulator.h"

int main(int argc, char **argv)
{
	char *fileName;
	if(argc == 2)
		fileName = argv[1];
	else
		return 0;
	Simulator simulator(fileName);
	simulator.run();

	return 0;
}