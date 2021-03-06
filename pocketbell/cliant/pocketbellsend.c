#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSockバージョン2.0 */
#include <windows.h> /* Windowsの基本関数 */
#include <mmsystem.h> /* マルチメディア関数 */
#include <conio.h> /* コンソール関数 */
#include <time.h> /* 時間管理の関数 */

#define NUMBER_OF_AD_BUFFER 8 /* 入力バッファの数 */
#define AD_BUFFER_SIZE 160 /* 入力バッファのサイズ */

#define NUMBER_OF_DA_BUFFER 8 /* 出力バッファの数 */
#define DA_BUFFER_SIZE 160 /* 出力バッファのサイズ */

/* 端末の状態 */
#define OFFLINE            0 /* オフライン */
#define ARP_REQUEST        1 /* ARPリクエスト */
#define OUTGOING           2 /* 呼び出し */
#define INCOMING           3 /* 着信 */
#define ONLINE             4 /* オンライン */

/* 呼制御の種類 */
#define CONNECT_REQUEST    0 /* 接続リクエスト */
#define CONNECT_REPLY      1 /* 接続リプライ */
#define DISCONNECT_REQUEST 2 /* 切断リクエスト */
#define DISCONNECT_REPLY   3 /* 切断リプライ */


char printWord( char in1, char in2){
	char c;
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
	c = write_str[in1-1][in2-1];
	return c;
}

int main(void)
{
    WSADATA wsa_data;
    SOCKET src_socket; /* クライアントのソケット */
    SOCKADDR_IN src; /* クライアントのソケット・アドレス */
    SOCKADDR_IN dst; /* サーバのソケット・アドレス */
    int enter_count = 0;
    unsigned short i;
    char key, c;
    unsigned short cursor;
    unsigned long argp;
	char insert1, insert2;
	char text_data[17];
    char input_buffer[256];
    char text_buffer[256];
	char text[16][2];

    /* Winsockの初期化 *//* WinSockバージョン2.0 */
    WSAStartup(MAKEWORD(2, 0), &wsa_data);
    /* ソケットの作成 */
    src_socket = socket(AF_INET, SOCK_DGRAM,0);

    /* クライアントのソケット・アドレス */
    memset(&src, 0, sizeof(src)); /* 0を書き込むことでsrcをクリアする */
    src.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
    src.sin_addr.s_addr = htonl(INADDR_ANY); /* あらゆるネットワーク機器からの接続を許可する */
    src.sin_port = htons(20000); /* クライアントのポート番号 */

    /* ポート番号の設定 */
    bind(src_socket, (LPSOCKADDR)&src, sizeof(src)); /* src_socketにクライアントのポート番号を設定する */

    /* サーバのソケット・アドレス （初期化）*/
    memset(&dst, 0, sizeof(dst)); /* 0を書き込むことでdstをクリアする */
    dst.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */

    printf("POCKETBELL server IP address:");/*入力を促す*/
    while (1) /* 無限ループ */
    {
        if (kbhit()) /* キーボードからの入力がある場合 */
        {
           gets(input_buffer);
           printf("設定しました。%s\n", input_buffer);
           dst.sin_addr.s_addr = inet_addr(input_buffer); /* サーバのIPアドレス */
           printf("ポートは30000です。Enterを押してください。\n");
           dst.sin_port = htons(30000); /* サーバのポート番号 */
           key = getch(); /* キーのチェック */
           if (key == 13) /* Enterキーが押されたとき */
               printf("!\n");
               break;
        }
    }
          /* ポケベルへの送信データの入力 */
          printf("send text data:");

	for(i=0; i < 16; i++){
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
	for(i=0; i<32; i+=2){
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
		}
		text_data[i/2] = printWord(insert1, insert2);
	}
	
    /* テキスト・データのパケットの送信 */
    sendto(src_socket,
         text_data, /* テキスト・データ */
         16, /* テキスト・データのサイズ（16byte） */
         0,
         (LPSOCKADDR)&dst, /* サーバのソケット・アドレス */
         sizeof(dst)
    );
		
    return 0;
}
