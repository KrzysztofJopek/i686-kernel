#define MAX_W 25
#define MAX_H 80
#define VIDEO_MEM 0xb8000
#define VIDEO_MEM_SIZE (MAX_W*MAX_H*2)

void kmain()
{
	char *vidp = (char*)VIDEO_MEM;
	int i=0;
	while(i<VIDEO_MEM_SIZE){
		vidp[i++] = ' ';
		vidp[i++] = 0x01;
	}

	const char* str = "JOPEK";
	i=0;
	while(*str){
		vidp[i] = *str++;
		i+=2;
	}
	return;
}
