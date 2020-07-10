#include <iostream>
#include <cstdio>

#include "Simulator.h"

int main(int argc, char **argv)
{
	FILE *data;
	if(argc == 2)
		data = fopen(argv[1], "r");
	else
		data = stdin;
	Simulator simulator(data);
	simulator.run();
	
	return 0;
}