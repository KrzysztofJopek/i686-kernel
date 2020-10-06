#include "fd.h"
#include "halloc.h"
#include "log.h"
#include "vm.h"
#include "syscall.h"
//todo add global NULL macro

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

static int32_t sys_read(int32_t fd, int32_t buff, int32_t size);
static int32_t sys_write(int32_t fd, int32_t buff, int32_t size);
static int32_t sys_open(int32_t path);
static int32_t sys_close(int32_t fd);
void setup_fd()
{
	for(int i=0; i<MAX_FD; i++){
		fds[i].fd = -1;
	}
	add_syscall(0, sys_read, 3);
	add_syscall(1, sys_write, 3);
	add_syscall(2, sys_open, 1);
	add_syscall(3, sys_close, 1);
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


static int32_t sys_open(int32_t path)
{
	return open((void*)path);
}

static int32_t sys_write(int32_t fd, int32_t buff, int32_t size)
{
	void* mem = halloc(size);
	if(!mem){
		LOG_ERR("Can't kalloc mem %d", size);
		panic();
	}
	copy_from_user(mem, (void*)buff, size);
	int32_t res = write(fd, mem, size);
	hfree(mem);
	return res;
}

static int32_t sys_read(int32_t fd, int32_t buff, int32_t size)
{
	void* mem = halloc(size);
	if(!mem){
		LOG_ERR("Can't kalloc mem %d", size);
		panic();
	}
	int32_t res = read(fd, mem, size);
	copy_to_user((void*)buff, mem, size);
	hfree(mem);
	return res;
}

static int32_t sys_close(int32_t fd)
{
	return close(fd);
}
