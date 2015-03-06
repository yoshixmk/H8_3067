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
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   TEXT_PACKET *udp_packet;
   
   H8_register_init(); /* H8/3067のレジスタの設定 */
   LCD_init(); /* SC1602Bの初期化 */
   NIC_init(); /* RTL8019ASの初期化 */
   
   /* 送信元IPアドレスの設定 */
   src_IP[0] = 10;
   src_IP[1] = 1;
   src_IP[2] = 68;
   src_IP[3] = 139; /* ディップ・スイッチによって送信元IPアドレスを設定する */
   
   /* 宛先IPアドレスの設定 */
   dst_IP[0] = 10;
   dst_IP[1] = 1;
   dst_IP[2] = 68;
   dst_IP[3] = 149;
   
   src_port = 30000; /* 送信元ポート番号の設定 */
   dst_port = 20000; /* 宛先ポート番号の設定 */
   
   LCD_clear(); /* 表示クリア */
   LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
   
   delay_ms(2000); /* 2000ms（2秒）の時間待ち */
   
   /*ARP_request(packet);  ARPリクエスト */

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
             (arp_packet -> arp_operation == 1)
             /* ARPリクエストのとき */
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
   ARP_reply(packet);
   LCD_display('!');

   while (1) /* 無限ループ */
   {
       if((udp_packet -> eth_ethernet_type == 0x0800) && (IP_compare(udp_packet -> ip_dst_IP, src_IP) == 0) && (udp_packet -> udp_dst_port == 30000)){
                       UDP_text_receive(packet);
                       LCD_print(udp_packet -> text_data);
       }

   }
}
