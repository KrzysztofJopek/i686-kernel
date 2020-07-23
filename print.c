#include "print.h"
#include "uart.h"

#define MAX_W 80
#define MAX_H 25
#define VIDEO_MEM 0xb8000
#define VIDEO_MEM_SIZE (MAX_W*MAX_H)


//XXX check in docs if blink bit exist
struct vid_col{
	unsigned char txt_col:4;
	unsigned char back_col:3;
	unsigned char blink:1;
};

struct vid_buf{
	unsigned char sym;
	struct vid_col col;
};

static struct vid_col curr_color;
static int set_color(char txt_col, char back_col)
{
	if(txt_col<COL_BLACK || txt_col>COL_WHITE){
		//wrong text color
		return 0;
	}
	if(back_col<COL_BLACK || back_col>COL_LIGHT_GRAY){
		//wrong background color
		return 0;
	}

	curr_color.txt_col = txt_col;
	curr_color.back_col = back_col;
	curr_color.blink = 0; // non blink
	return 1;
}

static int vid_pos = 0;
void print(const char* str)
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

void putc(const char c)
{
	char str[2] = {c, '\0'};
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

void log(const char* str)
{
	while(*str){
		uart_write(*str++, COM1_PORT);
	}
	uart_write('\n', COM1_PORT);
}
