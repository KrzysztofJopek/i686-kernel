#include "print.h"
#include "uart.h"
#include "fd.h"
#include "strutils.h"

#define MAX_W 80
#define MAX_H 25
#define VIDEO_MEM (0xC0000000 + 0xb8000)
#define VIDEO_MEM_SIZE (MAX_W*MAX_H)


//XXX check in docs if blink bit exist
struct vid_col{
	uint8_t txt_col:4;
	uint8_t back_col:3;
	uint8_t blink:1;
};

struct vid_buf{
	uint8_t sym;
	struct vid_col col;
};

static struct vid_col curr_color;
static int set_color(uint8_t txt_col, uint8_t back_col)
{
	if(txt_col>COL_WHITE){
		//wrong text color
		return 0;
	}
	if(back_col>COL_LIGHT_GRAY){
		//wrong background color
		return 0;
	}

	curr_color.txt_col = txt_col;
	curr_color.back_col = back_col;
	curr_color.blink = 0; // non blink
	return 1;
}

static int vid_pos = 0;
void print(const uint8_t* str)
{
	static struct vid_buf* vidp = (struct vid_buf*)VIDEO_MEM;
	while(vid_pos<VIDEO_MEM_SIZE && *str){
		if(*str == '\n'){
			vid_pos = (vid_pos/MAX_W + 1) * MAX_W;
		}
		else{
			vidp[vid_pos++].sym = *str;
		}
		str++;
	}
}

void putc(const uint8_t c)
{
	uint8_t str[2] = {c, '\0'};
	print(str);
}

void clear()
{
	static struct vid_buf* vidp = (struct vid_buf*)VIDEO_MEM;
	set_color(COL_WHITE, COL_BLACK);
	for(int i=0; i<VIDEO_MEM_SIZE; ++i){
		vidp[i].sym = ' ';
		vidp[i].col = curr_color;
	}
	vid_pos = 0;
}

#define MAX_INT_SIZE 30
void log(char* fmt, ...)
{
	static int32_t fd = -1;
	if(fd == -1){
		fd = open("C1");
	}

	if(!fmt){
		//log("log(NULL)");
		panic();
	}

	char buff[MAX_INT_SIZE];
	uint32_t res;
	char* str;
	uint32_t* argp = (uint32_t*)(void*)(&fmt+1);
	while(*fmt){
		if(*fmt != '%'){
			write(fd, (void*)fmt, 1);
			fmt++;
			continue;
		}
		fmt++;
		switch(*fmt){
			case 'd':
				res = itostr(*argp++, buff, 10);
				write(fd, buff, res-1);
				break;
			case 's':
				str = (char*)*argp++;
				if(!str){
					write(fd, "(null)", sizeof("null")-1);
				}
				else{
					write(fd, str, strlen(str)-1);
				}
				break;
			default:
				write(fd, "(unknown)", sizeof("unknown")-1);
				break;
		}
		fmt++;
	}
}
