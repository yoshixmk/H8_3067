#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h> /* Windowsの基本関数 */
#include <mmsystem.h> /* マルチメディア関数 */
#include <conio.h> /* コンソール関数 */

#define NUMBER_OF_AD_BUFFER 8 /* 入力バッファの数 */
#define AD_BUFFER_SIZE 160 /* 入力バッファのサイズ */

#define NUMBER_OF_DA_BUFFER 8 /* 出力バッファの数 */
#define DA_BUFFER_SIZE 160 /* 出力バッファのサイズ */

int main(void)
{
   char key;
   unsigned short i;
   
   unsigned char speech_data[160]; /* 音声データ（160サンプル） */
   unsigned short flag; /* 音声データの出力を許可するためのフラグ */
   
   /* 音声データのフォーマット */
   WAVEFORMATEX wave_format_ex = {WAVE_FORMAT_PCM, /* 符号化方式（PCM） */
                                  1, /* モノラル */
                                  8000, /* 標本化周波数（8000Hz） */
                                  8000, /*1秒あたりの音声データのサイズ（8000byte） */
                                  1, /* 音声データの最小単位（1byte） */
                                  8, /* 量子化精度（8bit） */
                                  0 /* オプション情報のサイズ（0byte） */
                                 };
   
   WAVEHDR ad_wave_hdr[NUMBER_OF_AD_BUFFER]; /* 入力バッファのヘッダ */
   HWAVEIN wave_in = 0; /* 入力デバイスのハンドル */
   unsigned char ad_buffer[NUMBER_OF_AD_BUFFER][AD_BUFFER_SIZE]; /* 入力バッファ */
   unsigned short ad_index0; /* 入力バッファのインデックス */
   unsigned short ad_index1; /* 入力バッファのインデックス */
   
   WAVEHDR da_wave_hdr[NUMBER_OF_DA_BUFFER]; /* 出力バッファのヘッダ */
   HWAVEOUT wave_out = 0; /* 出力デバイスのハンドル */
   unsigned char da_buffer[NUMBER_OF_DA_BUFFER][DA_BUFFER_SIZE]; /* 出力バッファ */
   unsigned short da_index0; /* 出力バッファのインデックス */
   unsigned short da_index1; /* 出力バッファのインデックス */
   
   /* 入力デバイスのオープン */
   waveInOpen(&wave_in, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
   
   /* 0を書き込むことでad_wave_hdrをクリアする */
   for (ad_index0 = 0; ad_index0 < NUMBER_OF_AD_BUFFER; ad_index0++)
   {
      memset(&ad_wave_hdr[ad_index0], 0, sizeof(WAVEHDR));
   }
   
   /* 出力デバイスのオープン */
   waveOutOpen(&wave_out, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
   
   /* 出力デバイスの一時停止 */
   waveOutPause(wave_out);
   
   /* 0を書き込むことでda_wave_hdrをクリアする */
   for (da_index0 = 0; da_index0 < NUMBER_OF_DA_BUFFER; da_index0++)
   {
      memset(&da_wave_hdr[da_index0], 0, sizeof(WAVEHDR));
   }
   
   ad_index0 = 0; /* ad_index0の初期値 */
   ad_index1 = 0; /* ad_index1の初期値 */
   
   da_index0 = 0; /* da_index0の初期値 */
   da_index1 = 0; /* da_index1の初期値 */
   
   flag = 0; /* flagの初期値 */
   
   while (1) /* 無限ループ */
   {
      if (flag == 1) /* flagが1のとき */
      {
         /* 音声データの出力 */
         if (da_index0 < NUMBER_OF_DA_BUFFER)
         {
            /* 出力バッファに対する音声データの書き込み */
            for (i = 0; i < DA_BUFFER_SIZE; i++)
            {
               da_buffer[da_index0][i] = speech_data[i];
            }
            
            flag = 0; /* flagを0に戻す */
            
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
               
               flag = 0; /* flagを0に戻す */
               
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
            
            flag = 1; /* flagを1にする */
            
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
            
            return 0;
         }
      }
      
      Sleep(1); /* 1msのスリープをはさむことで，プログラムの実行速度を調節する */
   }
}
