#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSockバージョン2.0 */
#include <windows.h> /* Windowsの基本関数 */
#include <mmsystem.h> /* マルチメディア関数 */
#include <conio.h> /* コンソール関数 */

#define NUMBER_OF_AD_BUFFER 8 /* 入力バッファの数 */
#define AD_BUFFER_SIZE 160 /* 入力バッファのサイズ */

int main(void)
{
   WSADATA wsa_data;
   SOCKET src_socket; /* クライアントのソケット */
   SOCKADDR_IN src; /* クライアントのソケット・アドレス */
   SOCKADDR_IN dst; /* サーバのソケット・アドレス */
   char key;
   unsigned short i;
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
   
   WAVEHDR ad_wave_hdr[NUMBER_OF_AD_BUFFER]; /* 出力バッファのヘッダ */
   HWAVEIN wave_in = 0; /* 出力デバイスのハンドル */
   unsigned char ad_buffer[NUMBER_OF_AD_BUFFER][AD_BUFFER_SIZE]; /* 出力バッファ */
   unsigned short ad_index0; /* 出力バッファのインデックス */
   unsigned short ad_index1; /* 出力バッファのインデックス */
   
   /* 入力デバイスのオープン */
   waveInOpen(&wave_in, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
   
   /* 0を書き込むことでad_wave_hdrをクリアする */
   for (ad_index0 = 0; ad_index0 < NUMBER_OF_AD_BUFFER; ad_index0++)
   {
      memset(&ad_wave_hdr[ad_index0], 0, sizeof(WAVEHDR));
   }
   
   ad_index0 = 0; /* ad_index0の初期値 */
   ad_index1 = 0; /* ad_index1の初期値 */
   
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
      /* 音声データの入力 */
      if (ad_index0 < NUMBER_OF_AD_BUFFER)
      {
         /* 入力バッファの設定 */
         ad_wave_hdr[ad_index0].lpData = ad_buffer[ad_index0]; /* 入力バッファの指定 */
         ad_wave_hdr[ad_index0].dwBufferLength = AD_BUFFER_SIZE; /* 入力バッファのサイズ */
         ad_wave_hdr[ad_index0].dwFlags = 0; /* フラグのクリア */
         
         /* 入力バッファのロック */
         waveInPrepareHeader(wave_in, &ad_wave_hdr[ad_index0], sizeof(WAVEHDR));
         
         /* 入力バッファを入力待ちキューに追加する */
         waveInAddBuffer(wave_in, &ad_wave_hdr[ad_index0], sizeof(WAVEHDR));
         
         ad_index0++; /* ad_index0のインクリメント */
         if (ad_index0 == NUMBER_OF_AD_BUFFER)
         {
            waveInStart(wave_in);
            /* すべての入力バッファが入力待ちキューに追加されたら音声データの入力を開始する */
         }
      }
      else
      {
         if ((ad_wave_hdr[ad_index1].dwFlags & WHDR_DONE) != 0)
         /* 入力バッファに対する音声データの入力が完了したとき */
         {
            /* 入力バッファのアンロック */
            waveInUnprepareHeader(wave_in, &ad_wave_hdr[ad_index1], sizeof(WAVEHDR));
            
            /* 入力バッファからの音声データの読み取り */
            for (i = 0; i < AD_BUFFER_SIZE; i++)
            {
               speech_data[i] = ad_wave_hdr[ad_index1].lpData[i];
            }
            
            /* 音声データのパケットを送信する */
            sendto(src_socket, speech_data, 160, 0, (LPSOCKADDR)&dst, sizeof(dst));
            
            /* 入力バッファの設定 */
            ad_wave_hdr[ad_index1].lpData = ad_buffer[ad_index1]; /* 入力バッファの指定 */
            ad_wave_hdr[ad_index1].dwBufferLength = AD_BUFFER_SIZE; /* 入力バッファのサイズ */
            ad_wave_hdr[ad_index1].dwFlags = 0; /* フラグのクリア */
            
            /* 入力バッファのロック */
            waveInPrepareHeader(wave_in, &ad_wave_hdr[ad_index1], sizeof(WAVEHDR));
            
            /* 入力バッファを入力待ちキューに追加する */
            waveInAddBuffer(wave_in, &ad_wave_hdr[ad_index1], sizeof(WAVEHDR));
            
            ad_index1++; /* ad_index1のインクリメント */
            if (ad_index1 == NUMBER_OF_AD_BUFFER)
            {
               ad_index1 = 0;
               /* ad_index1を0に戻し，入力バッファを再利用する */
            }
         }
      }
      
      if (kbhit()) /* キーボードからの入力がある場合 */
      {
         key = getch(); /* キーのチェック */
         if (key == 13) /* Enterキーが押されたとき */
         {
            waveInStop(wave_in); /* 音声データの入力を停止する */
            for (ad_index0 = 0; ad_index0 < NUMBER_OF_AD_BUFFER; ad_index0++)
            {
               /* 入力バッファのアンロック */
               if (ad_wave_hdr[ad_index0].dwFlags & WHDR_PREPARED)
               {
                  waveInUnprepareHeader(wave_in, &ad_wave_hdr[ad_index0], sizeof(WAVEHDR ));
               }
            }
            waveInClose(wave_in); /* 入力デバイスのクローズ */
            
            break;
         }
      }
      
      Sleep(1); /* 1msのスリープをはさむことで，プログラムの実行速度を調節する */
   }
   
   closesocket(src_socket); /* ソケットのクローズ */
   WSACleanup(); /* WinSockのリソースを解放する */
   
   return 0;
}
