#include <stdio.h>

int main(void)
{
	int i;
	char insert1, insert2;
	char text_buffer[256];
	char text[16][2];
	gets(text_buffer);
	for(i=0; i<16; i+=2){
		insert1 = text_buffer[i];
		insert2 = text_buffer[i+1];
		printf("%c%c\n",insert1,insert2);
	}
	return 0;
}