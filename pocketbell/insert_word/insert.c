#include <stdio.h>
#include <stdlib.h>

void printWord( char in1, char in2){
	int i,j;
	char write_str[10][11] ={"�����ABCDE"
							,"�����FGHIJ"
							,"�����KLMNO"
							,"�����PQRST"
							,"�����UVWXY"
							,"�����Z?!-/"
							,"�����\&$$$"
							,"�(�)�*#$$$"
							,"�����12345"
							,"ܦ���67890"};
	putchar(write_str[in1-1][in2-1]);
	putchar('\n');
}

int main(void)
{
	int i, enter_count = 0;
	char c;
	char insert1, insert2;
	char text_buffer[256];
	char text[16][2];
	/*�N���C�A���g���Ŏg��*/
	printf("%d : %d - ����\n" , '0' , '1');
	for(i=0; i < 256; i++){
		c = getchar();
		if(c == '\n'){
			enter_count++;
			i--; /*���s�����͊܂߂Ȃ�����*/
		}
		else if(c < 48 || 57 < c){
			i--; /*1-9�ȊO�͊܂߂Ȃ�����*/
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
	
	/*���M�f�[�^����T�[�o���Ŏg��*/
	for(i=0; i<16; i+=2){
		if(text_buffer[i] != '\0')
			insert1 = text_buffer[i];
		else break;
		if(text_buffer[i+1] != '\0')
			insert2 = text_buffer[i+1];
		else break;
		/*�����ɕύX*/
		if(insert1 == 48){
			insert1 = 10;
		}
		else{
			insert1 = insert1 - 48;
		}
		if(insert2 == 48){
			insert2 = 10;
		}
		else{
			insert2 = insert2 - 48;
		}		/*printf("%d%d\n",insert1,insert2);*/
		
	}
	printWord(insert1, insert2);
	return 0;
}