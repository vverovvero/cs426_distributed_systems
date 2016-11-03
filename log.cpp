//log.cpp															Wendy Chen, 10/16

//Contains functions for working with the log.

//log.cpp                             Wendy Chen, 10/16

//Contains functions for working with the log.
//use open/read/write instead of fopen/fread/fwrite
//use mmap, not malloc (pin along the page boundaries)
//Mahesh says
//1. call mmap
//2. cast into a struct, manipulate with a pointer
//3. flush using struct


//refactor as a class later
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




#define MAX_ENTRIES (204) //entries 0 - 203, each 20 bytes
#define LOG_SIZE (4096) //4000 bytes
#define LOG_ENTRY (20) //20 bytes

#define LOG_SEGMENT_START_BYTES (4096) //starts at the 4000th byte
#define LOG_SEGMENT_SIZE_BYTES (2147479552) //bytes.  this is 2GB minus the superblock

#define LOG_SEGMENT_MAX_BLOCKS (524287) //total number of log_blocks (excludes the superblock)

#define MAGIC_NUMBER (666)

//QUESTIONS:

//TODO:
//Finish log functions
//Finish read_log and play_log (for calling graph functions)
//Finish print_log (sanity check)


//Be consistent on what means success (from graph.cpp)
// 1 is success
// 0 is fail

/////////////////////////////////////////////////////////////////////////////////////
/* Structs                                                                         */
/////////////////////////////////////////////////////////////////////////////////////

struct superblock{
  uint64_t checksum;
  uint32_t generation;
  uint32_t start; //start of log segment
  uint32_t size; //size of log segment
  void *empty; //4076 bytes empty
};

//each, 20 bytes each
struct entry{
  uint32_t opcode;
  uint64_t node_a_id;
  uint64_t node_b_id;
};

struct block{
  uint64_t checksum;
  uint32_t generation;
  uint32_t num_entries;
  struct entry entries[MAX_ENTRIES]; //199 entries total, 0 to 198
  void *empty; //4 bytes empty
};

typedef struct superblock Superblock;
typedef struct entry Entry;
typedef struct block Block;

/////////////////////////////////////////////////////////////////////////////////////
/* Print functions                                                                 */
/////////////////////////////////////////////////////////////////////////////////////


void print_superblock(void *addr){
  Superblock *block_ptr = (Superblock *) addr;
  printf("Superblock~~\n");
  printf("Checksum: %llu\n", (long long unsigned int) block_ptr->checksum);
  printf("Generation: %u\n", block_ptr->generation);
  printf("Start: %u\n", block_ptr->start);
  printf("Size: %u\n", block_ptr->size);
}


void print_block(void *addr){
  //how to increment this pointer correctly
  // Block *block_ptr = ((Block *) addr) + LOG_SIZE * block_num;
  Block *block_ptr = (Block *) addr;
  printf("block_ptr: %u\n", block_ptr);

  printf("Log Block~~\n");
  printf("Checksum: %llu\n", (long long unsigned int) block_ptr->checksum);
  printf("Generation: %u\n", block_ptr->generation);
  printf("Number of entries in block: %u\n", block_ptr->num_entries);
  printf("Entries~~\n");

  int i;
  for(i=0; i<block_ptr->num_entries; i++){
    printf("Entry #%d\n", i);
    Entry entry = block_ptr->entries[i];
    printf("Opcode: %u\n", entry.opcode);
    printf("Node_a_id: %llu\n", (long long unsigned int) entry.node_a_id);
    printf("Node_b_id: %llu\n", (long long unsigned int) entry.node_b_id);
  }
}

// //sanity check function, printing the whole log
// void print_log(void *addr);


/////////////////////////////////////////////////////////////////////////////////////
/* Writing to virtual memory                                                       */
/////////////////////////////////////////////////////////////////////////////////////

