unsigned char src_IP[4]; /* 送信元IPアドレス */
unsigned char src_MAC[6]; /* 送信元MACアドレス */
unsigned short src_port; /* 送信元ポート番号 */
unsigned char dst_IP[4]; /* 宛先IPアドレス */
unsigned char dst_MAC[6]; /* 宛先MACアドレス */
unsigned short dst_port; /* 宛先ポート番号 */
unsigned short src_ip_id; /* 送信パケットにおけるIPヘッダのIdentifier */
unsigned char packet[256]; /* 受信パケットのデータ */
char text_buffer[17]; /* テキスト・バッファ */
