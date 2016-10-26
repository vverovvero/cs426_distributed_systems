//log.cpp															Wendy Chen, 10/16

//Contains functions for working with the log.

//init_log
//write_log
//get_checksum
//set_checksum

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
// #include <sys/stat.h>
// #include <fcntl.h>


void init_log(char *devfile){

	int fd;
	int length;
	int offset;
	char * addr;
	int s;

	fd = open(devfile, O_RDWR);

	length = 1024;
	offset = 0;
	addr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, offset);

	//modify addr from mmap

	//synchornize the memory map (to save changes?)
	msync(addr, length, MS_SYNC);

	(*addr) = "Hello, world!";

	//write changes to stdout, sanity check
	s = write(STDOUT_FILENO, addr, length);

}