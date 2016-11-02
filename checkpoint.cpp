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
#define CHECKPOINT_NUM_SLOTS (134217727) //number of uint64_t that can be stored in checkpoint, ignoring checksum

#define CH_MAGIC_NUMBER (666)

struct checkpoint{
	uint64_t checksum;
	uint64_t slots[CHECKPOINT_NUM_SLOTS];
};

typedef struct checkpoint Checkpoint;

//////////////////////////////////////////////////////////////////
/* Checkpoint virtual functions									*/
//////////////////////////////////////////////////////////////////

void * ch_load_block(){
	void * addr = mmap(NULL, CHECKPOINT_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert( addr != MAP_FAILED);
	return addr;
}

void ch_free_block(void * addr){
	int munmap_return = munmap(addr, CHECKPOINT_SIZE);
	assert(munmap_return == 0);
}

void ch_synchronize(void *dest, const void *source){
	memcpy(dest, source, CHECKPOINT_SIZE);
	mysync(dest, CHECKPOINT_SIZE, MS_SYNC);
}

//for all 8GB, calculate and return checksum.  
//Given addr that points to beginning of checkpoint
uint64_t ch_set_checksum(void * addr){
  int i;
  //word points to next word after addr begins
  uint64_t *word = (uint64_t *) addr;
  word++;
  uint64_t checksum = 0;
  //iterate over entire checkpoint
  for(i=0; i<CHECKPOINT_NUM_SLOTS; i++){
    checksum = (*word) ^ checksum;
    word++;
  }
  checksum += CH_MAGIC_NUMBER;
  return checksum;
}

//return 1 if success.  0 if fail
//write checkpoint to memory specified by addr
int ch_write_checkpoint(void * addr, Graph *graph){
  	Checkpoint checkpoint;
  	int used_slots = 0;
	//Update checkpoint while space (include checksum)
	while(used_slots + 1 <= CHECKPOINT_NUM_SLOTS){
		//Write total number of nodes
		uint64_t num_nodes = (*graph).get_size();
		checkpoint.slots[used_slots++] = num_nodes;
		//Write all entries in for loop
		map<uint64_t, set<uint64_t> > nodes = (*graph).get_graph();
		for(map<uint64_t, set<uint64_t> >::iterator node_iterator = nodes.begin(); node_iterator != nodes.end(); node_iterator++){
			//Write the node
			uint64_t node_id = (*node_iterator).first;
			checkpoint.slots[used_slots++] = node_id;
			//Fetch the neighbors
			pair<int, set<uint64_t> > get_neighbors_result = (*graph).get_neighbors(node_id);
			set<uint64_t> neighbors = get_neighbors_result.second;
			//Write num neighbors
			uint64_t num_neighbors = neighbors.size();
			checkpoint.slots[used_slots++] = num_neighbors;
			//Loop through the neighbors
			for(set<uint64_t>::iterator neighbor_iterator = neighbors.begin(); neighbor_iterator != neighbors.end(); neighbor_iterator++){
				//Write the neighbor
				uint64_t neighbor_id = (*neighbor_iterator);
				checkpoint.slots[used_slots++] = neighbor_id;
			}
		}
		//synchronize
		ch_synchronize(addr, &checkpoint);
		//perform checksum
		checkpoint.checksum = ch_set_checksum(addr);
		ch_synchronize(addr, &checkpoint);
		//success
		return 1;
	}
	//fail, if used_slots exceeds 	
	return 0;
}


//////////////////////////////////////////////////////////////////
/* Checkpoint physical functions								*/
//////////////////////////////////////////////////////////////////
//ASSUME THAT DISK FD IS ALREADY KNOWN(from opening log)

//writes checkpoint to disk, addr is virtual checkpoint memory
void ch_write_disk(int fd, const void *addr){
	//Set to checkpoint offset in disk
	off_t lseek_return = lseek(fd, CHECKPOINT_DISK_OFFSET, SEEK_SET);
	assert(lseek_return != -1);
	//Copy information from struct into the disk
	ssize_t write_return = write(fd, addr, CHECKPOINT_SIZE);
	assert(write_return == CHECKPOINT_SIZE);
}

//read from disk into the buffer (addr is the block)
void ch_read_disk(int fd, void *addr){
	//Set to checkpoint offset in disk
	off_t lseek_return = lseek(fd, CHECKPOINT_DISK_OFFSET, SEEK_SET);
	assert(lseek_return != -1);
	//read information into the buffer
	ssize_t read_return = read(fd, addr, CHECKPOINT_SIZE);
	assert(read_return == CHECKPOINT_SIZE);
}

//////////////////////////////////////////////////////////////////
/* Dump Checkpoint												*/
//////////////////////////////////////////////////////////////////

//return 1 if success.  0 if fail (ie, run out of space)
//assumes fresh start
int dump_checkpoint(int fd, Graph *graph){
	//Load virtual memory
	Checkpoint *checkpoint = (Checkpoint *) ch_load_block();
	//Write checkpoint to virtual memory
	int checkpoint_result = ch_write_checkpoint(checkpoint, graph);
	//Fail if checkpoint failed
	if(checkpoint_result == 0){
		ch_free_block(checkpoint);
		return 0;
	}
	else{
		//Write checkpoint to disk
		ch_write_disk(fd, checkpoint);
		//Free virutal memory
		ch_free_block(checkpoint);
		//return success
		return 1;
	}
}


//////////////////////////////////////////////////////////////////
/* Load Checkpoint												*/
//////////////////////////////////////////////////////////////////

//only call load_checkpoint if ch_check_vaildity_checkpoint passed
//return 1 if success; 0 if fail (would it fail?)
//Rebuilds id
int load_checkpoint(int fd, Graph *graph){
	//Load virtual memory
	Checkpoint *checkpoint = (Checkpoint *) ch_load_block();
	//Read from disk
	ch_read_disk(fd, checkpoint);
	//Rebuild from checkpoint
	int read_nodes = 0;
	int slot_i = 0;
	uint64_t num_nodes = checkpoint->slots[slot_i++];
	while(read_nodes < num_nodes){
		//Get node
		uint64_t node_id = checkpoint->slots[slot_i++];
		(*graph).add_node(node_id);
		//Get neighbors
		uint64_t num_neighbors = checkpoint->slots[slot_i++];
		int read_neighbors = 0;
		while(read_neighbors < num_neighbors){
			//Get neighbor id
			uint64_t neighbor_id = checkpoint->slots[slot_i++];
			(*graph).add_node(neighbor_id);
			(*graph).add_edge(node_id, neighbor_id);
			//Finished reading neighbor.  Increment.
			read_neighbors++;
		}
		//Finished reading node.  Increment.
		read_nodes++;
	}
	//Free virtual memory
	ch_free_block(checkpoint);
	return 1;
}


//////////////////////////////////////////////////////////////////
/* Print functions												*/
//////////////////////////////////////////////////////////////////

//print out the checkpoint fetched from disk
int print_checkpoint(int fd){
	printf("Printing CHECKPOINT in disk\n");
	//Load virtual memory
	Checkpoint *checkpoint = (Checkpoint *) ch_load_block();
	//Read from disk
	ch_read_disk(fd, checkpoint);
	//Rebuild from checkpoint
	int read_nodes = 0;
	int slot_i = 0;
	uint64_t num_nodes = checkpoint->slots[slot_i++];
	printf("num_nodes: %llu\n", (unsigned long long) num_nodes);
	while(read_nodes < num_nodes){
		//Get node
		uint64_t node_id = checkpoint->slots[slot_i++];
		printf("node_id: %llu\n", (unsigned long long) node_id);
		//Get neighbors
		uint64_t num_neighbors = checkpoint->slots[slot_i++];
		printf("num_neighbors: %llu\n", (unsigned long long) num_neighbors);
		int read_neighbors = 0;
		while(read_neighbors < num_neighbors){
			//Get neighbor id
			uint64_t neighbor_id = checkpoint->slots[slot_i++];
			printf("neighbor_id: %llu\n", (unsigned long long) neighbor_id);
			//Finished reading neighbor.  Increment.
			read_neighbors++;
		}
		//Finished reading node.  Increment.
		read_nodes++;
	}
	//Free virtual memory
	ch_free_block(checkpoint);
	return 1;
}


//////////////////////////////////////////////////////////////////
/* Existence 													*/
//////////////////////////////////////////////////////////////////

//return 1 if block specified is valid.  return 0 if invalid
//check is based on checksum
int ch_check_validity_checkpoint(int fd){
	uint64_t original_checksum;
	uint64_t current_checksum;
	Checkpoint *checkpoint = (Checkpoint *) ch_load_block();
	ch_read_disk(fd, checkpoint);
	original_checksum = checkpoint->checksum;
	current_checksum = ch_set_checksum(checkpoint);
	printf("Validity check.  Original_checksum=%llu, current_checksum=%llu\n", (unsigned long long) original_checksum, (unsigned long long) current_checksum);
	ch_free_block(checkpoint);
	if(original_checksum == current_checksum){
		return 1;
	}
	else{
		return 0;
	}
}


