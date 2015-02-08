unsigned char src_IP[4]; /* 送信元IPアドレス */
unsigned char src_MAC[6]; /* 送信元MACアドレス */
unsigned short src_port; /* 送信元ポート番号 */
unsigned char dst_IP[4]; /* 宛先IPアドレス */
unsigned char dst_MAC[6]; /* 宛先MACアドレス */
unsigned short dst_port; /* 宛先ポート番号 */

unsigned short src_ip_id; /* 送信パケットにおけるIPヘッダのIdentifier */

unsigned char packet[256]; /* 受信パケットのデータ */
char str_IP[16]; /* IPアドレスの文字列 */

unsigned char phone_state; /* 端末の状態 */

unsigned char speech_packet_send_flag; /* 音声データのパケットの送信を許可するためのフラグ */

unsigned char ad_buffer_flag; /* 入力バッファを選択するためのフラグ */
unsigned char da_buffer_flag; /* 出力バッファを選択するためのフラグ */

unsigned short anti_chattering_counter; /* チャタリングを回避するためのカウンタ */
unsigned short time_out_counter; /* タイムアウトを判定するためのカウンタ */
unsigned short ad_counter; /* 入力バッファにおける音声サンプルの書き込み位置 */
unsigned short da_counter; /* 出力バッファにおける音声サンプルの読み取り位置 */

unsigned char ad_buffer0[160]; /* 入力バッファ */
unsigned char ad_buffer1[160]; /* 入力バッファ */
unsigned char da_buffer0[160]; /* 出力バッファ */
unsigned char da_buffer1[160]; /* 出力バッファ */

char key, previous_key;
unsigned char on_switch, previous_on_switch;
unsigned char off_switch, previous_off_switch;
unsigned char cursor;

unsigned short src_rtp_sequence_number; /* 送信パケットにおけるRTPヘッダのSequence Number */
unsigned long src_rtp_time_stamp; /* 送信パケットにおけるRTPヘッダのTime Stamp */
unsigned long src_rtp_ssrc; /* 送信パケットにおけるRTPヘッダのSSRC */
