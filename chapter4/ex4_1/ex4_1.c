#include "H8_register.h" /* H8/3067のレジスタ */
#include "RTL8019AS_register.h" /* RTL8019ASのレジスタ */
#include "global_variables.h" /* グローバル変数の定義 */
#include "H8_register_function.h" /* H8/3067のレジスタの設定 */
#include "delay_function.h" /* 時間待ちの関数 */
#include "LCD_function.h" /* SC1602Bの関数 */
#include "NIC_function.h" /* RTL8019ASの関数 */
#include "matrix_key_function.h" /* マトリクス・キーの関数 */
#include "utility_function.h" /* そのほかの関数 */

void main(void)
{
   char str_IP[16];
   char str_MAC[13];
   
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
   IP_to_str(src_IP, str_IP); /* src_IPをstr_IP（文字列）に変換する */
   LCD_print(str_IP); /* 液晶ディスプレイにstr_IPを表示する */
   
   LCD_control(0xC0); /* カーソルを1行目の先頭に移動する */
   MAC_to_str(src_MAC, str_MAC); /* src_MACをstr_MAC（文字列）に変換する */
   LCD_print(str_MAC); /* 液晶ディスプレイにstr_MACを表示する */
   
   while (1) /* 無限ループ */
   {
      /* 何もしない */
   }
}
