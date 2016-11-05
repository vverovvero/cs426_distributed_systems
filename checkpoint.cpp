//checkpoint.cpp															Wendy Chen, 10/16

//Contains functions for checkpointing the graph

//dump_checkpoint - serialized current graph to disk
//load_checkpoint - rebuilds graph from serialization on disk

//dump_checkpoint: given graph pointer, checkpoint writes uint64_t numbers in format:
//byte 0: number of nodes in the graph
//Then for each node:
//uint64_t node_id
//uint64_t num_neighbors
//uint64_ts ... each num_neighbors
//Question; how to do this efficiently memory-wise.  What size block to load?
//I can dump this one entry at a time, even though it is slow

//load_checkpoint: Read uint64_t until all nodes have been parsed
//for each node and it's neighbors, check if node is in graph, then call add_node
//for each node-neighbor pair, check if edge is in graph, then call add_edge

//QUESTION:  Should the first uint64_t in the checkpoint be a checksum?  For validity check? -yes, save space for it.  XOR of all 8-bytes words in the 8GB block
//Design of the checkpoint for varying entry lengths? (ie. small loads inefficient?) - I can just load all 8GB
//Also, formatting the disk?  What should this entail? -only if -f specified.  Can zero out all memory.  Prevent error of random bits satisfying the checksum.
//When should logging or checkpointing errors be thrown?
//Lab1 - can the output be manually examined?

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
#include <string.h> //has memcpy? weird

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

const set<uint64_t> EmptySet;
const uint64_t Infinity = UINT64_MAX;


//////////////////////////////////////////////////////////////////
/* Globals and Structs											*/
//////////////////////////////////////////////////////////////////

//define max size of checkpoint area
#define CHECKPOINT_DISK_OFFSET (2147483648) //offset is size of log (first 2GB)
#define CHECKPOINT_SIZE (8589934592) //8GB total size
// #define CHECKPOINT_NUM_SLOTS (134217727) //number of uint64_t that can be stored in checkpoint, ignoring checksum
// #define CHECKPOINT_NUM_SLOTS (1073741816) //number of uint64_t ignoring checksum.  each is 8 byte

#define CH_BLOCK_SIZE (4096) //4096 bytes.  divides into 64 segments of 512 segments of 8-bytes
#define CH_BLOCK_NUM_SLOTS (512) //how many uint64_t fit in one block

#define CH_MAX_BLOCKS (2097151) //total number of blocks (excludes the superblock)

#define CH_MAX_SIZE (8589930496) //total max bytes, excluding superblock

#define CH_MAGIC_NUMBER (666)

struct ch_superblock{
	uint64_t checksum;
	uint32_t generation;
	uint32_t num_blocks; //how many blocks does the checkpoint take up
	uint32_t serial_size; //how many uint64_t does checkpoint serialize into?

};


typedef struct ch_superblock Ch_Superblock;

//Checkpoint is just a uint64_t * pointer

//////////////////////////////////////////////////////////////////
/* Checkpoint virtual functions									*/
//////////////////////////////////////////////////////////////////

//length should be the size of all uint64_t for the graph checkpoint
//length measured in bytees
//Can also be the size of the superblock
void * ch_load_block(size_t length){
	assert(length <= CHECKPOINT_SIZE);
	void * addr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
	assert( addr != MAP_FAILED);
	printf("Loaded block addr: %llu\n", (unsigned long long) addr);
	return addr;
}

void ch_free_block(void * addr, size_t length){
	int munmap_return = munmap(addr, length);
	assert(munmap_return == 0);
}

// void ch_synchronize(void *dest, const void *source){
// 	memcpy(dest, source, CHECKPOINT_SIZE);
// 	msync(dest, CHECKPOINT_SIZE, MS_SYNC);
// }

//addr points to start, goes for length in bytes
uint64_t ch_set_checksum(void * addr, size_t length){
  int i;
  //word points to next word after addr begins
  uint64_t *word = (uint64_t *) addr;
  word++;
  uint64_t checksum = 0;
  //iterate over entire checkpoint
  int num_words = length/4096 - 1;
  for(i=0; i<num_words; i++){
    checksum = (*word) ^ checksum;
    word++;
  }
  checksum += CH_MAGIC_NUMBER;
  return checksum;
}


