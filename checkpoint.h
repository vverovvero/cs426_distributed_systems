//checkpoint.h 										Wendy Chen, 10/2016


#include "graph.h"

#include "stdint.h" //has type 'uint64_t'

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


#include <assert.h>
#include <inttypes.h>

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

typedef struct ch_superblock Ch_Superblock;
// typedef struct ch_block Ch_Block;

void * ch_load_block(size_t length);
void ch_free_block(void * addr, size_t length);
uint64_t ch_set_checksum(void * addr, size_t length);
int ch_get_serial_size(Graph *graph);
void ch_write_checkpoint(uint64_t *checkpoint, int serial_size_bytes, Graph *graph);
void ch_write_superblock(Ch_Superblock *ch_superblock, uint32_t generation, uint32_t num_blocks);

void ch_write_disk_block(int fd, int block_num, const void *addr);
void ch_write_disk_checkpoint(int fd, int num_blocks, const void *addr);

void ch_read_disk_block(int fd, int block_num, void *addr);
void ch_read_disk_checkpoint(int fd, int num_blocks, void *addr);

int dump_checkpoint(int fd, Graph *graph, uint32_t generation);
int load_checkpoint(int fd, Graph *graph);
void print_checkpoint(int fd);

int ch_check_validity_superblock(int fd);
int ch_check_validity_checkpoint(int fd, int num_blocks, int serial_size_bytes);
int ch_check_validity(int fd);

uint32_t checkpoint_get_generation(int fd);


#endif

