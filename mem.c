#include "mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#define ERROR -1;

struct our_block* addNode_prev(struct our_block *newNode);
struct our_block* addNode_next(struct our_block *newNode);

struct our_block {
	struct our_block *next;
	struct our_block *prev;
	int	  size;
};

struct our_block *emptyNode = NULL;
int freeSize = 0;
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
		return ERROR;
	}

	
}

void *Mem_Alloc(int size)
{
	struct our_block *maxNode  = emptyNode;
	struct our_block *currentNode  = emptyNode;
	int sizeN;

	if(size % 8 != 0)
		size += 8 - (size % 8);

	if(emptyNode == NULL)
	{
		return NULL;	//empty list
	}

	do{
		if(currentNode->size > maxNode->size)
			maxNode = currentNode;

		currentNode = currentNode->next;
	}while(global_policy == MEM_POLICY_WORSTFIT);

	struct our_block* maxNodePrev = maxNode->prev;
	struct our_block* maxNodeNext = maxNode->next;
	int maxNodeSize = maxNode->size;

	if(maxNode->size < size + 24)
	{
		return NULL;
	}else if(maxNode->size == size + 24) {
		if(maxNode != emptyNode)
		{
			maxNode->prev->next = maxNodeNext;
		}
		if(maxNodeNext != NULL)
		{
			maxNode->next->prev = maxNodePrev;
		}
		if(maxNode == emptyNode)
		{
			emptyNode = maxNodeNext;
		}

		uintptr_t *new_size = (uintptr_t*)maxNode;
		*new_size = size;
		return (void*)new_size + 24;
	}else {
		void *returnVal;
		uintptr_t *new_size = (uintptr_t*)maxNode;
		returnVal =  (void*)new_size + 24;

		struct our_block* newNode = returnVal + size;
		
		newNode->prev = maxNodePrev;
		newNode->next = maxNodeNext;
		newNode->size = maxNodeSize - (size + 24);

		if(maxNode != emptyNode)
		{
			maxNode->prev->next = newNode;
		}
		if(maxNode->next != NULL)
		{
			maxNode->next->prev = newNode;
		}
		if(maxNode == emptyNode)
		{
			emptyNode = newNode;
		}

		*new_size = size;
		return returnVal;
	}

}

int Mem_Free(void *ptr)
{
	if(ptr == NULL) { return ERROR; }

	int *size = ptr - 24;

	struct our_block *beforeNode = emptyNode;
	struct our_block *newNode = (struct our_block*)size;

	if((uintptr_t*)size < (uintptr_t*)emptyNode)
	{
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
			return ERROR;
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
	return ERROR;
}

float Mem_GetFragmentation(){

	struct our_block *current = emptyNode;
	double maxFree = 0.0;
	double totalFree = 0.0;

	while (current != NULL){
		if(current->size > maxFree){ maxFree = current->size; }

		totalFree += current->size;
		current = current->next;
	}

	return (maxFree)/(totalFree);
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
