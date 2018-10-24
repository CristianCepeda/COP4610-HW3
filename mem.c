#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int Mem_Init(int size, int policy){
  // void *mmap(void *addr, size_t length, int prot, int flags,
  //                 int fd, off_t offset);

  void *addr;
  size_t length = size;

  int returnValue = 0;

  // open the /dev/zero device
  int fd = open("/dev/zero", O_RDWR);

  // size (in bytes) must be divisible by page size
  addr = mmap(NULL, length, PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (addr == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  // close the device (don't worry, mapping should be unaffected)
  close(fd);
  return addr;
}

void* Mem_Alloc(int size){


}
