#include "fd.h"
#include "strutils.h"
#include "log.h"
#define MAX_FD 3 

struct file{
	uint8_t valid;
	uint8_t* filename[100];
	uint32_t size;
	uint8_t* data;
	uint8_t* ptr;
};

struct ustar_fs{
	uint8_t* archive;	
	uint8_t* archive_end;	
	struct file files[MAX_FD];
};


static struct ustar_fs curr_fs;
static int32_t ustar_fs_ready = 0;

static uint32_t oct2bin(uint8_t* str, uint32_t size)
{ uint32_t n = 0;
	uint8_t* c = str;
	while (size-- > 0) {
		n *= 8;
		n += *c - '0';
		c++;
	}
	return n;
}

static uint32_t find_free_pos()
{
	for(int i=0; i<MAX_FD; i++){
		if(curr_fs.files[i].valid == 0){
			return i;
		}
	}
	return -1;
}

static uint32_t find_file(uint8_t* path, uint8_t** res)
{
	*res = 0;
	uint8_t* pos = curr_fs.archive;
	while(pos < curr_fs.archive_end){
		uint32_t filesize = oct2bin(pos + 0x7c, 11);
		if(strcmp(pos, path) == 0){
			*res = pos;
			return filesize;
		}
		pos += (((filesize + 511) / 512) + 1) * 512;
	}
	return 0;
}

int32_t ustar_load(uint8_t* archive, uint8_t* archive_end)
{
	curr_fs.archive = archive;
	curr_fs.archive_end = archive_end;
	ustar_fs_ready = 1;
	return 0;
}


static int32_t ustar_open(uint8_t* path)
{
	if(!ustar_fs_ready){
		return -1;
	}

	int32_t fd = find_free_pos();
	if(fd == -1){
		return -2;
	}
	
	uint8_t* file_pos;
	uint32_t size = find_file(path, &file_pos);
	if(file_pos == NULL){
		return -3;
	}
	memcpy(curr_fs.files[fd].filename, path, 100);
	curr_fs.files[fd].valid = 1;
	curr_fs.files[fd].size = size;
	curr_fs.files[fd].data = file_pos+512;
	curr_fs.files[fd].ptr = file_pos+512;
	return fd;
}

static int32_t ustar_read(int32_t fd, uint8_t* buff, uint32_t count)
{
	if(!ustar_fs_ready){
		return -1;
	}
	if(fd < 0 && fd >= MAX_FD){
		return -2;
	}
	struct file* file = curr_fs.files + fd;
	if(file->valid == 0){
		return -3;
	}
	count = count > file->size ? file->size : count;
	memcpy(buff, file->data, count);
	return count;
}

static int32_t ustar_write(int32_t fd, uint8_t* buff, uint32_t count)
{
	if(!ustar_fs_ready){
		return -1;
	}
	if(fd < 0 && fd >= MAX_FD){
		return -2;
	}
	struct file* file = curr_fs.files + fd;
	if(file->valid == 0){
		return -3;
	}
	count = count > file->size ? file->size : count;
	memcpy(file->data, buff, count);
	return count;
}

static int32_t ustar_close(int32_t fd)
{
	if(!ustar_fs_ready){
		return -1;
	}
	if(fd < 0 && fd >= MAX_FD){
		return -2;
	}
	struct file* file = curr_fs.files + fd;
	if(file->valid == 0){
		return -3;
	}
	file->valid = 0;
	return 0;
}

static struct fops fops = {
	.open = ustar_open,
	.write = ustar_write,
	.read = ustar_read,
	.close = ustar_close
};

struct fops* USTAR_fops = &fops;