//loads block of LOG_SIZE, with contents initalized to zero
void * load_block(){
  void * addr = mmap(NULL, LOG_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert( addr != MAP_FAILED);
  // printf("Load_block, addr: %lu\n", addr);
  return addr;
}

//unmap address with block of LOG_SIZE
void free_block(void * addr){
  int munmap_return = munmap(addr, LOG_SIZE);
  assert( munmap_return == 0);
}

//synchronize to write struct into virtual block
void synchronize(void * dest, const void * source){
  memcpy(dest, source, LOG_SIZE);
  msync(dest, LOG_SIZE, MS_SYNC);
}

//given addr. skip first 8 bytes, XOR 63 different words
uint64_t set_checksum(void * addr){
  // printf("Checksum starting addr: %lu\n", (long unsigned int*) addr);
  int i;
  //word points to next word after addr begins
  uint64_t *word = (uint64_t *) addr;
  word++;
  uint64_t checksum = 0;
  for(i=0; i<63; i++){
    // printf("Checksum word %d addr: %lu\n", i, (long unsigned int) word);
    // printf("i: %d, word: %llu\n", i, (long long unsigned int) *word);
    checksum = (*word) ^ checksum;
    //increment pointer to next word position
    word++;
  }
  checksum += MAGIC_NUMBER;
  return checksum;
}

void write_superblock(void * addr, uint32_t generation, uint32_t start, uint32_t size){
  Superblock superblock;
  superblock.generation = generation;
  superblock.start = start; //starts at first block
  superblock.size = size; //initially, the log is size 0

  synchronize(addr, &superblock);

  //perform checksum
  superblock.checksum = set_checksum(addr);
  synchronize(addr, &superblock);
}

//write header information and perform checksum
//only writes fresh block!
void write_block(void *addr, uint32_t generation, uint32_t num_entries){
  Block block;
  block.generation = generation;
  block.num_entries = num_entries;

  synchronize(addr, &block);

  block.checksum = set_checksum(addr);
  synchronize(addr, &block);
}


//given appropriate block address, update the log in the block and copy back
//call write_log on pre-existing block
void write_log(void *addr, uint32_t opcode, uint64_t node_a_id, uint64_t node_b_id){
  printf("write_log called on: addr=%u, opcode=%lu, node_a_id=%llu, node_b_id=%llu\n", addr, (unsigned long) opcode, (unsigned long long) node_a_id, (unsigned long long) node_b_id);
  //first create a block space and copy address to block space
  Block block;
  synchronize(&block, addr);
  //do edits on block
  assert(block.num_entries < MAX_ENTRIES);
  //Update entry
  block.entries[block.num_entries].opcode = opcode;
  block.entries[block.num_entries].node_a_id = node_a_id;
  block.entries[block.num_entries].node_b_id = node_b_id;

  //update block
  block.num_entries = block.num_entries + 1;
  block.checksum = set_checksum(&block);
  //copy block back to address space
  synchronize(addr, &block);
}

/////////////////////////////////////////////////////////////////////////////////////
/* Writing to physical disk                                                        */
/////////////////////////////////////////////////////////////////////////////////////

//returns the fd
int open_disk(const char *devfile){
  int fd;
  fd = open(devfile, O_RDWR | O_DIRECT);
  assert(fd != -1);
  return fd;
}

//move the file offset to appropriate block number
void set_disk_offset(int fd, uint32_t block_num){
  //Set the write position to correct block offset
  off_t offset = block_num * LOG_SIZE;
  // printf("Setting to offset: %ld\n", offset);
  off_t lseek_return = lseek(fd, offset, SEEK_SET);
  assert(lseek_return != -1);
}

//writes a block (LOG_SIZE) to the disk, addr is start of virtual block
void write_disk(int fd, uint32_t block_num, const void *addr){
  // printf("Writing to disk\n");
  //Set the write position to correct block offset
  set_disk_offset(fd, block_num);
  //Copy information from struct into the disk
  ssize_t write_return = write(fd, addr, LOG_SIZE);
  assert(write_return == LOG_SIZE);
}

//read from disk into the buffer (addr is the block)
void read_disk(int fd, uint32_t block_num, void *addr){
  // printf("Reading from disk\n");
  //Set the write position to correct block offset
  set_disk_offset(fd, block_num);
  //read information into the buffer
  ssize_t read_return = read(fd, addr, LOG_SIZE);
  // printf("read_return: %u\n", read_return);
  assert(read_return == LOG_SIZE);

}

void close_disk(int fd){
  close(fd);
}


/////////////////////////////////////////////////////////////////////////////////////
/* Write the Virtual memory to Physical disk                                       */
/////////////////////////////////////////////////////////////////////////////////////

//read into virtual buffer; user needs to load_block and free_block
void read_superblock_from_disk(int fd, void *addr){
  read_disk(fd, 0, addr);
}

//nothing was written?
//all aspects of virtual and physical writing covered.  user does not need to load or free
void write_superblock_to_disk(int fd, uint32_t generation, uint32_t start, uint32_t size){
  printf("Calling write_superblock_to_disk!\n");
  Superblock *superblock = (Superblock *) load_block();
  printf("here1\n");
  read_disk(fd, 0, superblock);
  printf("here2\n");
  write_superblock(superblock, generation, start, size);
  print_superblock(superblock);
  printf("here3\n");
  write_disk(fd, 0, superblock);
  printf("here4\n");
  free_block(superblock);
  printf("here5\n");
}

//called before any logging happens
void init_log_segment(int fd){
  printf("Calling init_log_segment!\n");
  //superblock starts at generation = 0, start = 1, size = 0;
  write_superblock_to_disk(fd, 0, 1, 0);
};

//return the current_log_block
uint32_t get_current_block_num(int fd){
  uint32_t block_num;
  Superblock *superblock = (Superblock *) load_block();
  read_superblock_from_disk(fd, superblock);
  block_num = superblock->size;
  free_block(superblock);
  return block_num;
}

//read into virtual buffer; specify block_num
void read_block_from_disk(int fd, uint32_t block_num, void *addr){
  read_disk(fd, block_num, addr);
}

//only for fresh block!
//all aspects of virtual and physical writing covered.  user does not need to load or free
void write_block_to_disk(int fd, uint32_t block_num, uint32_t generation, uint32_t num_entries){
  printf("Write block to disk!\n");
  Block *block = (Block *) load_block();
  read_disk(fd, block_num, block);
  write_block(block, generation, num_entries);
  write_disk(fd, block_num, block);
  free_block(block);
}

//for use at checkpoint.  Grab completely new block by setting superblock and new block
//return 1 on success, 0 on fail
int set_new_block_from_disk(int fd){
  //load block to store superblock info
  Superblock *superblock = (Superblock *) load_block();
  //get current block, and increment the number
  uint32_t block_num = superblock->size;
  block_num++;
  //assert that new block is still in log segment
  if(block_num > LOG_SEGMENT_MAX_BLOCKS){
  	free_block(superblock);
  	return 0;
  }
  //read_superblock_from_disk to get old values
  read_superblock_from_disk(fd, superblock);
  //write_superblock_to_disk to perform update (size has been incremented)
  write_superblock_to_disk(fd, superblock->generation, superblock->start, block_num);
  //write_block_to_disk for new block
  write_block_to_disk(fd, block_num, superblock->generation, 0);
  //free superblock
  free_block(superblock);
  return 1;
}

//for use when writing a log.  Do nothing if current block is writable, otherwise call set_new_block_from_disk
//return 1 on success, 0 on fail
int set_writable_block_from_disk(int fd){
  //Load block to store superblock info
  Superblock *superblock = (Superblock *) load_block();
  read_superblock_from_disk(fd, superblock);
  //get current superblock->start and superblock->size
  uint32_t start = superblock->start;
  uint32_t size = superblock->size;

  //if block_num is 1 and size is 0, then log is brand new.  Begin log.
  if((start == 1) && (size == 0)){
    if(set_new_block_from_disk(fd)){
    	free_block(superblock);
    	return 1;
    }
    else{
    	free_block(superblock);
    	return 0;
    }
  }
  //else, load the current block
  else{
    Block *block = (Block *) load_block();
    uint32_t num_entries = block->num_entries;
    //if current block has not reached max_entries, do nothing
    if(num_entries < MAX_ENTRIES){
      free_block(block);
      return 1;
    }
    //else, if current block is full, then set_new_block_from_disk
    else{
     if(size < LOG_SEGMENT_MAX_BLOCKS){
      if(set_new_block_from_disk(fd)){
      	free_block(block);
      	return 1;
      }
      else{
      	free_block(block);
      	return 0;
      }
     } 
     else{
      //log segment is completely full
      free_block(block);
      return 0;
     }
    }
  }
}


//calls set_writable_block.  Updates log and block.
//return 1 on success, else return 0
int write_log_to_disk(int fd, uint32_t opcode, uint64_t node_a_id, uint64_t node_b_id){
  //set_writable_block_from_disk
  if(set_writable_block_from_disk(fd) == 0){
  	//failed!  no space
  	return 0;
  }
  //get_current_block_num
  uint32_t block_num = get_current_block_num(fd);
  //load a block for Block
  Block *block = (Block *) load_block();
  //read_block_from_disk
  read_disk(fd, block_num, block);
  //find the empty entry and modify it (this should also update the log block header)
  write_log(block, opcode, node_a_id, node_b_id);
  //write_disk
  write_disk(fd, block_num, block);
  //free_block
  free_block(block);
  return 1;
}


//////////////////////////////////////////////////////////////////
/* Existence 													*/
//////////////////////////////////////////////////////////////////

//return 1 if block specified is valid.  return 0 if invalid
//check is based on checksum
int check_validity_superblock(int fd){
	uint64_t original_checksum;
	uint64_t current_checksum;
	Superblock *superblock = (Superblock *) load_block();
	read_superblock_from_disk(fd, superblock);
	original_checksum = superblock->checksum;
	current_checksum = set_checksum(superblock);
	printf("Validity check.  Original_checksum=%llu, current_checksum=%llu\n", (unsigned long long) original_checksum, (unsigned long long) current_checksum);
	free_block(superblock);
	if(original_checksum == current_checksum){
		return 1;
	}
	else{
		return 0;
	}
}

int check_validity_block(int fd, uint32_t block_num){
	uint64_t original_checksum;
	uint64_t current_checksum;
	Block *block = (Block *) load_block();
	read_block_from_disk(fd, block_num, block);
	original_checksum = block->checksum;
	current_checksum = set_checksum(block);
	free_block(block);
	if(original_checksum == current_checksum){
		return 1;
	}
	else{
		return 0;
	}
}


//////////////////////////////////////////////////////////////////
/* Reconstruction 												*/
//////////////////////////////////////////////////////////////////

//untested!!
//read_log_from_disk, given virtual memory space the size of an entry
void read_log_from_disk(int fd, void *addr){
  //get_current_block_num
  uint32_t block_num = get_current_block_num(fd);
  //load a block
  Block *block = (Block *) load_block();
  //read_block_from_disk
  read_block_from_disk(fd, block_num, block);
  //memcopy? void *memcpy(void *dest, const void *src, size_t n); for 20 bytes
  memcpy(block, addr, 20);
  //free block
  free_block(block);
}

//play_log_from_disk (this should be hooked up to the API)
//plays log up until current generation
//should check if checksum is current before playing the block

//////////////////////////////////////////////////////////////////
/* Scramble functions	   									    */
//////////////////////////////////////////////////////////////////

//write giberish to devfile
void randomize_disk(int fd){
	int fd_random = open("/dev/urandom", O_RDONLY);
	assert(fd_random != -1);

	uint32_t i;
	for(i=0; i<MAX_ENTRIES+1; i++){
		//allocate block size
		Block *block = (Block *) load_block();
		//read random bytes
		int read_return = read(fd_random, block, LOG_SIZE);
		// printf("read_return: %d\n", read_return);
		assert(read_return == LOG_SIZE);
		// read_disk(fd_random, i, block);
		//write random bytes to disk
		write_disk(fd, i, block);
		//free block
		free_block(block);
	}
	close(fd_random);
}

//////////////////////////////////////////////////////////////////


// int main(int argc, char *argv[])
// {
//   assert(argc == 2);

//   int fd;
//   void * addr;
//   int s;

//   //Get the fd to the "/dev/sdb" disk
//   fd = open_disk(argv[1]);
//   printf("fd: %d\n", fd);

//   // // Testing virtual functions and physical functions
//   // Superblock *superblock = (Superblock *) load_block();
//   // Superblock *new_superblock = (Superblock *) load_block();
//   // write_superblock(superblock, 0, 1, 0);
//   // print_superblock(superblock);
//   // //Testing writing to disk
//   // write_disk(fd, 0, superblock);
//   // read_disk(fd, 0, new_superblock);
//   // print_superblock(new_superblock);

//   // Block *block = (Block *) load_block();
//   // Block *new_block = (Block *) load_block();
//   // write_block(block, superblock->generation, 0);
//   // //Test block
//   // write_disk(fd, 1, block);
//   // read_disk(fd, 1, new_block);
//   // print_block(new_block);
//   // //Write to the log
//   // write_log(block, 0, 0, 0);
//   // write_log(block, 0, 555, 0);
//   // write_log(block, 0, 666, 0);
//   // write_log(block, 1, 555, 666);
//   // print_block(block);
//   // //Testing writing to disk
//   // write_disk(fd, 1, block);
//   // read_disk(fd, 1, new_block);
//   // print_block(new_block);

//   // //Free everything
//   // free_block(superblock);
//   // free_block(new_superblock);
//   // free_block(block);
//   // free_block(new_block);

//   ///////


//   //Testing the integrated functions
//   //Initalize the superblock
//   init_log_segment(fd);

//   //Testing superblock
//   printf("Sanity check superblock initialization\n");
//   Superblock *superblock = (Superblock *) load_block();
//   read_superblock_from_disk(fd, superblock);
//   print_superblock(superblock);

//   //Testing block
//   // printf("Sanity check for writing a fresh block\n");
//   // write_block_to_disk(fd, 1, 0, 0);
//   // Block *block = (Block *) load_block();
//   // read_block_from_disk(fd, 1, block);
//   // print_block(block);

//   //Write a log
//   printf("WRITING EXAMPLES TO LOG\n");
//   write_log_to_disk(fd, 0, 555, 0);
//   write_log_to_disk(fd, 0, 666, 0);
//   write_log_to_disk(fd, 1, 555, 666);
//   write_log_to_disk(fd, 3, 666, 555);
//   write_log_to_disk(fd, 2, 666, 0);
//   write_log_to_disk(fd, 2, 555, 0);

//   printf("Sanity check block 1\n");
//   Block *block = (Block *) load_block();
//   read_block_from_disk(fd, 1, block);
//   print_block(block);


//   //Free everything
//   free_block(superblock);
//   free_block(block);


//   //cleanup
//   close_disk(fd);
  
//   return 0;
// }
