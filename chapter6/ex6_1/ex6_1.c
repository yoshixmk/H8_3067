#include "H8_register.h" /* H8/3067のレジスタ */
#include "RTL8019AS_register.h" /* RTL8019ASのレジスタ */
#include "symbol_definition.h" /* 定数の定義 */
#include "packet_definition.h" /* パケットの定義 */
#include "global_variables.h" /* グローバル変数の定義 */
#include "H8_register_function.h" /* H8/3067のレジスタの設定 */
#include "delay_function.h" /* 時間待ちの関数 */
#include "LCD_function.h" /* SC1602Bの関数 */
#include "NIC_function.h" /* RTL8019ASの関数 */
#include "matrix_key_function.h" /* マトリクス・キーの関数 */
#include "utility_function.h" /* そのほかの関数 */
#include "ARP_function.h" /* ARPの関数 */
#include "call_packet_function.h" /* 呼制御データのパケットの関数 */
#include "speech_packet_function.h" /* 音声データのパケットの関数 */
#include <machine.h> /* set_imask_ccr関数 */

/* sampling_8khz関数は，タイマ割り込みが発生したときに呼び出される関数 */
#pragma interrupt(sampling_8khz)
void sampling_8khz(void)
{
   TISRA &= 0xFE; /* IMFA0を0に戻す */
   
   switch (phone_state) /* phone_stateにしたがって処理を行う */
   {
      case OFFLINE : /* オフラインの状態 */
         anti_chattering_counter++; /* anti_chattering_counterをインクリメントする */
         if (anti_chattering_counter == 160) /* anti_chattering_counterが160になったとき */
         {
            key = matrix_key_read(); /* 現在のキーをチェックする */
            if ((P5DR & 0x02) == 0x00) /* 現在のONスイッチがオンのとき */
            {
               on_switch = 0; /* on_switchを0にする */
            }
            else if ((P5DR & 0x02) == 0x02) /* 現在のONスイッチがオフのとき */
            {
               on_switch = 1; /* on_switchを1にする */
            }
            anti_chattering_counter = 0; /* anti_chattering_counterを0に戻す */
         }
         break;
      case ARP_REQUEST : /* ARPリクエストの状態 */
         time_out_counter++; /* time_out_counterをインクリメントする */
         break;
      case OUTGOING : /* 呼び出しの状態 */
         time_out_counter++; /* time_out_counterをインクリメントする */
         break;
      case INCOMING : /* 着信の状態 */
         time_out_counter++; /* time_out_counterをインクリメントする */
         if ((time_out_counter & 0x2000) == 0) /* (time_out_counter & 0x2000)が0のとき */
         {
            PADR = 0x7F;
            /* PA2は1（LED(D8)は発光しない），PA3は1（LED(D9)は発光しない），PA4は1（ブザーは5V） */
         }
         else /* (time_out_counter & 0x2000)が1のとき */
         {
            if ((time_out_counter & 0x0200) == 0) /* (time_out_counter & 0x0200)が0のとき */
            {
               if ((time_out_counter & 0x0008) == 0) /* (time_out_counter & 0x0008)が0のとき */
               {
                  PADR = 0x7B;
                  /* PA2は0（LED(D8)は発光する），PA3は1（LED(D9)は発光しない），PA4は1（ブザーは5V） */
               }
               else /* (time_out_counter & 0x0008)が1のとき */
               {
                  PADR = 0x6B;
                  /* PA2は0（LED(D8)は発光する），PA3は1（LED(D9)は発光しない），PA4は0（ブザーは0V） */
               }
            }
            else /* (time_out_counter & 0x0200)が1のとき */
            {
               PADR = 0x77;
               /* PA2は1（LED(D8)は発光しない），PA3は0（LED(D9)は発光する），PA4は1（ブザーは5V） */
            }
         }
         
         anti_chattering_counter++; /* anti_chattering_counterをインクリメントする */
         if (anti_chattering_counter == 160) /* anti_chattering_counterが160になったとき */
         {
            if ((P5DR & 0x02) == 0x00) /* 現在のONスイッチがオンのとき */
            {
               on_switch = 0; /* on_switchを0にする */
            }
            else if ((P5DR & 0x02) == 0x02) /* 現在のONスイッチがオフのとき */
            {
               on_switch = 1; /* on_switchを1にする */
            }
            anti_chattering_counter = 0; /* anti_chattering_counterを0に戻す */
         }
         break;
      case ONLINE : /* オンラインの状態 */
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
         
         anti_chattering_counter++; /* anti_chattering_counterをインクリメントする */
         if (anti_chattering_counter == 160) /* anti_chattering_counterが160になったとき */
         {
            if ((P5DR & 0x01) == 0x00) /* 現在のOFFスイッチがオンのとき */
            {
               off_switch = 0; /* off_switchを0にする */
            }
            else if ((P5DR & 0x01) == 0x01) /* 現在のOFFスイッチがオフのとき */
            {
               off_switch = 1; /* off_switchを1にする */
            }
            anti_chattering_counter = 0; /* anti_chattering_counterを0に戻す */
         }
         break;
   }
}

