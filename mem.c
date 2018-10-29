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

struct our_block {
    int start;
    int end;
};

typedef struct our_block my_block;

int block_size = sizeof(my_block);

int position(int index){
  index = map + (mem_size - index*block_size);
  return index;
}

struct our_block structure (int index){

  struct our_block *block;
  block = position(index);

  return *block;
}

int start_valObj(int index){
 struct our_block X;
 X.start = position(index);

 return X.start;
}

int end_valObj(int index){
 struct our_block X;
 X.end = position(index);

 return X.end;
}

//-----------------------------------------------

void objInsert(my_block obj){
	int position = objPosition(obj);

	for (int i = struc_size; i >= position; --i)
	{
		my_block X, I;
		X.start = start_valObj(i);
		X.end = end_valObj(i);
		I = structure((i+1));
	}

	structure(position) = obj;
	struc_size++;
	start_valObj(struc_size) = start_valObj(struc_size) - block_size;
}


void objRemove(my_block obj){

	int position = objPosition(obj) - 1;

	for (int i = position+1; i <= struc_size; ++i)
	{
		my_block X;
		X.start = start_valObj(i);
		X.end = end_valObj(i);
		structure((i-1)) = X;
	}
	--struc_size;
	start_valObj(struc_size) = start_valObj(struc_size) + block_size;
}


int objPosition(my_block obj){

	if (struc_size == 0){ return 1; }

	if (obj.start >= end_valObj(struc_size) || obj.start < 0 || obj.end < 0)
	{ return -1; }

	if (obj.end < start_valObj(1))
	{ return 1; }

	int x = 1, s_size = struc_size, tmp;
	while ((s_size - x) > 1)
	{
		tmp = (x + s_size)/2;
		if (obj.start < start_valObj(tmp))
		{
			s_size = tmp;
		}
		else
		{
			x = tmp;
		}
	}

	return s_size;
}

my_block findInObj(int offset){

	if (start_valObj(1) <= offset && offset < end_valObj(1))
	{
		return structure(1);
	}

	int x = 1, s_size = struc_size, tmp;
	while ((s_size - x) > 1)
	{
		tmp = (x + s_size)/2;
		if (start_valObj(tmp) <= offset && offset < end_valObj(tmp))
		{
			return structure(tmp);
		}
		else if (offset < start_valObj(tmp))
		{
			s_size = tmp;
		}
		else
		{
			x = tmp;
		}
	}

	my_block obj;
	obj.start = -1;
	obj.end = -1;

	return obj;
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
		if (fd == -1){ return FAIL; }

		map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

		if (map == MAP_FAILED){ return FAIL; }

		not_initalized = FALSE;
		global_policy = policy;

		my_block obj;
		obj.start =  mem_size; /* insertion will adjust it */
		obj.end = mem_size;
		objInsert(obj);
	}
	else
	{
		return FAIL;
	}

	close(fd);
	return PASS;
}

void* Mem_Alloc(int size){

  void *ptr = map;
  my_block obj;
  obj.start = 0;
  if (global_policy == MEM_POLICY_FIRSTFIT)
  {
    if (struc_size == 1 && size <= (start_valObj(1) - block_size))
    {
      obj.end = obj.start + size;

      objInsert(obj);
      return ptr + 0;
    }
    else if (struc_size > 1)
    {
      if (size <= start_valObj(1))
      {
        obj.end = obj.start + size;

        objInsert(obj);
        return ptr + 0;
      }
      for (int i = 1; i < struc_size; ++i)
      {
        if (i == (struc_size - 1))
        {
          if(size <= (start_valObj((i+1)) - end_valObj(i) - block_size))
          {
            obj.start = end_valObj(i);
            obj.end = obj.start + size;

            objInsert(obj);
            return ptr + end_valObj(i);
          }
        }
        else if(size <= (start_valObj((i+1)) - end_valObj(i)))
        {
          obj.start = end_valObj(i);
          obj.end = obj.start + size;

          objInsert(obj);
          return ptr + end_valObj(i);
        }
      }
    }
    return NULL;
  }
  else if(global_policy == MEM_POLICY_BESTFIT)
  {
    int i, best_fit = mem_size, best_position = -1;
    if (size <= (start_valObj(1)))
    {
      best_fit = start(1);
      best_position = 0;

    }
    for (i = 1; i < struc_size; ++i)
    {
      if (i == (struc_size - 1))
      {
        if(size <= (start_valObj((i+1)) - start_valObj(i) - block_size)
            && (start_valObj((i+1)) - start_valObj(i)) < best_fit)
        {
          best_fit = (start_valObj((i+1)) - start_valObj(i));
          best_position = i;
        }
      }
      else
      {
        if(size <= (start_valObj((i+1)) - start_valObj(i)) && (start_valObj((i+1)) - start_valObj(i)) < best_fit)
        {
          best_fit = (start_valObj((i+1)) - end_valObj(i));
          best_position = i;
        }
      }
    }
    if (best_position == 0)
    {
      obj.end = obj.start + size;
      objInsert(obj);
      return ptr + 0;
    }
    else if (best_position != -1)
    {
      obj.start = end_valObj(best_position);
      obj.end = obj.start + size;

      objInsert(obj);
      return ptr + end_valObj(best_position);
    }
    return NULL;
  }
  else if (global_policy == MEM_POLICY_WORSTFIT)
  {
    int i, worst_fit = 0, worst_position = -1;
    if (size <= (start_valObj(1)))
    {
      worst_fit = start_valObj(1);
      worst_position = 0;

    }
    for (i = 1; i < struc_size; ++i)
    {
      if (i == (struc_size - 1))
      {
        if(size <= (start_valObj((i+1)) - end_valObj(i) - block_size)
            && (start_valObj((i+1)) - end_valObj(i)) > worst_fit)
        {
          worst_fit = (start_valObj((i+1)) - end_valObj(i));
          worst_position = i;
        }
      }
      else
      {
        if(size <= (start_valObj((i+1)) - end_valObj(i)) && (start_valObj((i+1)) - end_valObj(i)) > worst_fit)
        {
          worst_fit = (start_valObj((i+1)) - end_valObj(i));
          worst_position = i;
        }
      }
    }
    if (worst_position == 0)
    {
      obj.end = obj.start + size;
      objInsert(obj);

      return ptr + 0;
    }
    else if (worst_position != -1)
    {
      obj.start = end_valObj(worst_position);
      obj.end = obj.start + size;

      objInsert(obj);
      return ptr + end_valObj(worst_position);
    }
    return NULL;
  }
  else
  {
    return (void*) FAIL;
  }
  return NULL;
}

int Mem_Free(void* ptr){

}

int Mem_IsValid(void* ptr){

}

int Mem_GetSize(void* ptr){

}

float Mem_GetFragmentation(){

}
