/* 端末の状態 */
#define OFFLINE            0 /* オフライン */
#define ARP_REQUEST        1 /* ARPリクエスト */
#define OUTGOING           2 /* 呼び出し */
#define INCOMING           3 /* 着信 */
#define ONLINE             4 /* オンライン */

/* 呼制御の種類 */
#define CONNECT_REQUEST    0 /* 接続リクエスト */
#define CONNECT_REPLY      1 /* 接続リプライ */
#define DISCONNECT_REQUEST 2 /* 切断リクエスト */
#define DISCONNECT_REPLY   3 /* 切断リプライ */
