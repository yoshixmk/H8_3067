#include "H8_register.h" /* H8/3067のレジスタ */
#include "RTL8019AS_register.h" /* RTL8019ASのレジスタ */
#include "packet_definition.h" /* パケットの定義 */
#include "global_variables.h" /* グローバル変数の定義 */
#include "H8_register_function.h" /* H8/3067のレジスタの設定 */
#include "delay_function.h" /* 時間待ちの関数 */
#include "LCD_function.h" /* SC1602Bの関数 */
#include "NIC_function.h" /* RTL8019ASの関数 */
#include "matrix_key_function.h" /* マトリクス・キーの関数 */
#include "utility_function.h" /* そのほかの関数 */
#include "ARP_function.h" /* ARPの関数 */
#include "speech_packet_function.h" /* 音声データのパケットの関数 */
#include <machine.h> /* set_imask_ccr関数 */

/* sampling_8khz関数は，タイマ割り込みが発生したときに呼び出される関数 */
#pragma interrupt(sampling_8khz)
void sampling_8khz(void)
{
   TISRA &= 0xFE; /* IMFA0を0に戻す */
   
   if (da_buffer_flag == 0) /* da_buffer_flagが0のとき */
   {
      DADR0 = da_buffer0[da_counter];
      /* da_buffer0に格納されている音声データをDA0に出力する */
   }
   else if (da_buffer_flag == 1) /* da_buffer_flagが1のとき */
   {
      DADR0 = da_buffer1[da_counter];
      /* da_buffer1に格納されている音声データをDA0に出力する */
   }
   
   da_counter++; /* da_counterをインクリメントする */
   if (da_counter == 160) /* da_counterが160になったとき */
   {
      da_buffer_flag ^= 1;
      /* XOR演算によって，da_buffer_flagが0のときは1，da_buffer_flagが1のときは0にする */
      
      da_counter = 0;
      /* da_counterを0に戻す */
   }
}

void main(void)
{
   unsigned short i;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   SPEECH_PACKET *speech_packet;
   
   set_imask_ccr(0); /* H8/3067を割り込みが受け付けられる状態にする */
   
   H8_register_init(); /* H8/3067のレジスタの設定 */
   LCD_init(); /* SC1602Bの初期化 */
   NIC_init(); /* RTL8019ASの初期化 */
   
   /* 送信元IPアドレスの設定 */
   src_IP[0] = 192;
   src_IP[1] = 168;
   src_IP[2] = 0;
   src_IP[3] = 100 + (P2DR & 0x07); /* ディップ・スイッチによって送信元IPアドレスを設定する */
   
   src_port = 20000; /* 送信元ポート番号の設定 */
   
   LCD_clear(); /* 表示クリア */
   LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
   
   da_buffer_flag = 0; /* da_buffer_flagの初期値 */
   da_counter = 0; /* da_counterの初期値 */
   for (i = 0; i < 160; i++)
   {
      da_buffer0[i] = 128; /* da_buffer0の初期値（音声データのオフセットは128） */
      da_buffer1[i] = 128; /* da_buffer1の初期値（音声データのオフセットは128） */
   }
   
   TISRA |= 0x10; /* IMIEA0を1に設定し，タイマ割り込みを有効にする */
   TSTR |= 0x01; /* STR0を1にしてタイマ0をスタートさせる */
   
   while (1) /* 無限ループ */
   {
      packet_type = 0; /* packet_typeを0にする */
      
      if (packet_receive(packet) != 1) /* パケットを受信したとき */
      {
         arp_packet = (ARP_PACKET *)packet; /* packetをARP_PACKET構造体に当てはめる */
         if ((arp_packet -> eth_ethernet_type == 0x0806) && 
             /* ARPのパケットのとき */
             (IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0) && 
             /* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
             (arp_packet -> arp_operation == 1)
             /* ARPリクエストのとき */
            )
         {
            packet_type = 'a'; /* packet_typeを'a'にする */
         }
         
         speech_packet = (SPEECH_PACKET *)packet; /* packetをSPEECH_PACKET構造体に当てはめる */
         if ((speech_packet -> eth_ethernet_type == 0x0800) && 
             /* IPのパケットのとき */
             (IP_compare(speech_packet -> ip_dst_IP, src_IP) == 0) && 
             /* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
             (speech_packet -> ip_protocol == 17) && 
             /* UDPのパケットのとき */
             (speech_packet -> udp_dst_port == src_port) && 
             /* パケットに記述されている宛先ポート番号が送信元ポート番号に一致したとき */
             (speech_packet_error_check(packet) != 1)
             /* パケットにエラーがないとき */
            )
         {
            packet_type = 's'; /* packet_typeを's'にする */
         }
      }
      
      switch (packet_type) /* packet_typeにしたがって処理を行う */
      {
         case 'a': /* packet_typeが'a'のとき */
            ARP_reply(packet); /* ARPリプライ */
            break;
         case 's': /* packet_typeが's'のとき */
            speech_packet = (SPEECH_PACKET *)packet; /* packetをSPEECH_PACKET構造体に当てはめる */
            if (da_buffer_flag == 0) /* da_buffer_flagが0のとき */
            {
               for (i = 0; i < 160; i++)
               {
                  da_buffer1[i] = speech_packet -> speech_data[i];
                  /* パケットに搭載されている音声データをda_buffer1に格納する */
               }
            }
            else if (da_buffer_flag == 1) /* da_buffer_flagが1のとき */
            {
               for (i = 0; i < 160; i++)
               {
                  da_buffer0[i] = speech_packet -> speech_data[i];
                  /* パケットに搭載されている音声データをda_buffer0に格納する */
               }
            }
            break;
      }
   }
}
