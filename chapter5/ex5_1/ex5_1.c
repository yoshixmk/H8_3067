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
   char key, previous_key;
   unsigned char on_switch, previous_on_switch;
   unsigned char cursor, line;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   
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
   for (i = 0; i < 16; i++)
   {
      text_buffer[i] = ' '; /* テキスト・バッファを空白でクリアする */
   }
   text_buffer[16] = 0x00; /* テキスト・バッファの最後をナル文字にする */
   cursor = 0; /* cursorの初期値 */
   line = 0; /* lineの初期値 */
   previous_key = 0; /* previous_keyの初期値 */
   previous_on_switch = 1; /* previous_on_switchの初期値 */
   
   while (1) /* 無限ループ */
   {
      while (cursor < 16) /* cursorが16よりも小さいとき */
      {
         delay_ms(20);
         /* 20msの時間待ちをはさむことで，チャタリングによるスイッチの誤検出を回避する */
         
         key = matrix_key_read(); /* 現在のキーをチェックする */
         
         if ((key != 0) && (key != previous_key))
         /* 現在のキーが0ではなく，直前のキーとは異なるとき */
         {
            if ((cursor == 0) && (line == 0)) /* cursorが0，lineが0のとき */
            {
               LCD_clear(); /* 表示クリア */
               LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
            }
            else if ((cursor == 0) && (line == 1)) /* cursorが0，lineが1のとき */
            {
               LCD_control(0xC0); /* カーソルを1行目の先頭に移動する */
            }
            LCD_display(key); /* 液晶ディスプレイに現在のキーを表示する */
            text_buffer[cursor] = key; /* keyをtext_bufferに格納する */
            cursor++; /* cursorをインクリメントする */
         }
         previous_key = key;
         /* 現在のキーを，次のチェック時刻における直前のキーとして定義する */
         
         if ((P5DR & 0x02) == 0x00) /* 現在のONスイッチがオンのとき */
         {
            on_switch = 0; /* on_switchを0にする */
         }
         else if ((P5DR & 0x02) == 0x02) /* 現在のONスイッチがオフのとき */
         {
            on_switch = 1; /* on_switchを1にする */
         }
         
         if ((on_switch == 0) && (on_switch != previous_on_switch))
         /* 現在のONスイッチがオンで，直前のONスイッチの状態とは異なるとき */
         {
            text_packet_send(packet); /* テキスト・データのパケットを送信する */
            src_ip_id++; /* 次のパケットのために，src_ip_idをインクリメントする */
            for (i = 0; i < 16; i++)
            {
               text_buffer[i] = ' '; /* テキスト・バッファを空白でクリアする */
            }
            cursor = 0; /* cursorを0に戻す */
            line ^= 1; /* XOR演算によって，lineが0のときは1，lineが1のときは0にする */
         }
         previous_on_switch = on_switch;
         /* 現在のONスイッチの状態を，次のチェック時刻におけるONスイッチの状態として定義する */
      }
      
      /* cursorが16以上になったとき */
      text_packet_send(packet); /* テキスト・データのパケットを送信する */
      src_ip_id++; /* 次のパケットのために，src_ip_idをインクリメントする */
      for (i = 0; i < 16; i++)
      {
         text_buffer[i] = ' '; /* テキスト・バッファを空白でクリアする */
      }
      cursor = 0; /* cursorを0に戻す */
      line ^= 1; /* XOR演算によって，lineが0のときは1，lineが1のときは0にする */
   }
}