void OFFLINE_state(void)
{
   unsigned short i;
   char str_to[] = "Call To"; /* 液晶ディスプレイに表示する文字列 */
   
   phone_state = OFFLINE; /* phone_stateをOFFLINEにする */
   anti_chattering_counter = 0; /* anti_chattering_counterの初期値 */
   cursor = 0; /* cursorの初期値 */
   for (i = 0; i < 16; i++)
   {
      str_IP[i] = ' '; /* str_IPを空白でクリアする */
   }
   
   PADR = 0x7F; /* PA2は1（LED(D8)は発光しない），PA3は1（LED(D9)は発光しない），PA4は1（ブザーは5V） */
   LCD_clear(); /* 表示クリア */
   LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
   LCD_print(str_to); /* 液晶ディスプレイにstr_toを表示する */
   LCD_control(0xC0); /* カーソルを1行目の先頭に移動する */
}

void ARP_REQUEST_state(void)
{
   phone_state = ARP_REQUEST; /* phone_stateをARP_REQUESTにする */
   time_out_counter = 0; /* time_out_counterの初期値 */
}

void OUTGOING_state(void)
{
   phone_state = OUTGOING; /* phone_stateをOUTGOINGにする */
   time_out_counter = 0; /* time_out_counterの初期値 */
}

void INCOMING_state(void)
{
   char str_from[] = "Call From"; /* 液晶ディスプレイに表示する文字列 */
   
   phone_state = INCOMING; /* phone_stateをINCOMINGにする */
   anti_chattering_counter = 0; /* anti_chattering_counterの初期値 */
   time_out_counter = 0; /* time_out_counterの初期値 */
   
   LCD_clear(); /* 表示クリア */
   LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
   LCD_print(str_from); /* 液晶ディスプレイにstr_fromを表示する */
   LCD_control(0xC0); /* カーソルを1行目の先頭に移動する */
   IP_to_str(dst_IP, str_IP); /* dst_IPをstr_IP（文字列）に変換する */
   LCD_print(str_IP); /* 液晶ディスプレイにstr_IPを表示する */
}

void ONLINE_state(void)
{
   unsigned short i;
   
   phone_state = ONLINE; /* phone_stateをONLINEにする */
   anti_chattering_counter = 0; /* anti_chattering_counterの初期値 */
   time_out_counter = 0; /* time_out_counterの初期値 */
   speech_packet_send_flag = 0; /* speech_packet_send_flagの初期値 */
   ad_buffer_flag = 0; /* ad_buffer_flagの初期値 */
   da_buffer_flag = 0; /* da_buffer_flagの初期値 */
   ad_counter = 0; /* ad_counterの初期値 */
   da_counter = 0; /* da_counterの初期値 */
   for (i = 0; i < 160; i++)
   {
      da_buffer0[i] = 128; /* da_buffer0の初期値（音声データのオフセットは128） */
      da_buffer1[i] = 128; /* da_buffer1の初期値（音声データのオフセットは128） */
   }
   src_rtp_sequence_number = 0; /* src_rtp_sequence_numberの初期値 */
   src_rtp_time_stamp = 0; /* src_rtp_time_stampの初期値 */
   src_rtp_ssrc = 0; /* src_rtp_ssrcの初期値 */
   PADR = 0x7F; /* PA2は1（LED(D8)は発光しない），PA3は1（LED(D9)は発光しない），PA4は1（ブザーは5V） */
}

