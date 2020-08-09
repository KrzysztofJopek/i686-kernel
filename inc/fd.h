#ifndef _FD_H_
#define _FD_H_

#include <stdint.h>
//TODO implement the rest
struct fops{
	int32_t (*open) (uint8_t* path);
	int32_t (*write) (int32_t fd, uint8_t* buff, uint32_t size);
	int32_t (*read) (int32_t fd, uint8_t* buff, uint32_t size);
	int32_t (*close) (int32_t fd);
};

void setup_fd();
int32_t open(uint8_t* path);
int32_t write(int32_t fd, uint8_t* buff, uint32_t size);
int32_t read(int32_t fd, uint8_t* buff, uint32_t size);
int32_t close(int32_t fd);

#endif
