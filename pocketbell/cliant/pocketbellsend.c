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

int main(void)
{
    WSADATA wsa_data;
    SOCKET src_socket; /* クライアントのソケット */
    SOCKADDR_IN src; /* クライアントのソケット・アドレス */
    SOCKADDR_IN dst; /* サーバのソケット・アドレス */
    unsigned short i;
    char key;
    unsigned short cursor;
    unsigned long argp;
    char input_buffer[256];
    char text_data[16];
    char text_buffer[256];

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

       while (1) /* 無限ループ */
       {
          /* ポケベルへの送信データの入力 */
          printf("send text data:");
          gets(text_buffer);
          /* 前のtext_dataを消去する */
          for (i = 0; i < 16; i++)
          {
             text_data[i] = ' ';
          }
          /* text_bufferをtext_dataにコピーする */
          for (i = 0; i < 16 && text_buffer[i] != '\0'; i++)
          {
             text_data[i] = text_buffer[i];
          }

          /* テキスト・データのパケットの送信 */
          sendto(src_socket,
                 text_data, /* テキスト・データ */
                 16, /* テキスト・データのサイズ（16byte） */
                 0,
                 (LPSOCKADDR)&dst, /* サーバのソケット・アドレス */
                 sizeof(dst)
                );

          /* 送信した文字列が"end"であれば，無限ループから抜ける */
          if (strcmp(text_buffer, "end") == 0)
          {
             break;
          }
       }

       return 0;
}