//return the number of uint64_t need to send
int ch_get_serial_size(Graph *graph){
	int count = 2; //checksum and total_num_nodes
	map<uint64_t, set<uint64_t> > nodes = (*graph).get_graph();
	for(map<uint64_t, set<uint64_t> >::iterator node_iterator = nodes.begin(); node_iterator != nodes.end(); node_iterator++){
		//Get the node
		uint64_t node_id = (*node_iterator).first;
		//Fetch the neighbors
		pair<int, set<uint64_t> > get_neighbors_result = (*graph).get_neighbors(node_id);
		set<uint64_t> neighbors = get_neighbors_result.second;
		uint64_t num_neighbors = neighbors.size();
		count += 2; //for node_id and num_neighbors
		count += num_neighbors; //for each neighbor
	}
	return count;
}

// //number of slots is serial size, excluding checksum
// int set_checkpoint_num_slots(int num_slots){
// 	Checkpoint_Num_Slots = num_slots;
// }

//write checkpoint to memory specified by addr.  don't modify checkpoint addr
void ch_write_checkpoint(uint64_t *checkpoint, int serial_size_bytes, Graph *graph){
	printf("Writing checkpoint...\n");
	uint64_t *checkpoint_start = checkpoint;
	uint64_t *checkpoint_cur = checkpoint;
	checkpoint_cur++; //increment to skip checksum location
	//Write total number of nodes
	uint64_t num_nodes = (*graph).get_num_nodes();
	(*checkpoint_cur) = num_nodes;
	checkpoint_cur++;
	printf("num_nodes: %llu\n", (unsigned long long) num_nodes);
	//Write all entries in for loop
	map<uint64_t, set<uint64_t> > nodes = (*graph).get_graph();
	for(map<uint64_t, set<uint64_t> >::iterator node_iterator = nodes.begin(); node_iterator != nodes.end(); node_iterator++){
		//Write the node
		uint64_t node_id = (*node_iterator).first;
		(*checkpoint_cur) = node_id;
		checkpoint_cur++;
		printf("~~~~~~~~~~~~~~~\n");
		printf("node_id: %llu\n", (unsigned long long) node_id);
		//Fetch the neighbors
		pair<int, set<uint64_t> > get_neighbors_result = (*graph).get_neighbors(node_id);
		set<uint64_t> neighbors = get_neighbors_result.second;
		//Write num neighbors
		uint64_t num_neighbors = neighbors.size();
		(*checkpoint_cur) = num_neighbors;
		checkpoint_cur++;
		printf("num_neighbors: %llu\n", (unsigned long long) num_neighbors);
		//Loop through the neighbors
		for(set<uint64_t>::iterator neighbor_iterator = neighbors.begin(); neighbor_iterator != neighbors.end(); neighbor_iterator++){
			//Write the neighbor
			uint64_t neighbor_id = (*neighbor_iterator);
			(*checkpoint_cur) = neighbor_id;
			checkpoint_cur++;
			printf("neighbor_id: %llu\n", (unsigned long long) neighbor_id);
		}
	}
	//synchronize
	// ch_synchronize(addr, &checkpoint);
	msync(checkpoint_start, serial_size_bytes, MS_SYNC);
	//perform checksum
	uint64_t checksum = ch_set_checksum(checkpoint, serial_size_bytes);
	(*checkpoint_start) = checksum;
	printf("checksum: %llu\n", (unsigned long long) *checkpoint_start);
	// ch_synchronize(addr, &checkpoint);
	msync(checkpoint_start, serial_size_bytes, MS_SYNC);
}

//write the superblock information
void ch_write_superblock(Ch_Superblock *ch_superblock, uint32_t generation, uint32_t num_blocks, uint32_t serial_size){
	ch_superblock->generation = generation;
	ch_superblock->num_blocks = num_blocks;
	ch_superblock->serial_size = serial_size;
	msync(ch_superblock, CH_BLOCK_SIZE, MS_SYNC);
	uint64_t checksum = ch_set_checksum(ch_superblock, CH_BLOCK_SIZE);
	ch_superblock->checksum = checksum;
	msync(ch_superblock, CH_BLOCK_SIZE, MS_SYNC);
}


//////////////////////////////////////////////////////////////////
/* Checkpoint physical functions								*/
//////////////////////////////////////////////////////////////////
//ASSUME THAT DISK FD IS ALREADY KNOWN(from opening log)

//TO DO
//Function that writes to disk in CH_BLOCK_SIZE
//Function that writes checkpoint to disk. First, get how many blocks it takes
//Write the checkpoint to disk with CH_BLOCK_SIZE.  Increment checkpoint pointer accordinglyl



