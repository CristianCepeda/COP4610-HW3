#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "mem.h"

typedef struct our_block {
  size_t size;
  struct our_block* nextPtr; // Use this pointer to advance in the linked list
  int isFree; // This will be a boolean variable to check if the block is free or not. 
              // You may have freed this block and its in the middle of or linked list
              // so we could use this space later to store something else
} block;

int Mem_Init(int size, int policy) {
  // void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
  
  int pa_size = getpagesize();
  int nextUp_size = (int) ceil((double) size / pa_size) * pa_size;
  off_t offset;

  // -----------------------------------------------------------------------------
  // ---------------------------- CODE GIVEN TO US -------------------------------
  // open the /dev/zero device
  int fd = open("/dev/zero", O_RDWR);
  // size (in bytes) must be divisible by page size
  void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }
  // close the device (don't worry, mapping should be unaffected)
  close(fd);
  return 0;
}

void* Mem_Alloc(int size){
  if(size == 0){ return NULL; }
}

int Mem_Free(void* ptr){

}

int Mem_IsValid(void* ptr){

}

int Mem_GetSize(void* ptr){

}

float Mem_GetFragmentation(){

}

