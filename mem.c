#include "mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#define E_NO_SPACE            (1)
#define E_CORRUPT_FREESPACE   (2)
#define E_PADDING_OVERWRITTEN (3)
#define E_BAD_ARGS            (4)
#define E_BAD_POINTER         (5)

struct our_block* addNode_prev(struct our_block *newNode);
struct our_block* addNode_next(struct our_block *newNode);

struct our_block {
	struct our_block *next;
	struct our_block *prev;
	int	  size;
};

struct our_block *emptyNode = NULL;
int freeSize = 0;
int m_error;
int global_policy;

int Mem_Init(int size, int policy)
{
	if(size > 0 && emptyNode == NULL )
	{
		int pageSize = getpagesize();
		if(size % getpagesize() != 0)
			size += getpagesize() - (size % getpagesize());
		

		int fd = open("/dev/zero", O_RDWR);

		void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		if (ptr == MAP_FAILED) { perror("mmap"); exit(1); }
		close(fd);

		//get 8 byte alligned
		if((uintptr_t)ptr  % 8 != 0)
		{
			ptr += 8 - ((uintptr_t)ptr % 8);
			size -= 8 - ((uintptr_t)ptr % 8);
		}
		emptyNode = ptr;
		emptyNode->next = NULL;
		emptyNode->prev = NULL;
		emptyNode->size = size;
		global_policy = policy;

		freeSize = size;
		return 0;
	}else {
		m_error = E_BAD_ARGS;
		return -1;
	}

	
}

void *Mem_Alloc(int size)
{
	struct our_block *biggestNode  = emptyNode;
	struct our_block *currentNode  = emptyNode;

	if(size % 8 != 0)
		size += 8 - (size % 8);

	if(emptyNode == NULL)
	{
		//our list is empty
		m_error = E_NO_SPACE;
		return NULL;
	}

	//find the biggest free chunk (worst fit)
	do{
		if(currentNode->size > biggestNode->size)
			biggestNode = currentNode;

		currentNode = currentNode->next;
	}while(currentNode != NULL);

	struct our_block* biggestNodePrev = biggestNode->prev;
	struct our_block* biggestNodeNext = biggestNode->next;
	int biggestNodeSize = biggestNode->size;

	//check for freespace
	if(biggestNode->size < size + 24)
	{
		m_error = E_NO_SPACE;
		return NULL;
	}else if(biggestNode->size == size + 24) {
		if(biggestNode != emptyNode)
		{
			biggestNode->prev->next = biggestNodeNext;
		}
		if(biggestNodeNext != NULL)
		{
			biggestNode->next->prev = biggestNodePrev;
		}
		if(biggestNode == emptyNode)
		{
			emptyNode = biggestNodeNext;
		}

		uintptr_t *new_size = (uintptr_t*)biggestNode;
		*new_size = size;
		return (void*)new_size + 24;
	}else {
		void *returnVal;
		uintptr_t *new_size = (uintptr_t*)biggestNode;
		returnVal =  (void*)new_size + 24;

		struct our_block* newNode = returnVal + size;
		
		newNode->prev = biggestNodePrev;
		newNode->next = biggestNodeNext;
		newNode->size = biggestNodeSize - (size + 24);

		if(biggestNode != emptyNode)
		{
			biggestNode->prev->next = newNode;
		}
		if(biggestNode->next != NULL)
		{
			biggestNode->next->prev = newNode;
		}
		if(biggestNode == emptyNode)
		{
			emptyNode = newNode;
		}

		*new_size = size;
		return returnVal;
	}

}

int Mem_Free(void *ptr)
{
	if(ptr == NULL) { return -1; }
	
	int *size = ptr - 24;

	struct our_block *beforeNode = emptyNode;
	struct our_block *newNode = (struct our_block*)size;

	if(beforeNode == NULL)
	{
		//Freed chunk size of our entire init size
		
		newNode->size = *size + 24;
		newNode->prev = NULL;
		newNode->next = NULL;
		emptyNode = newNode;
		return 0;
	}else if((uintptr_t*)size < (uintptr_t*)emptyNode)
	{
		//Freed memory at beginning of list

		newNode->size = *size + 24;
		newNode->prev = NULL;
		newNode->next = emptyNode;
		emptyNode->prev = newNode;
		emptyNode = newNode;
	}else {

		while((void*)beforeNode->next < (void*)size && beforeNode->next != NULL)
		{
			beforeNode = beforeNode->next;
		}

		if (beforeNode == emptyNode)
		{
			newNode->size = *size + 24;
			newNode->prev = beforeNode;
			newNode->next = beforeNode->next;
			newNode->next->prev = newNode;
			beforeNode->next = newNode;
		}
		else if(beforeNode->next == NULL)
		{
			newNode->size = *size + 24;
			newNode->prev = beforeNode;
			newNode->prev->next = newNode;
			newNode->next = beforeNode->next;
			beforeNode->next = newNode;
		}
		else if(beforeNode != emptyNode)
		{
			newNode->size = *size + 24;
			newNode->prev = beforeNode;
			newNode->next = beforeNode->next;
			beforeNode->next = newNode;
			newNode->next->prev = newNode;
		}
	}

	addNode_next(addNode_prev(newNode));
	return 0;
}

int Mem_IsValid(void* ptr){
	return 1;
}

int Mem_GetSize(void* ptr){
	return -1;
}

float Mem_GetFragmentation(){
	return 1.0;
}


struct our_block* addNode_prev(struct our_block *newNode)
{
	struct our_block *prevNode = newNode->prev;
	if(prevNode != NULL)
	{
		if(((void*)prevNode + prevNode->size) == newNode)
		{
			prevNode->next = newNode->next;
			prevNode->size += newNode->size;
			prevNode->next->prev = prevNode;
			return prevNode;
		}
	}
	return newNode;
}

struct our_block* addNode_next(struct our_block *newNode)
{
	struct our_block *nextNode = newNode->next;
	if(nextNode != NULL)
	{
		if(((void*)newNode + newNode->size) == nextNode)
		{
			newNode->next = nextNode->next;
			if(newNode->next != NULL)
				newNode->next->prev = newNode;

			newNode->size += nextNode->size;
		}
	}
	return newNode;
}
