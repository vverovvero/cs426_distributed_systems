//globals.h													Wendy Chen, 12/2016

//Contains the global graph declaration.

#include "graph.h"
#include <mutex>  

#ifndef GLOBALS_H
#define GLOBALS_H

extern Graph graph;  /* Declaration of the variable */
extern std::mutex partition_mtx;

//the 0th slot is always empty (to keep it simpler)
extern char* partition[10]; //array of 9 to hold up to 10 partitions


#endif