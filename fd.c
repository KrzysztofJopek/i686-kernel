#include "fd.h"
//todo add global NULL macro
#define NULL (void*)0;

//TODO handle error codes
struct fd{
	int32_t fd;
	struct fops* fops;
};

#define MAX_FD 100
struct fd fds[MAX_FD];

//TODO make register API
extern struct fops* COM_fops;

static struct fd* get_fdp(int32_t fd)
{
	if(fd<0 || fd > MAX_FD){
		return NULL;
	}
	return fds+fd;
}

static uint32_t find_free_pos()
{
	for(int i=0; i<MAX_FD; i++){
		if(fds[i].fd == -1){
			return i;
		}
	}
	return -1;
}

static void fds_add(struct fd* global_fd, int32_t local_fd, struct fops* fops)
{
	global_fd->fd = local_fd;
	global_fd->fops = fops;
}

static void fds_del(struct fd* global_fd)
{
	global_fd->fd = -1;
	global_fd->fops = NULL;
}

void setup_fd()
{
	for(int i=0; i<MAX_FD; i++){
		fds[i].fd = -1;
	}
}

int32_t open(uint8_t* path)
{
	if(!path){
		return -1;
	}
	int32_t fd = find_free_pos();
	if(fd == -1){
		//Out of free fds
		return -2;
	}
	
	//TODO implement strcmp
	//XXX "C1" string == COM1
	if(path[0] == 'C' && path[1] == '1' && path[2] == '\0'){
		int32_t local_fd = COM_fops->open(path);
		struct fd* fdp = get_fdp(fd);
		fdp->fd = local_fd;
		fdp->fops = COM_fops;
		return fd;
	}
	return -3;
}

int32_t write(int32_t fd, uint8_t* buff, uint32_t size)
{
	struct fd* fdp = get_fdp(fd);
	if(!fdp){
		return -1;
	}
	return fdp->fops->write(fd, buff, size);
}

int32_t read(int32_t fd, uint8_t* buff, uint32_t size)
{
	struct fd* fdp = get_fdp(fd);
	if(!fdp){
		return -1;
	}
	return fdp->fops->read(fd, buff, size);
}

int32_t close(int32_t fd)
{
	struct fd* fdp = get_fdp(fd);
	if(!fdp){
		return -1;
	}
	int32_t res = fdp->fops->close(fd);
	fds_del(fdp);
	return res;
}

