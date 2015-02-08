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

typedef struct
{
   /* 呼制御メッセージ（2byte） */
   unsigned short call_type;
} CALL_PACKET;

typedef struct
{
   /* RTPヘッダ（12byte） */
   unsigned char rtp_v_p_x_cc;
   unsigned char rtp_m_payload_type;
   unsigned short rtp_sequence_number;
   unsigned long rtp_time_stamp;
   unsigned long rtp_ssrc;
   
   /* 音声データ（160byte） */
   unsigned char speech_data[160];
} SPEECH_PACKET;

int main(void)
{
   WSADATA wsa_data;
   SOCKET s10000; /* 呼制御データの送受信のためのソケット */
   SOCKET s20000; /* 音声データの送受信のためのソケット */
   SOCKADDR_IN src0; /* この端末のソケット・アドレス（呼制御データ） */
   SOCKADDR_IN src1; /* この端末のソケット・アドレス（音声データ） */
   SOCKADDR_IN dst0; /* 相手先の端末のソケット・アドレス（呼制御データ） */
   SOCKADDR_IN dst1; /* 相手先の端末のソケット・アドレス（音声データ） */
   char key;
   unsigned short i;
   unsigned short phone_state;
   unsigned short cursor;
   unsigned long argp;
   int dst_size;
   char str_IP[16];
   time_t initial_time;
   time_t reference_time;
   time_t current_time;
   CALL_PACKET call_packet;
   SPEECH_PACKET speech_packet;
   unsigned short src_rtp_sequence_number;
   unsigned long src_rtp_time_stamp;
   unsigned long src_rtp_ssrc;
   
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
   
   /* Winsockの初期化 */
   WSAStartup(MAKEWORD(2, 0), /* WinSockバージョン2.0 */
              &wsa_data
             );
   
   /* 呼制御データの送受信のためのソケット */
   s10000 = socket(AF_INET, SOCK_DGRAM, 0); /* ソケットの作成 */
   memset(&src0, 0, sizeof(src0)); /* 0を書き込むことでsrc0をクリアする */
   src0.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
   src0.sin_addr.s_addr = htonl(INADDR_ANY); /* あらゆるネットワーク機器からの接続を許可する */
   src0.sin_port = htons(10000); /* ポート番号を10000番に設定する */
   bind(s10000, (LPSOCKADDR)&src0, sizeof(src0)); /* s10000にポート番号を設定する */
   argp = 1;
   ioctlsocket(s10000, FIONBIO, &argp); /* s10000のブロッキングを解除する */
   
   /* 音声データの送受信のためのソケット */
   s20000 = socket(AF_INET, SOCK_DGRAM, 0); /* ソケットの作成 */
   memset(&src1, 0, sizeof(src1)); /* 0を書き込むことでsrc1をクリアする */
   src1.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
   src1.sin_addr.s_addr = htonl(INADDR_ANY); /* あらゆるネットワーク機器からの接続を許可する */
   src1.sin_port = htons(20000); /* ポート番号を20000番に設定する */
   bind(s20000, (LPSOCKADDR)&src1, sizeof(src1)); /* s20000にポート番号を設定する */
   argp = 1;
   ioctlsocket(s20000, FIONBIO, &argp); /* s20000のブロッキングを解除する */
   
   /* オフラインの状態に遷移する */
   printf("Call To:");
   cursor = 0;
   memset(str_IP, 0, sizeof(str_IP));
   phone_state = OFFLINE; /* phone_stateをOFFLINEにする */
   
   while (1) /* 無限ループ */
   {
      switch (phone_state) /* phone_stateにしたがって処理を行う */
      {
         case OFFLINE : /* オフラインの状態 */
            dst_size = sizeof(dst0);
            if (recvfrom(s10000, (char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, &dst_size) > 0)
            /* 呼制御データのパケットを受信したとき */
            {
               if (ntohs(call_packet.call_type) == CONNECT_REQUEST) /* 接続リクエストのとき */
               {
                  printf("\n");
                  printf("Call From:%s\n", inet_ntoa(dst0.sin_addr));
                  initial_time = time(NULL); /* 現在の時刻を初期時刻とする */
                  reference_time = initial_time; /* 基準時刻 */
                  phone_state = INCOMING; /* phone_stateをINCOMINGにする */
               }
            }
            
            if (kbhit()) /* キーボードからの入力がある場合 */
            {
               key = getch(); /* キーのチェック */
               if (key == 13) /* Enterキーが押されたとき */
               {
                  str_IP[cursor] = '\0';
                  printf("\n");
                  
                  /* 送信した文字列が"end"であれば，アプリケーションを終了する */
                  if (strcmp(str_IP, "end") == 0)
                  {
                     closesocket(s10000); /* ソケットのクローズ */
                     closesocket(s20000); /* ソケットのクローズ */
                     WSACleanup(); /* WinSockのリソースを解放する */
                     return 0;
                  }
                  
                  memset(&dst0, 0, sizeof(dst0)); /* 0を書き込むことでdst0をクリアする */
                  dst0.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
                  dst0.sin_addr.s_addr = inet_addr(str_IP); /* 相手先の端末のIPアドレス */
                  dst0.sin_port = htons(10000); /* 相手先の端末のポート番号（10000番） */
                  
                  /* 接続リクエストを送信する */
                  memset(&call_packet, 0, sizeof(call_packet));
                  call_packet.call_type = htons(CONNECT_REQUEST);
                  sendto(s10000, (const char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, sizeof(dst0));
                  
                  initial_time = time(NULL); /* 現在の時刻を初期時刻とする */
                  reference_time = initial_time; /* 基準時刻 */
                  phone_state = OUTGOING; /* phone_stateをOUTGOINGにする */
               }
               else
               {
                  putch(key); /* keyを表示する */
                  str_IP[cursor] = key; /* keyをstr_IPに格納する */
                  cursor++; /* cursorをインクリメントする */
               }
            }
            Sleep(1); /* 1msのスリープをはさむことで，プログラムの実行速度を調節する */
            break;
         case ARP_REQUEST : /* ARPリクエストの状態 */
            break;
         case OUTGOING : /* 呼び出しの状態 */
            current_time = time(NULL); /* 現在の時刻 */
            if (current_time - initial_time < 8) /* 初期時刻から8秒以上経過していないとき */
            {
               if (current_time - reference_time > 1) /* 基準時刻から1秒以上経過したとき */
               {
                  putch('.'); /* ピリオドを表示する */
                  reference_time++; /* reference_timeをインクリメントする */
               }
               
               dst_size = sizeof(dst0);
               if (recvfrom(s10000, (char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, &dst_size) > 0)
               /* 呼制御データのパケットを受信したとき */
               {
                  if (ntohs(call_packet.call_type) == CONNECT_REPLY) /* 接続リプライのとき */
                  {
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
                     
                     src_rtp_sequence_number = 0; /* src_rtp_sequence_numberの初期値 */
                     src_rtp_time_stamp = 0; /* src_rtp_time_stampの初期値 */
                     src_rtp_ssrc = 0; /* src_rtp_ssrcの初期値 */
                     
                     printf("online\n");
                     
                     memset(&dst1, 0, sizeof(dst1)); /* 0を書き込むことでdst1をクリアする */
                     dst1.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
                     dst1.sin_addr.s_addr = dst0.sin_addr.s_addr; /* 相手先の端末のIPアドレス */
                     dst1.sin_port = htons(20000); /* 相手先の端末のポート番号（20000番） */
                     
                     phone_state = ONLINE; /* phone_stateをONLINEにする */
                  }
               }
            }
            else /* タイムアウト */
            {
               printf("time out\n");
               
               /* オフラインの状態に遷移する */
               printf("Call To:");
               cursor = 0;
               memset(str_IP, 0, sizeof(str_IP));
               phone_state = OFFLINE; /* phone_stateをOFFLINEにする */
            }
            Sleep(1); /* 1msのスリープをはさむことで，プログラムの実行速度を調節する */
            break;
         case INCOMING : /* 着信の状態 */
            current_time = time(NULL); /* 現在の時刻 */
            if (current_time - initial_time < 8) /* 初期時刻から8秒以上経過していないとき */
            {
               if (current_time - reference_time > 1) /* 基準時刻から1秒以上経過したとき */
               {
                  putch('.'); /* ピリオドを表示する */
                  reference_time++; /* reference_timeをインクリメントする */
               }
               
               if (kbhit()) /* キーボードからの入力がある場合 */
               {
                  key = getch(); /* キーのチェック */
                  if (key == 13) /* Enterキーが押されたとき */
                  {
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
                     
                     src_rtp_sequence_number = 0; /* src_rtp_sequence_numberの初期値 */
                     src_rtp_time_stamp = 0; /* src_rtp_time_stampの初期値 */
                     src_rtp_ssrc = 0; /* src_rtp_ssrcの初期値 */
                     
                     /* 接続リプライを送信する */
                     memset(&call_packet, 0, sizeof(call_packet));
                     call_packet.call_type = htons(CONNECT_REPLY);
                     sendto(s10000, (const char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, sizeof(dst0));
                     
                     printf("online\n");
                     
                     memset(&dst1, 0, sizeof(dst1)); /* 0を書き込むことでdst1をクリアする */
                     dst1.sin_family = AF_INET; /* インターネットのアドレス・ファミリ */
                     dst1.sin_addr.s_addr = dst0.sin_addr.s_addr; /* 相手先の端末のIPアドレス */
                     dst1.sin_port = htons(20000); /* 相手先の端末のポート番号（20000番） */
                     
                     phone_state = ONLINE; /* phone_stateをONLINEにする */
                  }
               }
            }
            else /* タイムアウト */
            {
               printf("time out\n");
               
               /* オフラインの状態に遷移する */
               printf("Call To:");
               cursor = 0;
               memset(str_IP, 0, sizeof(str_IP));
               phone_state = OFFLINE; /* phone_stateをOFFLINEにする */
            }
            Sleep(1); /* 1msのスリープをはさむことで，プログラムの実行速度を調節する */
            break;
         case ONLINE :
            dst_size = sizeof(dst0);
            if (recvfrom(s10000, (char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, &dst_size) > 0)
            /* 呼制御データのパケットを受信したとき */
            {
               if (ntohs(call_packet.call_type) == DISCONNECT_REQUEST) /* 切断リクエストのとき */
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
                  
                  /* 切断リプライを送信する */
                  memset(&call_packet, 0, sizeof(call_packet));
                  call_packet.call_type = htons(DISCONNECT_REPLY);
                  sendto(s10000, (const char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, sizeof(dst0));
                  
                  /* オフラインの状態に遷移する */
                  printf("Call To:");
                  cursor = 0;
                  memset(str_IP, 0, sizeof(str_IP));
                  phone_state = OFFLINE; /* phone_stateをOFFLINEにする */
               }
            }
            
            dst_size = sizeof(dst1);
            if (recvfrom(s20000, (char *)&speech_packet, 172, 0, (LPSOCKADDR)&dst1, &dst_size) > 0)
            /* 音声データのパケットを受信したとき */
            {
               /* 音声データの出力 */
               if (da_index0 < NUMBER_OF_DA_BUFFER)
               {
                  /* 出力バッファに対する音声データの書き込み */
                  for (i = 0; i < DA_BUFFER_SIZE; i++)
                  {
                     da_buffer[da_index0][i] = speech_packet.speech_data[i];
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
                        da_buffer[da_index1][i] = speech_packet.speech_data[i];
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
                  
                  /* 0を書き込むことでspeech_packetをクリアする */
                  memset(&speech_packet, 0, sizeof(speech_packet));
                  
                  /* RTPヘッダの作成 */
                  speech_packet.rtp_v_p_x_cc = 0x80;
                  speech_packet.rtp_m_payload_type = 0x14;
                  speech_packet.rtp_sequence_number = htons(src_rtp_sequence_number);
                  speech_packet.rtp_time_stamp = htonl(src_rtp_time_stamp);
                  speech_packet.rtp_ssrc = htonl(src_rtp_ssrc);
                  
                  /* 入力バッファからの音声データの読み取り */
                  for (i = 0; i < AD_BUFFER_SIZE; i++)
                  {
                     speech_packet.speech_data[i] = ad_wave_hdr[ad_index1].lpData[i];
                  }
                  
                  /* 音声データのパケットを送信する */
                  sendto(s20000, (const char *)&speech_packet, 172, 0, (LPSOCKADDR)&dst1, sizeof(dst1));
                  
                  src_rtp_sequence_number++; /* src_rtp_sequence_numberをインクリメントする */
                  src_rtp_time_stamp += AD_BUFFER_SIZE; /* src_rtp_time_stampにAD_BUFFER_SIZEを加算する */
                  
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
                  
                  /* 切断リクエストを送信する */
                  memset(&call_packet, 0, sizeof(call_packet));
                  call_packet.call_type = htons(DISCONNECT_REQUEST);
                  sendto(s10000, (const char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, sizeof(dst0));
                  
                  /* オフラインの状態に遷移する */
                  printf("Call To:");
                  cursor = 0;
                  memset(str_IP, 0, sizeof(str_IP));
                  phone_state = OFFLINE; /* phone_stateをOFFLINEにする */
               }
            }
            Sleep(1); /* 1msのスリープをはさむことで，プログラムの実行速度を調節する */
            break;
      }
   }
}
