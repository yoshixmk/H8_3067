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
   
   ADCSR |= 0x20; /* ADSTを1にしてA-D変換をスタートさせる */
   while ((ADCSR & 0x80) == 0x00) /* ADFが1になるのを待つ */
   {
      /* A-D変換が完了するとADFは1になる */
   }
   ADCSR &= 0x7F; /* ADFを0に戻す */
   
   if (ad_buffer_flag == 0) /* ad_buffer_flagが0のとき */
   {
      ad_buffer0[ad_counter] = (unsigned char)(ADDRA >> 8);
      /* ADDRAの上位8bitをad_buffer0に格納する */
   }
   else if (ad_buffer_flag == 1) /* ad_buffer_flagが1のとき */
   {
      ad_buffer1[ad_counter] = (unsigned char)(ADDRA >> 8);
      /* ADDRAの上位8bitをad_buffer1に格納する */
   }
   
   ad_counter++; /* ad_counterをインクリメントする */
   if (ad_counter == 160) /* ad_counterが160になったとき */
   {
      speech_packet_send_flag = 1;
      /* speech_packet_send_flagを1にする */
      
      ad_buffer_flag ^= 1;
      /* XOR演算によって，ad_buffer_flagが0のときは1，ad_buffer_flagが1のときは0にする */
      
      ad_counter = 0;
      /* ad_counterを0に戻す */
   }
}

void main(void)
{
   unsigned short i;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   
   set_imask_ccr(0); /* H8/3067を割り込みが受け付けられる状態にする */
   
   H8_register_init(); /* H8/3067のレジスタの設定 */
   LCD_init(); /* SC1602Bの初期化 */
   NIC_init(); /* RTL8019ASの初期化 */
   
   /* 送信元IPアドレスの設定 */
   src_IP[0] = 192;
   src_IP[1] = 168;
   src_IP[2] = 0;
   src_IP[3] = 100 + (P2DR & 0x07); /* ディップ・スイッチによって送信元IPアドレスを設定する */
   
   /* 宛先IPアドレスの設定 */
   dst_IP[0] = 192;
   dst_IP[1] = 168;
   dst_IP[2] = 0;
   dst_IP[3] = 2;
   
   src_port = 10000; /* 送信元ポート番号の設定 */
   dst_port = 20000; /* 宛先ポート番号の設定 */
   
   LCD_clear(); /* 表示クリア */
   LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
   
   delay_ms(2000); /* 2000ms（2秒）の時間待ち */
   
   ARP_request(packet); /* ARPリクエスト */
   
   packet_type = 0; /* packet_typeを0にする */
   do
   {
      if (packet_receive(packet) != 1) /* パケットを受信したとき */
      {
         arp_packet = (ARP_PACKET *)packet; /* packetをARP_PACKET構造体に当てはめる */
         if ((arp_packet -> eth_ethernet_type == 0x0806) && 
             /* ARPのパケットのとき */
             (IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0) && 
             /* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
             (arp_packet -> arp_operation == 2)
             /* ARPリプライのとき */
            )
         {
            for (i = 0; i < 6; i++)
            {
               dst_MAC[i] = arp_packet -> arp_src_MAC[i];
               /* パケットに記述されている送信元MACアドレスをdst_MACに格納する */
            }
            packet_type = 'a'; /* packet_typeを'a'にする */
         }
      }
   } while (packet_type != 'a'); /* packet_typeが'a'になるまで待つ */
   
   src_ip_id = 0; /* src_ip_idの初期値 */
   speech_packet_send_flag = 0; /* speech_packet_send_flagの初期値 */
   ad_buffer_flag = 0; /* ad_buffer_flagの初期値 */
   ad_counter = 0; /* ad_counterの初期値 */
   
   TISRA |= 0x10; /* IMIEA0を1に設定し，タイマ割り込みを有効にする */
   TSTR |= 0x01; /* STR0を1にしてタイマ0をスタートさせる */
   
   while (1) /* 無限ループ */
   {
      if (speech_packet_send_flag == 1) /* speech_packet_send_flagが1のとき */
      {
         speech_packet_send(packet); /* 音声データのパケットを送信する */
         src_ip_id++; /* 次のパケットのために，src_ip_idをインクリメントする */
         speech_packet_send_flag = 0; /* speech_packet_send_flagを0に戻す */
      }
   }
}
