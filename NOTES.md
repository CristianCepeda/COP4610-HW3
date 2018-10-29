Notes on the Testmem.c and why it gives certain output

-the first myalloc(1000) fails becasue when you run it the function 
Mem_Alloc(size) is called but since freeHead is still NULL then the 
linked list is still empty. 

The first call to Mem_Int will be a success becasue it had never been called before thefore it will allocate the space and set the freeHead 
equal to the ptr at which mmap return

On the second call to Mem_Init it fails becasue freeHead is not eqaul 
to NULL anymore therfore it fails. 

/******************* IMPORTANT *************************/
to the struct mem_node we need to add an int that can be used later to 
determine the type of policy
so add some define like we did yesterday

#define FIRST-FIT 100
#define BEST-FIT 111
#define WORST-FIT 112

and use these constants inside Mem_Alloc