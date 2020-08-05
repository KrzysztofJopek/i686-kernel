#include "halloc.h"
#include <stdio.h>


int main(int argc, char* argv[])
{
	int* p1 = (int*)halloc(100*sizeof(int));
	if(!p1)
		return 1;

	for(int i=1, *p=p1; i<=100; i++)
		*p++ = i;

	for(int i=1, *p=p1; i<=100; i++){
		printf("%d ", *p++);
		if(!(i%10))
			printf("\n");
	}

	hfree(p1);
	return 0;
}
