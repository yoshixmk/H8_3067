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
#include "text_packet_function.h" /* テキスト・データのパケットの関数 */

void main(void)
{
   unsigned short i;
   unsigned char line;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   TEXT_PACKET *text_packet;
   
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
   
   text_buffer[16] = 0x00; /* テキスト・バッファの最後をナル文字にする */
   line = 0; /* lineの初期値 */
   
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
         
         text_packet = (TEXT_PACKET *)packet; /* packetをTEXT_PACKET構造体に当てはめる */
         if ((text_packet -> eth_ethernet_type == 0x0800) && 
             /* IPのパケットのとき */
             (IP_compare(text_packet -> ip_dst_IP, src_IP) == 0) && 
             /* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
             (text_packet -> ip_protocol == 17) && 
             /* UDPのパケットのとき */
             (text_packet -> udp_dst_port == src_port) && 
             /* パケットに記述されている宛先ポート番号が送信元ポート番号に一致したとき */
             (text_packet_error_check(packet) != 1)
             /* パケットにエラーがないとき */
            )
         {
            packet_type = 't'; /* packet_typeを't'にする */
         }
      }
      
      switch (packet_type) /* packet_typeにしたがって処理を行う */
      {
         case 'a': /* packet_typeが'a'のとき */
            ARP_reply(packet); /* ARPリプライ */
            break;
         case 't': /* packet_typeが't'のとき */
            text_packet = (TEXT_PACKET *)packet; /* packetをTEXT_PACKET構造体に当てはめる */
            for (i = 0; i < 16; i++)
            {
               text_buffer[i] = text_packet -> text_data[i];
               /* パケットに搭載されているテキスト・データをtext_bufferに格納する */
            }
            if (line == 0) /* lineが0のとき */
            {
               LCD_clear(); /* 表示クリア */
               LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
            }
            else if (line == 1) /* lineが1のとき */
            {
               LCD_control(0xC0); /* カーソルを1行目の先頭に移動する */
            }
            LCD_print(text_buffer); /* 液晶ディスプレイにテキスト・バッファの文字列を表示する */
            line ^= 1; /* XOR演算によって，lineが0のときは1，lineが1のときは0にする */
            break;
      }
   }
}
