unsigned char src_IP[4]; /* ���M��IP�A�h���X */
unsigned char src_MAC[6]; /* ���M��MAC�A�h���X */
unsigned short src_port; /* ���M���|�[�g�ԍ� */
unsigned char dst_IP[4]; /* ����IP�A�h���X */
unsigned char dst_MAC[6]; /* ����MAC�A�h���X */
unsigned short dst_port; /* ����|�[�g�ԍ� */

unsigned short src_ip_id; /* ���M�p�P�b�g�ɂ�����IP�w�b�_��Identifier */

unsigned char packet[256]; /* ��M�p�P�b�g�̃f�[�^ */
char str_IP[16]; /* IP�A�h���X�̕����� */

unsigned char phone_state; /* �[���̏�� */

unsigned char speech_packet_send_flag; /* �����f�[�^�̃p�P�b�g�̑��M�������邽�߂̃t���O */

unsigned char ad_buffer_flag; /* ���̓o�b�t�@��I�����邽�߂̃t���O */
unsigned char da_buffer_flag; /* �o�̓o�b�t�@��I�����邽�߂̃t���O */

unsigned short anti_chattering_counter; /* �`���^�����O��������邽�߂̃J�E���^ */
unsigned short time_out_counter; /* �^�C���A�E�g�𔻒肷�邽�߂̃J�E���^ */
unsigned short ad_counter; /* ���̓o�b�t�@�ɂ����鉹���T���v���̏������݈ʒu */
unsigned short da_counter; /* �o�̓o�b�t�@�ɂ����鉹���T���v���̓ǂݎ��ʒu */

unsigned char ad_buffer0[160]; /* ���̓o�b�t�@ */
unsigned char ad_buffer1[160]; /* ���̓o�b�t�@ */
unsigned char da_buffer0[160]; /* �o�̓o�b�t�@ */
unsigned char da_buffer1[160]; /* �o�̓o�b�t�@ */

char key, previous_key;
unsigned char on_switch, previous_on_switch;
unsigned char off_switch, previous_off_switch;
unsigned char cursor;

unsigned short src_rtp_sequence_number; /* ���M�p�P�b�g�ɂ�����RTP�w�b�_��Sequence Number */
unsigned long src_rtp_time_stamp; /* ���M�p�P�b�g�ɂ�����RTP�w�b�_��Time Stamp */
unsigned long src_rtp_ssrc; /* ���M�p�P�b�g�ɂ�����RTP�w�b�_��SSRC */
