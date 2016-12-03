//globals_def.cpp												Wendy Chen, 12/2016

//Contains the global graph definition.  to be included once.

#include "graph.h"
#include "globals.h"

int test = 666;
Graph graph;  /* Definition of the variable */

void test(void){
	test++;
	printf("test: %d\n", test);
}


int main(int argc, char const *argv[])
{
	test();
	return 0;
}
