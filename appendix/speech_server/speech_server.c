#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSockバージョン2.0 */
#include <windows.h> /* Windowsの基本関数 */
#include <mmsystem.h> /* マルチメディア関数 */
#include <conio.h> /* コンソール関数 */

#define NUMBER_OF_DA_BUFFER 8 /* 出力バッファの数 */
#define DA_BUFFER_SIZE 160 /* 出力バッファのサイズ */

int main(void)
{
   WSADATA wsa_data;
   SOCKET src_socket; /* サーバのソケット */
   SOCKADDR_IN src; /* サーバのソケット・アドレス */
   SOCKADDR_IN dst; /* クライアントのソケット・アドレス */
   char key;
   unsigned short i;
   unsigned long argp;
   int dst_size;
   char input_buffer[256];
   char speech_data[160]; /* 音声データ（160サンプル） */
   
   /* 音声データのフォーマット */
   WAVEFORMATEX wave_format_ex = {WAVE_FORMAT_PCM, /* 符号化方式（PCM） */
                                  1, /* モノラル */
                                  8000, /* 標本化周波数（8000Hz） */
                                  8000, /*1秒あたりの音声データのサイズ（8000byte） */
                                  1, /* 音声データの最小単位（1byte） */
                                  8, /* 量子化精度（8bit） */
                                  0 /* オプション情報のサイズ（0byte） */
                                 };
   
   WAVEHDR da_wave_hdr[NUMBER_OF_DA_BUFFER]; /* 出力バッファのヘッダ */
   HWAVEOUT wave_out = 0; /* 出力デバイスのハンドル */
   unsigned char da_buffer[NUMBER_OF_DA_BUFFER][DA_BUFFER_SIZE]; /* 出力バッファ */
   unsigned short da_index0; /* 出力バッファのインデックス */
   unsigned short da_index1; /* 出力バッファのインデックス */
   
   /* 出力デバイスのオープン */
   waveOutOpen(&wave_out, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
   
   /* 出力デバイスの一時停止 */
   waveOutPause(wave_out);
   
   /* 0を書き込むことでda_wave_hdrをクリアする */
   for (da_index0 = 0; da_index0 < NUMBER_OF_DA_BUFFER; da_index0++)
   {
      memset(&da_wave_hdr[da_index0], 0, sizeof(WAVEHDR));
   }
   
   da_index0 = 0; /* da_index0の初期値 */
   da_index1 = 0; /* da_index1の初期値 */
   
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
   
   /* ソケットのブロッキングを解除する */
   argp = 1;
   ioctlsocket(src_socket, FIONBIO, &argp);
   
   while (1) /* 無限ループ */
   {
      dst_size = sizeof(dst);
      if (recvfrom(src_socket, speech_data, 160, 0, (LPSOCKADDR)&dst, &dst_size) > 0)
      /* 音声データのパケットを受信したとき */
      {
         /* 音声データの出力 */
         if (da_index0 < NUMBER_OF_DA_BUFFER)
         {
            /* 出力バッファに対する音声データの書き込み */
            for (i = 0; i < DA_BUFFER_SIZE; i++)
            {
               da_buffer[da_index0][i] = speech_data[i];
            }
            
            /* 出力バッファの設定 */
            da_wave_hdr[da_index0].lpData = da_buffer[da_index0]; /* 出力バッファの指定 */
            da_wave_hdr[da_index0].dwBufferLength = DA_BUFFER_SIZE; /* 出力バッファのサイズ */
            da_wave_hdr[da_index0].dwFlags = 0; /* フラグのクリア */
            
            /* 出力バッファのロック */
            waveOutPrepareHeader(wave_out, &da_wave_hdr[da_index0], sizeof(WAVEHDR));
            
            /* 出力バッファを出力待ちキューに追加する */
            waveOutWrite(wave_out, &da_wave_hdr[da_index0], sizeof(WAVEHDR));
            
            da_index0++; /* da_index0のインクリメント */
            if (da_index0 == NUMBER_OF_DA_BUFFER)
            {
               waveOutRestart(wave_out);
               /* すべての出力バッファが出力待ちキューに追加されたら音声データの出力を開始する */
            }
         }
         else
         {
            if ((da_wave_hdr[da_index1].dwFlags & WHDR_DONE) != 0)
            /* 出力バッファからの音声データの出力が完了したとき */
            {
               /* 出力バッファに対する音声データの書き込み */
               for (i = 0; i < DA_BUFFER_SIZE; i++)
               {
                  da_buffer[da_index1][i] = speech_data[i];
               }
               
               /* 出力バッファのアンロック */
               waveOutUnprepareHeader(wave_out, &da_wave_hdr[da_index1], sizeof(WAVEHDR));
               
               /* 出力バッファの設定 */
               da_wave_hdr[da_index1].lpData = da_buffer[da_index1]; /* 出力バッファの指定 */
               da_wave_hdr[da_index1].dwBufferLength = DA_BUFFER_SIZE; /* 出力バッファのサイズ */
               da_wave_hdr[da_index1].dwFlags = 0; /* フラグのクリア */
               
               /* 出力バッファのロック */
               waveOutPrepareHeader(wave_out, &da_wave_hdr[da_index1], sizeof(WAVEHDR));
               
               /* 出力バッファを出力待ちキューに追加する */
               waveOutWrite(wave_out, &da_wave_hdr[da_index1], sizeof(WAVEHDR));
               
               da_index1++; /* da_index1のインクリメント */
               if (da_index1 == NUMBER_OF_DA_BUFFER)
               {
                  da_index1 = 0;
                  /* da_index1を0に戻し，出力バッファを再利用する */
               }
            }
         }
      }
      
      if (kbhit()) /* キーボードからの入力がある場合 */
      {
         key = getch(); /* キーのチェック */
         if (key == 13) /* Enterキーが押されたとき */
         {
            waveOutPause(wave_out); /* 音声データの出力を停止する */
            for (da_index0 = 0; da_index0 < NUMBER_OF_DA_BUFFER; da_index0++)
            {
               /* 出力バッファのアンロック */
               if (da_wave_hdr[da_index0].dwFlags & WHDR_PREPARED)
               {
                  waveOutUnprepareHeader(wave_out, &da_wave_hdr[da_index0], sizeof(WAVEHDR));
               }
            }
            waveOutClose(wave_out); /* 出力デバイスのクローズ */
            
            break;
         }
      }
      
      Sleep(1); /* 1msのスリープをはさむことで，プログラムの実行速度を調節する */
   }
   
   closesocket(src_socket); /* ソケットのクローズ */
   WSACleanup(); /* WinSockのリソースを解放する */
   
   return 0;
}
