#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "mem.h"

typedef struct our_block {
  size_t sizeB;
  struct our_block* prev;
  struct our_block* next;
}head;

int Mem_Init(int size, int policy){
  // void *mmap(void *addr, size_t length, int prot, int flags,
  //                 int fd, off_t offset);

  void *addr;
  size_t length = size;
  int pa_size = getpagesize();
  int nextUp_size = (int) ceil((double) size / pa_size) * pa_size;

  // open the /dev/zero device
  int fd = open("/dev/zero", O_RDWR);

  off_t offset;

  // size (in bytes) must be divisible by page size
  addr = mmap(NULL, nextUp_size, PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (addr == MAP_FAILED) {
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

