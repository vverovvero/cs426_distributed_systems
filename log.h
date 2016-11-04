//log.h												Wendy Chen, 10/2016

//Contains logging functions.  To be integrated with graph store setup
//and the api calls.

#ifndef LOG_H
#define LOG_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


#include <assert.h>
#include <string.h>

#include <stdint.h>
#include <inttypes.h>


// struct superblock{
//   uint64_t checksum;
//   uint32_t generation;
//   uint32_t start; //start of log segment
//   uint32_t size; //size of log segment
//   void *empty; //4076 bytes empty
// };

// //each, 20 bytes each
// struct entry{
//   uint32_t opcode;
//   uint64_t node_a_id;
//   uint64_t node_b_id;
// };

// struct block{
//   uint64_t checksum;
//   uint32_t generation;
//   uint32_t num_entries;
//   struct entry entries[MAX_ENTRIES]; //199 entries total, 0 to 198
//   void *empty; //4 bytes empty
// };

typedef struct superblock Superblock;
typedef struct entry Entry;
typedef struct block Block;

void print_superblock(void *addr);
void print_block(void *addr);

void * load_block();
void free_block(void * addr);
void synchronize(void *addr);
uint64_t set_checksum(void * addr);
void write_superblock(Superblock *superblock, uint32_t generation, uint32_t start, uint32_t size);
void write_block(Block *block, uint32_t generation, uint32_t num_entries);
void write_log(Block *block, uint32_t opcode, uint64_t node_a_id, uint64_t node_b_id);

int open_disk(const char *devfile);
void set_disk_offset(int fd, uint32_t block_num);
void write_disk(int fd, uint32_t block_num, const void *addr);
void read_disk(int fd, uint32_t block_num, void *addr);
void close_disk(int fd);

void read_superblock_from_disk(int fd, void *addr);
void write_superblock_to_disk(int fd, uint32_t generation, uint32_t start, uint32_t size);
void init_log_segment(int fd);
uint32_t get_current_block_num(int fd);
void read_block_from_disk(int fd, uint32_t block_num, void *addr);
void write_block_to_disk(int fd, uint32_t block_num, uint32_t generation, uint32_t num_entries);
int set_new_block_from_disk(int fd);
int set_writable_block_from_disk(int fd);
int write_log_to_disk(int fd, uint32_t opcode, uint64_t node_a_id, uint64_t node_b_id);

int check_validity_superblock(int fd);
int check_validity_block(int fd, uint32_t block_num);

uint32_t log_get_generation(int fd);
void log_increment_generation(int fd);

void rebuild_entry(Graph *graph, uint32_t opcode, uint64_t node_a_id, uint64_t node_b_id);
void play_log_from_disk(int fd, Graph *graph, uint32_t checkpoint_generation);

void randomize_disk_log(int fd);

#endif