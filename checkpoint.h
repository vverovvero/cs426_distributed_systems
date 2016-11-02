//checkpoint.h 										Wendy Chen, 10/2016

#include "graph.h"

#include "stdint.h" //has type 'uint64_t'

#include <iostream>
#include <stdlib.h> //has exit, EXIT_FAILURE
// #include <cstdint> //has type 'uint64_t'
#include <utility> //has type 'pair'
#include <map>
#include <set>
#include <algorithm>


using std::pair; 
using std::map;
using std::set;

#ifndef CHECKPOINT_H
#define CHECKPOINT_H


void * ch_load_block();
void ch_free_block(void * addr);
void ch_synchronize(void *dest, const void *source);
uint64_t ch_set_checksum(void * addr);
int ch_write_checkpoint(void * addr, Graph *graph);

void ch_write_disk(int fd, const void *addr);
void ch_read_disk(int fd, void *addr);

int dump_checkpoint(int fd, Graph *graph);

int load_checkpoint(int fd, Graph *graph);

int print_checkpoint(int fd);

int ch_check_validity_checkpoint(int fd);

#endif

