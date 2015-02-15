#include <stdio.h>
#include <stdlib.h>

void printWord( char in1, char in2){
	int i,j;
	char write_str[10][11] ={"ｱｲｳｴｵABCDE"
							,"ｶｷｸｹｺFGHIJ"
							,"ｻｼｽｾｿKLMNO"
							,"ﾀﾁﾂﾃﾄPQRST"
							,"ﾅﾆﾇﾈﾉUVWXY"
							,"ﾊﾋﾌﾍﾎZ?!-/"
							,"ﾏﾐﾑﾒﾓ\&$$$"
							,"ﾔ(ﾕ)ﾖ*#$$$"
							,"ﾗﾘﾙﾚﾛ12345"
							,"ﾜｦﾝﾞﾟ67890"};
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
	/*クライアント側で使う*/
	printf("%d : %d - 文字\n" , '0' , '1');
	for(i=0; i < 256; i++){
		c = getchar();
		if(c == '\n'){
			enter_count++;
			i--; /*改行文字は含めないため*/
		}
		else if(c < 48 || 57 < c){
			i--; /*1-9以外は含めないため*/
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
	
	/*送信データからサーバ側で使う*/
	for(i=0; i<16; i+=2){
		if(text_buffer[i] != '\0')
			insert1 = text_buffer[i];
		else break;
		if(text_buffer[i+1] != '\0')
			insert2 = text_buffer[i+1];
		else break;
		/*数字に変更*/
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