//write block to disk
void ch_write_disk_block(int fd, int block_num, const void *addr){
	printf("Writing to the disk...\n");
	//Set to checkpoint offset in disk
	off_t lseek_return = lseek(fd, CHECKPOINT_DISK_OFFSET + block_num * CH_BLOCK_SIZE, SEEK_SET);
	assert(lseek_return != -1);
	//Copy information from struct into the disk
	ssize_t write_return = write(fd, addr, CH_BLOCK_SIZE);
	printf("write_return: %lu\n", write_return);  //2147479552 (why is it only 2GB?)
	assert(write_return == CH_BLOCK_SIZE);
	printf("Finished writing to the disk\n");
}

//writes checkpoint to disk, addr is virtual checkpoint memory
void ch_write_disk_checkpoint(int fd, int num_blocks, const void *addr){
	uint64_t *checkpoint_cur = (uint64_t *) addr;
	for(int i=1; i<num_blocks+1; i++){
		ch_write_disk_block(fd, i, checkpoint_cur);
		checkpoint_cur += CH_BLOCK_NUM_SLOTS; //increment by num uint64_t's, or bytes?
	}
}

//read from disk into the buffer (addr is the block)
void ch_read_disk_block(int fd, int block_num, void *addr){
	printf("Reading from the disk...\n");
	//Set to checkpoint offset in disk
	off_t lseek_return = lseek(fd, CHECKPOINT_DISK_OFFSET + block_num * CH_BLOCK_SIZE, SEEK_SET);
	assert(lseek_return != -1);
	//read information into the buffer
	ssize_t read_return = read(fd, addr, CH_BLOCK_SIZE);
	printf("read_return: %lu\n", read_return);
	assert(read_return == CH_BLOCK_SIZE);
}

//read checkpoint in in number of blocks
void ch_read_disk_checkpoint(int fd, int num_blocks, void *addr){
	uint64_t *checkpoint_cur = (uint64_t *) addr;
	for(int i=1; i<num_blocks+1; i++){
		ch_read_disk_block(fd, i, checkpoint_cur);
		checkpoint_cur += CH_BLOCK_NUM_SLOTS;
	}
}

//////////////////////////////////////////////////////////////////
/* Dump Checkpoint												*/
//////////////////////////////////////////////////////////////////

//FAIL IF NOT ENOUGH SPACE
//return 1 if success.  0 if fail (ie, run out of space)
//assumes fresh start
int dump_checkpoint(int fd, Graph *graph, uint32_t generation){
	//Get size of the graph
	int serial_size = ch_get_serial_size(graph);
	printf("Serial_size: %d\n", serial_size);
	//Load virtual memory
	int serial_size_bytes = serial_size * 8;
	printf("Serial_size_bytes: %d\n", serial_size_bytes);
	if(serial_size_bytes > CHECKPOINT_SIZE){
		//checkpoint is too big.  Fail
		return 0;
	}
	uint64_t *checkpoint = (uint64_t *) ch_load_block(serial_size_bytes);
	printf("Checkpoint addr loaded\n");
	//Write checkpoint to virtual memory
	ch_write_checkpoint(checkpoint, serial_size_bytes, graph);
	printf("Checkpoint finished\n");

	//Load virtual memory for superblock
	Ch_Superblock *ch_superblock = (Ch_Superblock *) ch_load_block(CH_BLOCK_SIZE);
	uint32_t num_blocks = 1 + ((serial_size_bytes - 1) / CH_BLOCK_SIZE); // if x != 0
	ch_write_superblock(ch_superblock, generation, num_blocks, serial_size);

	//MODIFY HOW THE DISK IS WRITTEN!!!
	printf("Checkpoint success\n");
	//Write checkpoint to disk
	ch_write_disk_block(fd, 0, ch_superblock);
	ch_write_disk_checkpoint(fd, num_blocks, checkpoint);
	printf("Checkpoint copied to disk\n");
	//Free virutal memory
	ch_free_block(checkpoint, serial_size_bytes);
	ch_free_block(ch_superblock, CH_BLOCK_SIZE);
	//return success
	return 1;
}


//////////////////////////////////////////////////////////////////
/* Load Checkpoint												*/
//////////////////////////////////////////////////////////////////

