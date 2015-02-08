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
#include "ping_function.h" /* pingの関数 */

void main(void)
{
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   PING_PACKET *ping_packet;
   
   H8_register_init(); /* H8/3067のレジスタの設定 */
   LCD_init(); /* SC1602Bの初期化 */
   NIC_init(); /* RTL8019ASの初期化 */
   
   /* 送信元IPアドレスの設定 */
   src_IP[0] = 192;
   src_IP[1] = 168;
   src_IP[2] = 0;
   src_IP[3] = 100 + (P2DR & 0x07); /* ディップ・スイッチによって送信元IPアドレスを設定する */
   
   LCD_clear(); /* 表示クリア */
   LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
   
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
         
         ping_packet = (PING_PACKET *)packet; /* packetをPING_PACKET構造体に当てはめる */
         if ((ping_packet -> eth_ethernet_type == 0x0800) && 
             /* IPのパケットのとき */
             (IP_compare(ping_packet -> ip_dst_IP, src_IP) == 0) && 
             /* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
             (ping_packet -> ip_protocol == 1) && 
             /* ICMPのパケットのとき */
             (ping_packet -> ping_type == 8)
             /* pingリクエストのとき */
            )
         {
            packet_type = 'p'; /* packet_typeを'p'にする */
         }
      }
      
      switch (packet_type) /* packet_typeにしたがって処理を行う */
      {
         case 'a': /* packet_typeが'a'のとき */
            LCD_display(packet_type); /* 液晶ディスプレイにpacket_typeを表示する */
            ARP_reply(packet); /* ARPリプライ */
            break;
         case 'p': /* packet_typeが'p'のとき */
            LCD_display(packet_type); /* 液晶ディスプレイにpacket_typeを表示する */
            ping_reply(packet); /* pingリプライ */
            break;
      }
   }
}
