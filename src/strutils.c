#include "strutils.h"
#include "print.h"
#include "log.h"

uint32_t strlen(const uint8_t* str)
{
	if(!str){
		LOG("str == NULL");
		return -1;
	}

	uint32_t size = 0;
	while(*str++) ++size;
	return size+1; // include NULL byte
}

void strcpy(uint8_t* to, const uint8_t* from)
{
	if(!to){
		LOG("to == NULL");
		return;
	}
	if(!from){
		LOG("from == NULL");
		return;
	}

	while(*to++ = *from++){}
}

void strncpy(uint8_t* to, const uint8_t* from, uint32_t size)
{
	if(!to){
		LOG("to == NULL");
		return;
	}
	if(!from){
		LOG("from == NULL");
		return;
	}

	while(--size && (*to++ = *from++)){}
}