//FAIL IF CHECKSUM NOT VALID
//ASSUMES CHECKSUMS PASSED
//only call load_checkpoint if ch_check_vaildity_checkpoint passed
//return 1 if success; 0 if fail (would it fail?) 
//Rebuilds id
int load_checkpoint(int fd, Graph *graph){
	printf("Loading checkpoint!\n");
	//First get information from superblock
	Ch_Superblock *ch_superblock = (Ch_Superblock *) ch_load_block(CH_BLOCK_SIZE);
	ch_read_disk_block(fd, 0, ch_superblock);
	int num_blocks = ch_superblock->num_blocks;
	int serial_size = ch_superblock->serial_size;
	int serial_size_bytes = serial_size * 8;
	//Load virtual memory
	uint64_t *checkpoint = (uint64_t *) ch_load_block(serial_size_bytes);
	//Read from disk
	ch_read_disk_checkpoint(fd, num_blocks, checkpoint);
	//Rebuild from checkpoint
	uint64_t *checkpoint_cur = checkpoint;
	uint64_t checksum = (*checkpoint_cur);
	checkpoint_cur++;
	uint64_t num_nodes = (*checkpoint_cur);
	checkpoint_cur++;
	uint64_t read_nodes = 0;
	while(read_nodes < num_nodes){
		//Get node
		uint64_t node_id = (*checkpoint_cur);
		checkpoint_cur++;
		(*graph).add_node(node_id);
		printf("Adding node: %llu\n", (unsigned long long) node_id);
		//Get neighbors
		uint64_t num_neighbors = (*checkpoint_cur);
		checkpoint_cur++;
		uint64_t read_neighbors = 0;
		while(read_neighbors < num_neighbors){
			//Get neighbor id
			uint64_t neighbor_id = (*checkpoint_cur);
			checkpoint_cur++;
			(*graph).add_node(neighbor_id);
			(*graph).add_edge(node_id, neighbor_id);
			printf("Adding neighbor: %llu\n", (unsigned long long) neighbor_id);
			//Finished reading neighbor.  Increment.
			read_neighbors++;
		}
		//Finished reading node.  Increment.
		read_nodes++;
	}
	//Free virtual memory
	ch_free_block(checkpoint, serial_size_bytes);
	ch_free_block(ch_superblock, CH_BLOCK_SIZE);
	printf("Finish loading checkpoint.\n");
	return 1;
}


//////////////////////////////////////////////////////////////////
/* Print functions												*/
//////////////////////////////////////////////////////////////////

//print out checkpoint fetched from disk
void print_checkpoint(int fd){
	printf("Printing CHECKPOINT in disk\n");
	//First get information from superblock
	Ch_Superblock *ch_superblock = (Ch_Superblock *) ch_load_block(CH_BLOCK_SIZE);
	printf("Loaded virtual mem for superblock\n");
	ch_read_disk_block(fd, 0, ch_superblock);
	int num_blocks = ch_superblock->num_blocks;
	int serial_size = ch_superblock->serial_size;
	int serial_size_bytes = serial_size * 8;
	//Load virtual memory
	printf("Need bytes for checkpoint: %d\n", serial_size_bytes);
	uint64_t *checkpoint = (uint64_t *) ch_load_block(serial_size_bytes);
	printf("Loaded virtual mem for checkpoint\n");
	//Read from disk
	ch_read_disk_checkpoint(fd, num_blocks, checkpoint);
	//Rebuild from checkpoint
	uint64_t *checkpoint_cur = checkpoint;
	uint64_t checksum = (*checkpoint_cur);
	checkpoint_cur++;
	uint64_t num_nodes = (*checkpoint_cur);
	checkpoint_cur++;
	printf("num_nodes: %llu\n", (unsigned long long) num_nodes);
	uint64_t read_nodes = 0;
	while(read_nodes < num_nodes){
		printf("~~~~~~~~~~~~~~~\n");
		//Get node
		uint64_t node_id = (*checkpoint_cur);
		checkpoint_cur++;
		printf("node_id: %llu\n", (unsigned long long) node_id);
		//Get neighbors
		uint64_t num_neighbors = (*checkpoint_cur);
		checkpoint_cur++;
		printf("num_neighbors: %llu\n", (unsigned long long) num_neighbors);
		uint64_t read_neighbors = 0;
		while(read_neighbors < num_neighbors){
			//Get neighbor id
			uint64_t neighbor_id = (*checkpoint_cur);
			checkpoint_cur++;
			printf("neighbor_id: %llu\n", (unsigned long long) neighbor_id);
			//Finished reading neighbor.  Increment.
			read_neighbors++;
		}
		//Finished reading node.  Increment.
		read_nodes++;
	}
	//Free virtual memory
	ch_free_block(checkpoint, serial_size_bytes);
	ch_free_block(ch_superblock, CH_BLOCK_SIZE);
}

