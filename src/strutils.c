#include "strutils.h"
#include "print.h"
#include "log.h"

uint32_t strlen(const uint8_t* str)
{
	if(!str){
		LOG_WRN("str == NULL");
		return -1;
	}

	uint32_t size = 0;
	while(*str++) ++size;
	return size+1; // include NULL byte
}

void strcpy(uint8_t* to, const uint8_t* from)
{
	if(!to){
		LOG_WRN("to == NULL");
		return;
	}
	if(!from){
		LOG_WRN("from == NULL");
		return;
	}

	while(*to++ = *from++){}
}

void strncpy(uint8_t* to, const uint8_t* from, uint32_t size)
{
	if(!to){
		LOG_WRN("to == NULL");
		return;
	}
	if(!from){
		LOG_WRN("from == NULL");
		return;
	}

	while(--size && (*to++ = *from++)){}
}

void memcpy(void* dst, void* src, size_t size)
{
	if(size <= 0)
		return;
	if(!dst || !src)
		return;

	while(size--)
		*(uint8_t*)dst++ = *(uint8_t*)src++;
}

void memset(void* dst, uint8_t byte, size_t size)
{
	if(size <= 0)
		return;
	if(!dst)
		return;

	while(size--)
		*(uint8_t*)dst++ = byte;
}

uint32_t itostr(int64_t it, char* res, uint32_t base)
{
	if(base < 1 || base > 16){
		LOG_ERR("itostr, wrong base");
		panic();
	}
	char* dig = "0123456789ABCDEF";

	uint8_t neg = 0;
	if(it < 0){
		neg = 1;
	}

	char temp[30];
	uint8_t i = 0;
	do{
		temp[i++] = dig[it%base];
	}while((it /= base));
	if(neg){
		temp[i++] = '-';
	}

	uint32_t size = i+1;
	while(i--){
		*res++ = temp[i];
	}
	*res = '\0';
	return size;
}