void main(void)
{
   unsigned short i;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   CALL_PACKET *call_packet;
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
   
   src_ip_id = 0; /* src_ip_idの初期値 */
   
   key = 0; /* keyの初期値 */
   previous_key = 0; /* previous_keyの初期値 */
   on_switch = 1; /* on_switchの初期値 */
   previous_on_switch = 1; /* previous_on_switchの初期値 */
   off_switch = 1; /* off_switchの初期値 */
   previous_off_switch = 1; /* previous_off_switchの初期値 */
   
   OFFLINE_state(); /* オフラインの状態に遷移する */
   
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
             (IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0)
             /* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
            )
         {
            packet_type = 'a'; /* packet_typeを'a'にする */
         }
         
         call_packet = (CALL_PACKET *)packet; /* packetをCALL_PACKET構造体に当てはめる */
         if ((call_packet -> eth_ethernet_type == 0x0800) && 
             /* IPのパケットのとき */
             (IP_compare(call_packet -> ip_dst_IP, src_IP) == 0) && 
             /* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
             (call_packet -> ip_protocol == 17) && 
             /* UDPのパケットのとき */
             (call_packet -> udp_dst_port == 10000) && 
             /* パケットに記述されている宛先ポート番号が10000番であるとき */
             (call_packet_error_check(packet) != 1)
             /* パケットにエラーがないとき */
            )
         {
            packet_type = 'c'; /* packet_typeを'c'にする */
         }
         
         speech_packet = (SPEECH_PACKET *)packet; /* packetをSPEECH_PACKET構造体に当てはめる */
         if ((speech_packet -> eth_ethernet_type == 0x0800) && 
             /* IPのパケットのとき */
             (IP_compare(speech_packet -> ip_dst_IP, src_IP) == 0) && 
             /* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
             (speech_packet -> ip_protocol == 17) && 
             /* UDPのパケットのとき */
             (speech_packet -> udp_dst_port == 20000) && 
             /* パケットに記述されている宛先ポート番号が20000番であるとき */
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
            arp_packet = (ARP_PACKET *)packet; /* packetをARP_PACKET構造体に当てはめる */
            switch (arp_packet -> arp_operation) /* arp_operationにしたがって処理を行う */
            {
               case 1 : /* ARPリクエストのとき */
                  ARP_reply(packet); /* ARPリプライ */
                  break;
               case 2 : /* ARPリプライのとき */
                  for (i = 0; i < 6; i++)
                  {
                     dst_MAC[i] = arp_packet -> arp_src_MAC[i];
                     /* パケットに記述されている送信元MACアドレスをdst_MACに格納する */
                  }
                  src_port = 10000; /* 送信元ポート番号の設定 */
                  dst_port = 10000; /* 宛先ポート番号の設定 */
                  call_packet_send(packet, CONNECT_REQUEST); /* 接続リクエストを送信する */
                  src_ip_id++; /* 次のパケットのために，src_ip_idをインクリメントする */
                  OUTGOING_state(); /* 呼び出しの状態に遷移する */
                  break;
            }
            break;
         case 'c': /* packet_typeが'c'のとき */
            call_packet = (CALL_PACKET *)packet; /* packetをCALL_PACKET構造体に当てはめる */
            switch (call_packet -> call_type) /* call_typeにしたがって処理を行う */
            {
               case CONNECT_REQUEST : /* 接続リクエストのとき */
                  for (i = 0; i < 4; i++)
                  {
                     dst_IP[i] = call_packet -> ip_src_IP[i];
                     /* パケットに記述されている送信元IPアドレスをdst_IPに格納する */
                  }
                  for (i = 0; i < 6; i++)
                  {
                     dst_MAC[i] = call_packet -> eth_src_MAC[i];
                     /* パケットに記述されている送信元MACアドレスをdst_MACに格納する */
                  }
                  INCOMING_state(); /* 着信の状態に遷移する */
                  break;
               case CONNECT_REPLY : /* 接続リプライのとき */
                  ONLINE_state(); /* オンラインの状態に遷移する */
                  break;
               case DISCONNECT_REQUEST : /* 切断リクエストのとき */
                  src_port = 10000; /* 送信元ポート番号の設定 */
                  dst_port = 10000; /* 宛先ポート番号の設定 */
                  call_packet_send(packet, DISCONNECT_REPLY); /* 切断リプライを送信する */
                  src_ip_id++; /* 次のパケットのために，src_ip_idをインクリメントする */
                  OFFLINE_state(); /* オフラインの状態に遷移する */
                  break;
               case DISCONNECT_REPLY : /* 切断リプライのとき */
                  break;
            }
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
      
      switch (phone_state) /* phone_stateにしたがって処理を行う */
      {
         case OFFLINE : /* オフラインの状態 */
            if ((key != 0) && (key != previous_key))
            /* 現在のキーが0ではなく，直前のキーとは異なるとき */
            {
               if ((key == '#') || (cursor >= 16))
               /* 現在のキーが'#'，または，cursorが16以上のとき */
               {
                  OFFLINE_state(); /* オフラインの状態に遷移する */
               }
               else
               {
                  LCD_display(key); /* 液晶ディスプレイに現在のキーを表示する */
                  str_IP[cursor] = key; /* keyをstr_IPに格納する */
                  cursor++; /* cursorをインクリメントする */
               }
            }
            previous_key = key;
            /* 現在のキーを，次のチェック時刻における直前のキーとして定義する */
            
            if ((on_switch == 0) && (on_switch != previous_on_switch))
            /* 現在のONスイッチがオンで，直前のONスイッチの状態とは異なるとき */
            {
               if (str_to_IP(str_IP, dst_IP) != 1)
               /* str_IP（IPアドレスの文字列）をdst_IPに正しく変換できたとき */
               {
                  ARP_request(packet); /* ARPリクエスト */
                  ARP_REQUEST_state(); /* ARPリクエストの状態に遷移する */
               }
               else
               {
                  OFFLINE_state(); /* オフラインの状態に遷移する */
               }
            }
            previous_on_switch = on_switch;
            /* 現在のONスイッチの状態を，次のチェック時刻におけるONスイッチの状態として定義する */
            break;
         case ARP_REQUEST : /* ARPリクエストの状態 */
            if (time_out_counter == 0xFFFF) /* タイムアウトのとき */
            {
               OFFLINE_state(); /* オフラインの状態に遷移する */
            }
            break;
         case OUTGOING : /* 呼び出しの状態 */
            if (time_out_counter == 0xFFFF) /* タイムアウトのとき */
            {
               OFFLINE_state(); /* オフラインの状態に遷移する */
            }
            break;
         case INCOMING : /* 着信の状態 */
            if (time_out_counter == 0xFFFF) /* タイムアウトのとき */
            {
               OFFLINE_state(); /* オフラインの状態に遷移する */
            }
            
            if ((on_switch == 0) && (on_switch != previous_on_switch))
            /* 現在のONスイッチがオンで，直前のONスイッチの状態とは異なるとき */
            {
               src_port = 10000; /* 送信元ポート番号の設定 */
               dst_port = 10000; /* 宛先ポート番号の設定 */
               call_packet_send(packet, CONNECT_REPLY); /* 接続リプライを送信する */
               src_ip_id++; /* 次のパケットのために，src_ip_idをインクリメントする */
               ONLINE_state(); /* オンラインの状態に遷移する */
            }
            previous_on_switch = on_switch;
            /* 現在のONスイッチの状態を，次のチェック時刻におけるONスイッチの状態として定義する */
            break;
         case ONLINE : /* オンラインの状態 */
            if (speech_packet_send_flag == 1) /* speech_packet_send_flagが1のとき */
            {
               src_port = 20000; /* 送信元ポート番号の設定 */
               dst_port = 20000; /* 宛先ポート番号の設定 */
               speech_packet_send(packet); /* 音声データのパケットを送信する */
               src_ip_id++; /* 次のパケットのために，src_ip_idをインクリメントする */
               speech_packet_send_flag = 0; /* speech_packet_send_flagを0に戻す */
               src_rtp_sequence_number++; /* src_rtp_sequence_numberをインクリメントする */
               src_rtp_time_stamp += 160; /* src_rtp_time_stampに160を加算する */
            }
            
            if ((off_switch == 0) && (off_switch != previous_off_switch))
            /* 現在のOFFスイッチがオンで，直前のOFFスイッチの状態とは異なるとき */
            {
               src_port = 10000; /* 送信元ポート番号の設定 */
               dst_port = 10000; /* 宛先ポート番号の設定 */
               call_packet_send(packet, DISCONNECT_REQUEST); /* 切断リクエストを送信する */
               src_ip_id++; /* 次のパケットのために，src_ip_idをインクリメントする */
               OFFLINE_state(); /* オフラインの状態に遷移する */
            }
            previous_off_switch = off_switch;
            /* 現在のOFFスイッチの状態を，次のチェック時刻におけるOFFスイッチの状態として定義する */
            break;
      }
   }
}
