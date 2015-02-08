#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSockバージョン2.0 */

int main(void)
{
   WSADATA wsa_data;
   SOCKET src_socket; /* クライアントのソケット */
   SOCKADDR_IN src; /* クライアントのソケット・アドレス */
   SOCKADDR_IN dst; /* サーバのソケット・アドレス */
   unsigned short i;
   char input_buffer[256];
   char text_data[16];
   char text_buffer[256];
   
   /* Winsockの初期化 */
   WSAStartup(MAKEWORD(2, 0), /* WinSockバージョン2.0 */
              &wsa_data
             );
   
   /* ソケットの作成 */
   src_socket = socket(AF_INET, /* インターネットのアドレス・ファミリ */
                       SOCK_DGRAM, /* データグラム型（UDP） */
                       0
                      );
   
   /* クライアントのソケット・アドレス */
   memset(&src, 0, sizeof(src)); /* 0を書き込むことでsrcをクリアする */
   src.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
   src.sin_addr.s_addr = htonl(INADDR_ANY); /* あらゆるネットワーク機器からの接続を許可する */
   src.sin_port = htons(10000); /* クライアントのポート番号 */
   
   /* ポート番号の設定 */
   bind(src_socket, (LPSOCKADDR)&src, sizeof(src)); /* src_socketにクライアントのポート番号を設定する */
   
   /* サーバのソケット・アドレス */
   memset(&dst, 0, sizeof(dst)); /* 0を書き込むことでdstをクリアする */
   dst.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
   printf("server IP address:");
   gets(input_buffer); /* ユーザーによるIPアドレスの指定 */
   dst.sin_addr.s_addr = inet_addr(input_buffer); /* サーバのIPアドレス */
   printf("server port:");
   gets(input_buffer); /* ユーザーによるポート番号の指定 */
   dst.sin_port = htons(atoi(input_buffer)); /* サーバのポート番号 */
   
   while (1) /* 無限ループ */
   {
      /* テキスト・データの入力 */
      printf("text data:");
      gets(text_buffer);
      
      /* text_bufferをtext_dataにコピーする */
      for (i = 0; i < 16; i++)
      {
         text_data[i] = ' ';
      }
      i = 0;
      while (i < 16)
      {
         if (text_buffer[i] == '\0')
         {
            break;
         }
         text_data[i] = text_buffer[i];
         i++;
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
   
   closesocket(src_socket); /* ソケットのクローズ */
   WSACleanup(); /* WinSockのリソースを解放する */
   
   return 0;
}
