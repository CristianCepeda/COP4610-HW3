#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "mem.h"

#define TRUE 1
#define FALSE 0
#define PASS 0
#define FAIL -1

void *map;
void *ptr;
int mem_size;
int global_policy;
int not_initalized = TRUE;
int struc_size = 0;

//DS_UNIT
int block_size = sizeof(our_block);

//POS(a)
int position(int index){
  index = map + (mem_size - index*block_size);
  return index;
}


//DS(a)
struct our_block structure (int index){

  struct our_block *block;
  block = position(index);  

  return block;
}


int start_valObj(int index){
 struct our_block *block;
 block.start = position(index);  

 return block.start;
}


int end_valObj(int index){
 struct our_block *block;
 block.end = position(index);  

 return block.end;
}

typedef struct our_block {
    int start;
    int end;
} 


//-----------------------------------------------

void objInsert(our_block obj)
{	
	int position = objPosition(obj);
	
	for (int i = struc_size; i >= position; --i)
	{
		our_block X;
		X.start = start_valObj(i);
		X.end = end_valObj(i);
		structure((i+1)) = X;
	}
	structure(position) = obj;
	struc_size++;
	start_valObj(struc_size) = start_valObj(struc_size) - block_size;
}


void objRemove(our_block obj)
{
	
	int position = objPosition(obj) - 1;
	
	for (int i = position+1; i <= struc_size; ++i)
	{
		our_block X;
		X.start = start_valObj(i);
		X.end = end_valObj(i);
		structure((i-1)) = X;
	}
	--struc_size;
	start_valObj(struc_size) = start_valObj(struc_size) + block_size;	
}


int objPosition(our_block obj)
{
	
	if (DS_size == 0)
	{
		#ifdef DEBUG
			printf("    Size = 0.\n");
		#endif		
		return 1;
	}
	if (obj.start >= DSF(DS_size) || obj.start < 0 || obj.end < 0)
	{
		#ifdef DEBUG
			printf("    Memory out of bounds.\n");
		#endif	
		return -1;
	}
	if (obj.end < DSI(1))
	{
		#ifdef DEBUG
			printf("    Pos = 1.\n");
		#endif	
		return 1;
	}
	int l = 1, r = DS_size, m;
	while ((r-l) > 1)
	{
		m = (l+r)/2;
		if (obj.start < DSI(m)) 
		{
			r = m;
		}
		else 
		{
			l = m;
		}
	}
	
	return r;
}

our_block findInObj(int offset)
{
	#ifdef DEBUG
		printf("    &&& Serching offset: %d.\n", offset);
	#endif
	if (DSI(1) <= offset && offset < DSF(1))
	{
		#ifdef DEBUG
			printf("    &&& Offset in first allocation\n");
		#endif
		return DS(1);
	}
	int l = 1, r = DS_size, m;
	while ((r-l) > 1)
	{
		m = (l+r)/2;
		if (DSI(m) <= offset && offset < DSF(m)) 
		{
			#ifdef DEBUG
				printf("    Offset at Interval #%d.\n", m);
			#endif
			return DS(m);
		}
		else if (offset < DSI(m)) 
		{
			r = m;
		}
		else 
		{
			l = m;
		}
	}
	#ifdef DEBUG
		printf("    Offset not in Interval.\n");
	#endif
	our_block I;
	I.initial = -1;
	I.final = -1;
	return I;
}



//-----------------------------------------------


int Mem_Init(int size, int policy) {

  	int fd;
	if (not_initalized) 
	{
		int pa_size = getpagesize();
		int roundUP_size = size + pa_size - size%pa_size;

		/* Adjusting provided size to next multiple of pagesize */
		mem_size = (size%pa_size) ? roundUP_size : size;

		/* Creates the memory file with read, write and execute permissions
		 * set for the current user */
		fd = open("/dev/zero", O_RDWR);
		if (fd == -1)
		{
			return FAIL;
		}
 
		ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

		if (ptr == MAP_FAILED) 
		{
			return EXIT_ERROR;
		}

		not_initalized = FALSE;
		global_policy = policy;

		our_block obj;
		obj.start =  mem_size; /* insertion will adjust it */
		obj.end = mem_size;
		objInsert(I);

		#ifdef DEBUG
			print_DS();
		#endif
	}
	else 
	{
		#ifdef DEBUG
			printf("Mem already initialized.\n");
		#endif
		return EXIT_ERROR;
	}

	close(fd);
	#ifdef DEBUG
		printf("*** Success, memory initialized.\n\n");
	#endif
	return EXIT_SUCCESS;
}

void* Mem_Alloc(int size){
 
}

int Mem_Free(void* ptr){

}

int Mem_IsValid(void* ptr){

}

int Mem_GetSize(void* ptr){

}

float Mem_GetFragmentation(){

}

