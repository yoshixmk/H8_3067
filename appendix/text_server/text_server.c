#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSockバージョン2.0 */

int main(void)
{
   WSADATA wsa_data;
   SOCKET src_socket; /* サーバのソケット */
   SOCKADDR_IN src; /* サーバのソケット・アドレス */
   SOCKADDR_IN dst; /* クライアントのソケット・アドレス */
   int dst_size;
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
   
   /* サーバのソケット・アドレス */
   memset(&src, 0, sizeof(src)); /* 0を書き込むことでsrcをクリアする */
   src.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
   src.sin_addr.s_addr = htonl(INADDR_ANY); /* あらゆるネットワーク機器からの接続を許可する */
   printf("server port:");
   gets(input_buffer); /* ユーザーによるポート番号の指定 */
   src.sin_port = htons(atoi(input_buffer)); /* サーバのポート番号 */
   
   /* ポート番号の設定 */
   bind(src_socket, (LPSOCKADDR)&src, sizeof(src)); /* src_socketにサーバのポート番号を設定する */
   
   while (1) /* 無限ループ */
   {
      /* テキスト・データのパケットの受信 */
      dst_size = sizeof(dst);
      recvfrom(src_socket, 
               text_data, /* テキスト・データ */
               16, /* テキスト・データのサイズ（16byte） */
               0, 
               (LPSOCKADDR)&dst, /* クライアントのソケット・アドレス */
               &dst_size
              );
      
      /* text_dataをtext_bufferにコピーする */
      for (i = 0; i < 16; i++)
      {
         text_buffer[i] = text_data[i];
      }
      text_buffer[16] = '\0'; /* テキスト・データの最後をナル文字にする */
      
      /* テキスト・データの表示 */
      printf("%s > %s\n", 
             inet_ntoa(dst.sin_addr), /* クライアントのIPアドレス */
             text_buffer /* テキスト・データ */
            );
      
      /* 受信した文字列が"0000000000000000"であれば，無限ループから抜ける */
      if (strcmp(text_buffer, "0000000000000000") == 0)
      {
         break;
      }
   }
   
   closesocket(src_socket); /* ソケットのクローズ */
   WSACleanup(); /* WinSockのリソースを解放する */
   
   return 0;
}