//////////////////////////////////////////////////////////////////
/* Existence 													*/
//////////////////////////////////////////////////////////////////

//return 1 if is valid.  return 0 if invalid
//check is based on checksum
int ch_check_validity_superblock(int fd){
	uint64_t original_checksum;
	uint64_t current_checksum;
	Ch_Superblock *ch_superblock = (Ch_Superblock *) ch_load_block(CH_BLOCK_SIZE);
	ch_read_disk_block(fd, 0, ch_superblock);
	original_checksum = ch_superblock->checksum;
	current_checksum = ch_set_checksum(ch_superblock, CH_BLOCK_SIZE);
	printf("Validity check.  Original_checksum=%llu, current_checksum=%llu\n", (unsigned long long) original_checksum, (unsigned long long) current_checksum);
	ch_free_block(ch_superblock, CH_BLOCK_SIZE);
	if(original_checksum == current_checksum){
		return 1;
	}
	else{
		return 0;
	}
}


int ch_check_validity_checkpoint(int fd, int num_blocks, int serial_size_bytes){
	uint64_t original_checksum;
	uint64_t current_checksum;
	uint64_t *checkpoint = (uint64_t *) ch_load_block(serial_size_bytes);
	ch_read_disk_checkpoint(fd, num_blocks, checkpoint);
	original_checksum = (*checkpoint);
	current_checksum = ch_set_checksum(checkpoint, serial_size_bytes);
	printf("Validity check.  Original_checksum=%llu, current_checksum=%llu\n", (unsigned long long) original_checksum, (unsigned long long) current_checksum);
	ch_free_block(checkpoint, serial_size_bytes);
	if(original_checksum == current_checksum){
		return 1;
	}
	else{
		return 0;
	}
}

//returns checksum validity of both superblock and checkpoint
int ch_check_validity(int fd){
	//If superblock is invalid
	if(!ch_check_validity_superblock(fd)){
		return 0;
	}
	//Superblock is valid.  Fetch info and check checkpoint validity
	Ch_Superblock *ch_superblock = (Ch_Superblock *) ch_load_block(CH_BLOCK_SIZE);
	ch_read_disk_block(fd, 0, ch_superblock);
	int num_blocks = ch_superblock->num_blocks;
	int serial_size = ch_superblock->serial_size;
	int serial_size_bytes = serial_size * 8;
	if(ch_check_validity_checkpoint(fd, num_blocks, serial_size_bytes)){
		//Checkpoint is valid
		return 1;
	}
	else{
		//Checkpoint is invalid
		return 0;
	}

}


//////////////////////////////////////////////////////////////////
/* Functions for working with log								*/
//////////////////////////////////////////////////////////////////

uint32_t checkpoint_get_generation(int fd){
	Ch_Superblock *ch_superblock = (Ch_Superblock *) ch_load_block(CH_BLOCK_SIZE);
	ch_read_disk_block(fd, 0, ch_superblock);
	uint32_t checkpoint_generation = ch_superblock->generation;
	ch_free_block(ch_superblock, CH_BLOCK_SIZE);
	return checkpoint_generation;
}



//////////////////////////////////////////////////////////////////
/* Scramble functions	   									    */
//////////////////////////////////////////////////////////////////

//write giberish to devfile
//randomize first three blocks
void randomize_disk_checkpoint(int fd){
	int fd_random = open("/dev/urandom", O_RDONLY);
	assert(fd_random != -1);

	//iterate through all the blocks in checkpoint
	uint32_t i;
	for(i=0; i<3; i++){
	// for(i=0; i<CH_MAX_BLOCKS+1; i++){
		//allocate block size
		void *block = ch_load_block(CH_BLOCK_SIZE);
		//read random bytes
		int read_return = read(fd_random, block, CH_BLOCK_SIZE);
		// printf("read_return: %d\n", read_return);
		assert(read_return == CH_BLOCK_SIZE);
		// read_disk(fd_random, i, block);
		//write random bytes to disk
		ch_write_disk_block(fd, i, block);
		//free block
		ch_free_block(block, CH_BLOCK_SIZE);
	}
	close(fd_random);
}

