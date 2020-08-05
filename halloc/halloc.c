#include "halloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

//#define __DEBUG__
#define SMALLESTBLOCK 16

static void* firstBlock = NULL;
static void* endMem = NULL;

//private structs
typedef struct{
	size_t size;
}beginStruct;

typedef struct{
	beginStruct* ptr;
}endStruct;

//private functions
static int initBlock(void* ptr, size_t size);
static int isBlockFree(beginStruct* location);
static size_t getRealSize(beginStruct* ptr);
static void setFreeSize(beginStruct* ptr);
static void setRealSize(beginStruct* ptr);
static void* requestBlock(size_t size);
static void* findFree(size_t size);
static void splitBlock(void* ptr, size_t size);
static void* getNextBlock(void* ptr);
static void* getPreviousBlock(void* ptr);
static size_t getFullSize(size_t size);
static void mergeBlocks(void* pB1);


void* halloc(size_t size)
{
	if(size<=0)
		return NULL;
	//allocate only even size
	if(size%2)
		size++;
	void* ptr = NULL;
	if(firstBlock != NULL){
		ptr = findFree(getFullSize(size));
		if(ptr != NULL){
			if(((beginStruct*)ptr)->size >= getFullSize(SMALLESTBLOCK))
				splitBlock(ptr, size);
			return ptr + sizeof(beginStruct);
		}
	}
	ptr = requestBlock(getFullSize(size));
	if(ptr != NULL){
		initBlock(ptr, size);
		setRealSize(ptr);
		return ptr + sizeof(beginStruct);
	}
	return NULL;
}

void hfree(void* ptr)
{
	ptr = ptr-sizeof(beginStruct);
	if(!ptr)
		return;
	if(isBlockFree(ptr))
		return;
	setFreeSize(ptr);
	if(getNextBlock(ptr)!=endMem)
		mergeBlocks(ptr);
	if(ptr != firstBlock)
		mergeBlocks(getPreviousBlock(ptr));
}

//Init block with begin and end structures;
static int initBlock(void* ptr, size_t size)
{
	beginStruct* bgSt = (beginStruct*)ptr;
	endStruct* enSt = ptr+size+sizeof(beginStruct);

	bgSt->size = size;
	enSt->ptr = bgSt;

	setFreeSize(ptr);
	return 1;
}

//request memory from kernel
static void* requestBlock(size_t size)
{
	void* ptr = sbrk(size);
	if(ptr == (void*)-1)
		return NULL;
	if(!firstBlock)
		firstBlock = ptr;
	endMem = ptr+size;
#ifdef __DEBUG__
	printf("ALLOCATE ADRESS:\nST: %p\nEND: %p\n",ptr, sbrk(0));
	fflush(stdout);
#endif
	return ptr;
}

static void splitBlock(void* ptr, size_t size)
{
	beginStruct* block1 = ptr;
	beginStruct* block2 = ptr+getFullSize(size);
	initBlock(block2, block1->size - size - 
			sizeof(endStruct) - sizeof(beginStruct));
	initBlock(block1, size);
#ifdef __DEBUG__
	printf("SPLITBLOCKS:\n b1: %p\n b2: %p\n", block1, block2);
#endif
}

static void* findFree(size_t size)
{
	assert(firstBlock != NULL);
	beginStruct* block = firstBlock;
	do{
		if(isBlockFree(block) && block->size >= size)
			return block;
	} while((block = getNextBlock(block)) != endMem);
	return NULL;
}

static void mergeBlocks(void* pB1)
{
	void* nextBlock = getNextBlock(pB1);
	if(isBlockFree(nextBlock)&&isBlockFree(pB1)){
		setRealSize(pB1);
		setRealSize(nextBlock);
		((endStruct*)(nextBlock+sizeof(beginStruct)+
			getRealSize(nextBlock)))->ptr = pB1;
		((beginStruct*)pB1)->size += getFullSize(((beginStruct*)nextBlock)->size);
		setFreeSize(pB1);
	}
}

static void* getNextBlock(void* ptr)
{
	return ptr+getRealSize(ptr)+
		sizeof(endStruct)+sizeof(beginStruct);
}

static void* getPreviousBlock(void* ptr)
{
	return ((endStruct*)(ptr-sizeof(endStruct)))->ptr;
}

static size_t getFullSize(size_t size)
{
	return size + sizeof(endStruct) + sizeof(beginStruct);
}

static int isBlockFree(beginStruct* ptr)
{
	return ptr->size & 1;
}

static size_t getRealSize(beginStruct* ptr)
{
	return (ptr->size & ~1);
}

static void setRealSize(beginStruct* ptr)
{
	ptr->size = ptr->size & ~1;
}

static void setFreeSize(beginStruct* ptr)
{
	ptr->size = ptr->size | 1;
}
