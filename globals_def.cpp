//globals_def.cpp												Wendy Chen, 12/2016

//Contains the global graph definition.  to be included once.

#include "graph.h"
#include "globals.h"

Graph graph;  /* Definition of the variable */
std::mutex partition_mtx;

//the 0th slot is always empty (to keep it simpler)
char* partition[10]; //array of 9 to hold up to 10 partitions