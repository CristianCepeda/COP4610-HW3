#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int Mem_Init(int size, int policy){
  // void *mmap(void *addr, size_t length, int prot, int flags,
  //                 int fd, off_t offset);

  int returnValue = 0;

  void *addr;
  size_t length = size;
  // int prot = PROT_EXEC;
  // ---------------------------------------------------------------------------------------------------
  // int flags = MAP_SHARED;
  // ---------------------------------------------------------------------------------------------------
  int fd;
  // ---------------------------------------------------------------------------------------------------
  off_t offset = 32;
  off_t pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE)-1); // WE ARE HARDCODING OUR OFFSET TO BE 32
  // ---------------------------------------------------------------------------------------------------

  void* p = mmap(addr,length,PROT_EXEC, MAP_SHARED, fd, pa_offset);


  return returnValue;
}

void* Mem_Alloc(int size){


}
