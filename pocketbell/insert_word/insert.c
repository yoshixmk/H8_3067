#include <stdio.h>

int main(void)
{
	int i, enter_count = 0;
	char c;
	char insert1, insert2;
	char text_buffer[256];
	char text[16][2];
/*gets(text_buffer);*/
	for(i=0; i < 256; i++){
		c = getchar();
		if(c == '\n'){
			enter_count++;
			i--; /*‰üs•¶Žš‚ÍŠÜ‚ß‚È‚¢‚½‚ß*/
		}
		else{
			enter_count = 0;
			text_buffer[i] = c;
		}
		if(enter_count == 2){
			text_buffer[++i] = '\0';
			break;
		}
	}
	for(i=0; i<16; i+=2){
		if(text_buffer[i] != '\0')
			insert1 = text_buffer[i];
		else break;
		if(text_buffer[i+1] != '\0')
			insert2 = text_buffer[i+1];
		else break;
		printf("%i%c%c\n",i,insert1,insert2);
	}
	return 0;
}