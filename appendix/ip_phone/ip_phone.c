#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSock�o�[�W����2.0 */
#include <windows.h> /* Windows�̊�{�֐� */
#include <mmsystem.h> /* �}���`���f�B�A�֐� */
#include <conio.h> /* �R���\�[���֐� */
#include <time.h> /* ���ԊǗ��̊֐� */

#define NUMBER_OF_AD_BUFFER 8 /* ���̓o�b�t�@�̐� */
#define AD_BUFFER_SIZE 160 /* ���̓o�b�t�@�̃T�C�Y */

#define NUMBER_OF_DA_BUFFER 8 /* �o�̓o�b�t�@�̐� */
#define DA_BUFFER_SIZE 160 /* �o�̓o�b�t�@�̃T�C�Y */

/* �[���̏�� */
#define OFFLINE            0 /* �I�t���C�� */
#define ARP_REQUEST        1 /* ARP���N�G�X�g */
#define OUTGOING           2 /* �Ăяo�� */
#define INCOMING           3 /* ���M */
#define ONLINE             4 /* �I�����C�� */

/* �Đ���̎�� */
#define CONNECT_REQUEST    0 /* �ڑ����N�G�X�g */
#define CONNECT_REPLY      1 /* �ڑ����v���C */
#define DISCONNECT_REQUEST 2 /* �ؒf���N�G�X�g */
#define DISCONNECT_REPLY   3 /* �ؒf���v���C */

typedef struct
{
   /* �Đ��䃁�b�Z�[�W�i2byte�j */
   unsigned short call_type;
} CALL_PACKET;

typedef struct
{
   /* RTP�w�b�_�i12byte�j */
   unsigned char rtp_v_p_x_cc;
   unsigned char rtp_m_payload_type;
   unsigned short rtp_sequence_number;
   unsigned long rtp_time_stamp;
   unsigned long rtp_ssrc;
   
   /* �����f�[�^�i160byte�j */
   unsigned char speech_data[160];
} SPEECH_PACKET;

int main(void)
{
   WSADATA wsa_data;
   SOCKET s10000; /* �Đ���f�[�^�̑���M�̂��߂̃\�P�b�g */
   SOCKET s20000; /* �����f�[�^�̑���M�̂��߂̃\�P�b�g */
   SOCKADDR_IN src0; /* ���̒[���̃\�P�b�g�E�A�h���X�i�Đ���f�[�^�j */
   SOCKADDR_IN src1; /* ���̒[���̃\�P�b�g�E�A�h���X�i�����f�[�^�j */
   SOCKADDR_IN dst0; /* �����̒[���̃\�P�b�g�E�A�h���X�i�Đ���f�[�^�j */
   SOCKADDR_IN dst1; /* �����̒[���̃\�P�b�g�E�A�h���X�i�����f�[�^�j */
   char key;
   unsigned short i;
   unsigned short phone_state;
   unsigned short cursor;
   unsigned long argp;
   int dst_size;
   char str_IP[16];
   time_t initial_time;
   time_t reference_time;
   time_t current_time;
   CALL_PACKET call_packet;
   SPEECH_PACKET speech_packet;
   unsigned short src_rtp_sequence_number;
   unsigned long src_rtp_time_stamp;
   unsigned long src_rtp_ssrc;
   
   /* �����f�[�^�̃t�H�[�}�b�g */
   WAVEFORMATEX wave_format_ex = {WAVE_FORMAT_PCM, /* �����������iPCM�j */
                                  1, /* ���m���� */
                                  8000, /* �W�{�����g���i8000Hz�j */
                                  8000, /*1�b������̉����f�[�^�̃T�C�Y�i8000byte�j */
                                  1, /* �����f�[�^�̍ŏ��P�ʁi1byte�j */
                                  8, /* �ʎq�����x�i8bit�j */
                                  0 /* �I�v�V�������̃T�C�Y�i0byte�j */
                                 };
   
   WAVEHDR ad_wave_hdr[NUMBER_OF_AD_BUFFER]; /* ���̓o�b�t�@�̃w�b�_ */
   HWAVEIN wave_in = 0; /* ���̓f�o�C�X�̃n���h�� */
   unsigned char ad_buffer[NUMBER_OF_AD_BUFFER][AD_BUFFER_SIZE]; /* ���̓o�b�t�@ */
   unsigned short ad_index0; /* ���̓o�b�t�@�̃C���f�b�N�X */
   unsigned short ad_index1; /* ���̓o�b�t�@�̃C���f�b�N�X */
   
   WAVEHDR da_wave_hdr[NUMBER_OF_DA_BUFFER]; /* �o�̓o�b�t�@�̃w�b�_ */
   HWAVEOUT wave_out = 0; /* �o�̓f�o�C�X�̃n���h�� */
   unsigned char da_buffer[NUMBER_OF_DA_BUFFER][DA_BUFFER_SIZE]; /* �o�̓o�b�t�@ */
   unsigned short da_index0; /* �o�̓o�b�t�@�̃C���f�b�N�X */
   unsigned short da_index1; /* �o�̓o�b�t�@�̃C���f�b�N�X */
   
   /* Winsock�̏����� */
   WSAStartup(MAKEWORD(2, 0), /* WinSock�o�[�W����2.0 */
              &wsa_data
             );
   
   /* �Đ���f�[�^�̑���M�̂��߂̃\�P�b�g */
   s10000 = socket(AF_INET, SOCK_DGRAM, 0); /* �\�P�b�g�̍쐬 */
   memset(&src0, 0, sizeof(src0)); /* 0���������ނ��Ƃ�src0���N���A���� */
   src0.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
   src0.sin_addr.s_addr = htonl(INADDR_ANY); /* ������l�b�g���[�N�@�킩��̐ڑ��������� */
   src0.sin_port = htons(10000); /* �|�[�g�ԍ���10000�Ԃɐݒ肷�� */
   bind(s10000, (LPSOCKADDR)&src0, sizeof(src0)); /* s10000�Ƀ|�[�g�ԍ���ݒ肷�� */
   argp = 1;
   ioctlsocket(s10000, FIONBIO, &argp); /* s10000�̃u���b�L���O���������� */
   
   /* �����f�[�^�̑���M�̂��߂̃\�P�b�g */
   s20000 = socket(AF_INET, SOCK_DGRAM, 0); /* �\�P�b�g�̍쐬 */
   memset(&src1, 0, sizeof(src1)); /* 0���������ނ��Ƃ�src1���N���A���� */
   src1.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
   src1.sin_addr.s_addr = htonl(INADDR_ANY); /* ������l�b�g���[�N�@�킩��̐ڑ��������� */
   src1.sin_port = htons(20000); /* �|�[�g�ԍ���20000�Ԃɐݒ肷�� */
   bind(s20000, (LPSOCKADDR)&src1, sizeof(src1)); /* s20000�Ƀ|�[�g�ԍ���ݒ肷�� */
   argp = 1;
   ioctlsocket(s20000, FIONBIO, &argp); /* s20000�̃u���b�L���O���������� */
   
   /* �I�t���C���̏�ԂɑJ�ڂ��� */
   printf("Call To:");
   cursor = 0;
   memset(str_IP, 0, sizeof(str_IP));
   phone_state = OFFLINE; /* phone_state��OFFLINE�ɂ��� */
   
   while (1) /* �������[�v */
   {
      switch (phone_state) /* phone_state�ɂ��������ď������s�� */
      {
         case OFFLINE : /* �I�t���C���̏�� */
            dst_size = sizeof(dst0);
            if (recvfrom(s10000, (char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, &dst_size) > 0)
            /* �Đ���f�[�^�̃p�P�b�g����M�����Ƃ� */
            {
               if (ntohs(call_packet.call_type) == CONNECT_REQUEST) /* �ڑ����N�G�X�g�̂Ƃ� */
               {
                  printf("\n");
                  printf("Call From:%s\n", inet_ntoa(dst0.sin_addr));
                  initial_time = time(NULL); /* ���݂̎��������������Ƃ��� */
                  reference_time = initial_time; /* ����� */
                  phone_state = INCOMING; /* phone_state��INCOMING�ɂ��� */
               }
            }
            
            if (kbhit()) /* �L�[�{�[�h����̓��͂�����ꍇ */
            {
               key = getch(); /* �L�[�̃`�F�b�N */
               if (key == 13) /* Enter�L�[�������ꂽ�Ƃ� */
               {
                  str_IP[cursor] = '\0';
                  printf("\n");
                  
                  /* ���M����������"end"�ł���΁C�A�v���P�[�V�������I������ */
                  if (strcmp(str_IP, "end") == 0)
                  {
                     closesocket(s10000); /* �\�P�b�g�̃N���[�Y */
                     closesocket(s20000); /* �\�P�b�g�̃N���[�Y */
                     WSACleanup(); /* WinSock�̃��\�[�X��������� */
                     return 0;
                  }
                  
                  memset(&dst0, 0, sizeof(dst0)); /* 0���������ނ��Ƃ�dst0���N���A���� */
                  dst0.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
                  dst0.sin_addr.s_addr = inet_addr(str_IP); /* �����̒[����IP�A�h���X */
                  dst0.sin_port = htons(10000); /* �����̒[���̃|�[�g�ԍ��i10000�ԁj */
                  
                  /* �ڑ����N�G�X�g�𑗐M���� */
                  memset(&call_packet, 0, sizeof(call_packet));
                  call_packet.call_type = htons(CONNECT_REQUEST);
                  sendto(s10000, (const char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, sizeof(dst0));
                  
                  initial_time = time(NULL); /* ���݂̎��������������Ƃ��� */
                  reference_time = initial_time; /* ����� */
                  phone_state = OUTGOING; /* phone_state��OUTGOING�ɂ��� */
               }
               else
               {
                  putch(key); /* key��\������ */
                  str_IP[cursor] = key; /* key��str_IP�Ɋi�[���� */
                  cursor++; /* cursor���C���N�������g���� */
               }
            }
            Sleep(1); /* 1ms�̃X���[�v���͂��ނ��ƂŁC�v���O�����̎��s���x�𒲐߂��� */
            break;
         case ARP_REQUEST : /* ARP���N�G�X�g�̏�� */
            break;
         case OUTGOING : /* �Ăяo���̏�� */
            current_time = time(NULL); /* ���݂̎��� */
            if (current_time - initial_time < 8) /* ������������8�b�ȏ�o�߂��Ă��Ȃ��Ƃ� */
            {
               if (current_time - reference_time > 1) /* ���������1�b�ȏ�o�߂����Ƃ� */
               {
                  putch('.'); /* �s���I�h��\������ */
                  reference_time++; /* reference_time���C���N�������g���� */
               }
               
               dst_size = sizeof(dst0);
               if (recvfrom(s10000, (char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, &dst_size) > 0)
               /* �Đ���f�[�^�̃p�P�b�g����M�����Ƃ� */
               {
                  if (ntohs(call_packet.call_type) == CONNECT_REPLY) /* �ڑ����v���C�̂Ƃ� */
                  {
                     /* ���̓f�o�C�X�̃I�[�v�� */
                     waveInOpen(&wave_in, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
                     
                     /* 0���������ނ��Ƃ�ad_wave_hdr���N���A���� */
                     for (ad_index0 = 0; ad_index0 < NUMBER_OF_AD_BUFFER; ad_index0++)
                     {
                        memset(&ad_wave_hdr[ad_index0], 0, sizeof(WAVEHDR));
                     }
                     
                     /* �o�̓f�o�C�X�̃I�[�v�� */
                     waveOutOpen(&wave_out, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
                     
                     /* �o�̓f�o�C�X�̈ꎞ��~ */
                     waveOutPause(wave_out);
                     
                     /* 0���������ނ��Ƃ�da_wave_hdr���N���A���� */
                     for (da_index0 = 0; da_index0 < NUMBER_OF_DA_BUFFER; da_index0++)
                     {
                        memset(&da_wave_hdr[da_index0], 0, sizeof(WAVEHDR));
                     }
                     
                     ad_index0 = 0; /* ad_index0�̏����l */
                     ad_index1 = 0; /* ad_index1�̏����l */
                     
                     da_index0 = 0; /* da_index0�̏����l */
                     da_index1 = 0; /* da_index1�̏����l */
                     
                     src_rtp_sequence_number = 0; /* src_rtp_sequence_number�̏����l */
                     src_rtp_time_stamp = 0; /* src_rtp_time_stamp�̏����l */
                     src_rtp_ssrc = 0; /* src_rtp_ssrc�̏����l */
                     
                     printf("online\n");
                     
                     memset(&dst1, 0, sizeof(dst1)); /* 0���������ނ��Ƃ�dst1���N���A���� */
                     dst1.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
                     dst1.sin_addr.s_addr = dst0.sin_addr.s_addr; /* �����̒[����IP�A�h���X */
                     dst1.sin_port = htons(20000); /* �����̒[���̃|�[�g�ԍ��i20000�ԁj */
                     
                     phone_state = ONLINE; /* phone_state��ONLINE�ɂ��� */
                  }
               }
            }
            else /* �^�C���A�E�g */
            {
               printf("time out\n");
               
               /* �I�t���C���̏�ԂɑJ�ڂ��� */
               printf("Call To:");
               cursor = 0;
               memset(str_IP, 0, sizeof(str_IP));
               phone_state = OFFLINE; /* phone_state��OFFLINE�ɂ��� */
            }
            Sleep(1); /* 1ms�̃X���[�v���͂��ނ��ƂŁC�v���O�����̎��s���x�𒲐߂��� */
            break;
         case INCOMING : /* ���M�̏�� */
            current_time = time(NULL); /* ���݂̎��� */
            if (current_time - initial_time < 8) /* ������������8�b�ȏ�o�߂��Ă��Ȃ��Ƃ� */
            {
               if (current_time - reference_time > 1) /* ���������1�b�ȏ�o�߂����Ƃ� */
               {
                  putch('.'); /* �s���I�h��\������ */
                  reference_time++; /* reference_time���C���N�������g���� */
               }
               
               if (kbhit()) /* �L�[�{�[�h����̓��͂�����ꍇ */
               {
                  key = getch(); /* �L�[�̃`�F�b�N */
                  if (key == 13) /* Enter�L�[�������ꂽ�Ƃ� */
                  {
                     /* ���̓f�o�C�X�̃I�[�v�� */
                     waveInOpen(&wave_in, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
                     
                     /* 0���������ނ��Ƃ�ad_wave_hdr���N���A���� */
                     for (ad_index0 = 0; ad_index0 < NUMBER_OF_AD_BUFFER; ad_index0++)
                     {
                        memset(&ad_wave_hdr[ad_index0], 0, sizeof(WAVEHDR));
                     }
                     
                     /* �o�̓f�o�C�X�̃I�[�v�� */
                     waveOutOpen(&wave_out, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
                     
                     /* �o�̓f�o�C�X�̈ꎞ��~ */
                     waveOutPause(wave_out);
                     
                     /* 0���������ނ��Ƃ�da_wave_hdr���N���A���� */
                     for (da_index0 = 0; da_index0 < NUMBER_OF_DA_BUFFER; da_index0++)
                     {
                        memset(&da_wave_hdr[da_index0], 0, sizeof(WAVEHDR));
                     }
                     
                     ad_index0 = 0; /* ad_index0�̏����l */
                     ad_index1 = 0; /* ad_index1�̏����l */
                     
                     da_index0 = 0; /* da_index0�̏����l */
                     da_index1 = 0; /* da_index1�̏����l */
                     
                     src_rtp_sequence_number = 0; /* src_rtp_sequence_number�̏����l */
                     src_rtp_time_stamp = 0; /* src_rtp_time_stamp�̏����l */
                     src_rtp_ssrc = 0; /* src_rtp_ssrc�̏����l */
                     
                     /* �ڑ����v���C�𑗐M���� */
                     memset(&call_packet, 0, sizeof(call_packet));
                     call_packet.call_type = htons(CONNECT_REPLY);
                     sendto(s10000, (const char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, sizeof(dst0));
                     
                     printf("online\n");
                     
                     memset(&dst1, 0, sizeof(dst1)); /* 0���������ނ��Ƃ�dst1���N���A���� */
                     dst1.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
                     dst1.sin_addr.s_addr = dst0.sin_addr.s_addr; /* �����̒[����IP�A�h���X */
                     dst1.sin_port = htons(20000); /* �����̒[���̃|�[�g�ԍ��i20000�ԁj */
                     
                     phone_state = ONLINE; /* phone_state��ONLINE�ɂ��� */
                  }
               }
            }
            else /* �^�C���A�E�g */
            {
               printf("time out\n");
               
               /* �I�t���C���̏�ԂɑJ�ڂ��� */
               printf("Call To:");
               cursor = 0;
               memset(str_IP, 0, sizeof(str_IP));
               phone_state = OFFLINE; /* phone_state��OFFLINE�ɂ��� */
            }
            Sleep(1); /* 1ms�̃X���[�v���͂��ނ��ƂŁC�v���O�����̎��s���x�𒲐߂��� */
            break;
         case ONLINE :
            dst_size = sizeof(dst0);
            if (recvfrom(s10000, (char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, &dst_size) > 0)
            /* �Đ���f�[�^�̃p�P�b�g����M�����Ƃ� */
            {
               if (ntohs(call_packet.call_type) == DISCONNECT_REQUEST) /* �ؒf���N�G�X�g�̂Ƃ� */
               {
                  waveInStop(wave_in); /* �����f�[�^�̓��͂��~���� */
                  for (ad_index0 = 0; ad_index0 < NUMBER_OF_AD_BUFFER; ad_index0++)
                  {
                     /* ���̓o�b�t�@�̃A�����b�N */
                     if (ad_wave_hdr[ad_index0].dwFlags & WHDR_PREPARED)
                     {
                        waveInUnprepareHeader(wave_in, &ad_wave_hdr[ad_index0], sizeof(WAVEHDR ));
                     }
                  }
                  waveInClose(wave_in); /* ���̓f�o�C�X�̃N���[�Y */
                  
                  waveOutPause(wave_out); /* �����f�[�^�̏o�͂��~���� */
                  for (da_index0 = 0; da_index0 < NUMBER_OF_DA_BUFFER; da_index0++)
                  {
                     /* �o�̓o�b�t�@�̃A�����b�N */
                     if (da_wave_hdr[da_index0].dwFlags & WHDR_PREPARED)
                     {
                        waveOutUnprepareHeader(wave_out, &da_wave_hdr[da_index0], sizeof(WAVEHDR));
                     }
                  }
                  waveOutClose(wave_out); /* �o�̓f�o�C�X�̃N���[�Y */
                  
                  /* �ؒf���v���C�𑗐M���� */
                  memset(&call_packet, 0, sizeof(call_packet));
                  call_packet.call_type = htons(DISCONNECT_REPLY);
                  sendto(s10000, (const char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, sizeof(dst0));
                  
                  /* �I�t���C���̏�ԂɑJ�ڂ��� */
                  printf("Call To:");
                  cursor = 0;
                  memset(str_IP, 0, sizeof(str_IP));
                  phone_state = OFFLINE; /* phone_state��OFFLINE�ɂ��� */
               }
            }
            
            dst_size = sizeof(dst1);
            if (recvfrom(s20000, (char *)&speech_packet, 172, 0, (LPSOCKADDR)&dst1, &dst_size) > 0)
            /* �����f�[�^�̃p�P�b�g����M�����Ƃ� */
            {
               /* �����f�[�^�̏o�� */
               if (da_index0 < NUMBER_OF_DA_BUFFER)
               {
                  /* �o�̓o�b�t�@�ɑ΂��鉹���f�[�^�̏������� */
                  for (i = 0; i < DA_BUFFER_SIZE; i++)
                  {
                     da_buffer[da_index0][i] = speech_packet.speech_data[i];
                  }
                  
                  /* �o�̓o�b�t�@�̐ݒ� */
                  da_wave_hdr[da_index0].lpData = da_buffer[da_index0]; /* �o�̓o�b�t�@�̎w�� */
                  da_wave_hdr[da_index0].dwBufferLength = DA_BUFFER_SIZE; /* �o�̓o�b�t�@�̃T�C�Y */
                  da_wave_hdr[da_index0].dwFlags = 0; /* �t���O�̃N���A */
                  
                  /* �o�̓o�b�t�@�̃��b�N */
                  waveOutPrepareHeader(wave_out, &da_wave_hdr[da_index0], sizeof(WAVEHDR));
                  
                  /* �o�̓o�b�t�@���o�͑҂��L���[�ɒǉ����� */
                  waveOutWrite(wave_out, &da_wave_hdr[da_index0], sizeof(WAVEHDR));
                  
                  da_index0++; /* da_index0�̃C���N�������g */
                  if (da_index0 == NUMBER_OF_DA_BUFFER)
                  {
                     waveOutRestart(wave_out);
                     /* ���ׂĂ̏o�̓o�b�t�@���o�͑҂��L���[�ɒǉ����ꂽ�特���f�[�^�̏o�͂��J�n���� */
                  }
               }
               else
               {
                  if ((da_wave_hdr[da_index1].dwFlags & WHDR_DONE) != 0)
                  /* �o�̓o�b�t�@����̉����f�[�^�̏o�͂����������Ƃ� */
                  {
                     /* �o�̓o�b�t�@�ɑ΂��鉹���f�[�^�̏������� */
                     for (i = 0; i < DA_BUFFER_SIZE; i++)
                     {
                        da_buffer[da_index1][i] = speech_packet.speech_data[i];
                     }
                     
                     /* �o�̓o�b�t�@�̃A�����b�N */
                     waveOutUnprepareHeader(wave_out, &da_wave_hdr[da_index1], sizeof(WAVEHDR));
                     
                     /* �o�̓o�b�t�@�̐ݒ� */
                     da_wave_hdr[da_index1].lpData = da_buffer[da_index1]; /* �o�̓o�b�t�@�̎w�� */
                     da_wave_hdr[da_index1].dwBufferLength = DA_BUFFER_SIZE; /* �o�̓o�b�t�@�̃T�C�Y */
                     da_wave_hdr[da_index1].dwFlags = 0; /* �t���O�̃N���A */
                     
                     /* �o�̓o�b�t�@�̃��b�N */
                     waveOutPrepareHeader(wave_out, &da_wave_hdr[da_index1], sizeof(WAVEHDR));
                     
                     /* �o�̓o�b�t�@���o�͑҂��L���[�ɒǉ����� */
                     waveOutWrite(wave_out, &da_wave_hdr[da_index1], sizeof(WAVEHDR));
                     
                     da_index1++; /* da_index1�̃C���N�������g */
                     if (da_index1 == NUMBER_OF_DA_BUFFER)
                     {
                        da_index1 = 0;
                        /* da_index1��0�ɖ߂��C�o�̓o�b�t�@���ė��p���� */
                     }
                  }
               }
            }
            
            /* �����f�[�^�̓��� */
            if (ad_index0 < NUMBER_OF_AD_BUFFER)
            {
               /* ���̓o�b�t�@�̐ݒ� */
               ad_wave_hdr[ad_index0].lpData = ad_buffer[ad_index0]; /* ���̓o�b�t�@�̎w�� */
               ad_wave_hdr[ad_index0].dwBufferLength = AD_BUFFER_SIZE; /* ���̓o�b�t�@�̃T�C�Y */
               ad_wave_hdr[ad_index0].dwFlags = 0; /* �t���O�̃N���A */
               
               /* ���̓o�b�t�@�̃��b�N */
               waveInPrepareHeader(wave_in, &ad_wave_hdr[ad_index0], sizeof(WAVEHDR));
               
               /* ���̓o�b�t�@����͑҂��L���[�ɒǉ����� */
               waveInAddBuffer(wave_in, &ad_wave_hdr[ad_index0], sizeof(WAVEHDR));
               
               ad_index0++; /* ad_index0�̃C���N�������g */
               if (ad_index0 == NUMBER_OF_AD_BUFFER)
               {
                  waveInStart(wave_in);
                  /* ���ׂĂ̓��̓o�b�t�@�����͑҂��L���[�ɒǉ����ꂽ�特���f�[�^�̓��͂��J�n���� */
               }
            }
            else
            {
               if ((ad_wave_hdr[ad_index1].dwFlags & WHDR_DONE) != 0)
               /* ���̓o�b�t�@�ɑ΂��鉹���f�[�^�̓��͂����������Ƃ� */
               {
                  /* ���̓o�b�t�@�̃A�����b�N */
                  waveInUnprepareHeader(wave_in, &ad_wave_hdr[ad_index1], sizeof(WAVEHDR));
                  
                  /* 0���������ނ��Ƃ�speech_packet���N���A���� */
                  memset(&speech_packet, 0, sizeof(speech_packet));
                  
                  /* RTP�w�b�_�̍쐬 */
                  speech_packet.rtp_v_p_x_cc = 0x80;
                  speech_packet.rtp_m_payload_type = 0x14;
                  speech_packet.rtp_sequence_number = htons(src_rtp_sequence_number);
                  speech_packet.rtp_time_stamp = htonl(src_rtp_time_stamp);
                  speech_packet.rtp_ssrc = htonl(src_rtp_ssrc);
                  
                  /* ���̓o�b�t�@����̉����f�[�^�̓ǂݎ�� */
                  for (i = 0; i < AD_BUFFER_SIZE; i++)
                  {
                     speech_packet.speech_data[i] = ad_wave_hdr[ad_index1].lpData[i];
                  }
                  
                  /* �����f�[�^�̃p�P�b�g�𑗐M���� */
                  sendto(s20000, (const char *)&speech_packet, 172, 0, (LPSOCKADDR)&dst1, sizeof(dst1));
                  
                  src_rtp_sequence_number++; /* src_rtp_sequence_number���C���N�������g���� */
                  src_rtp_time_stamp += AD_BUFFER_SIZE; /* src_rtp_time_stamp��AD_BUFFER_SIZE�����Z���� */
                  
                  /* ���̓o�b�t�@�̐ݒ� */
                  ad_wave_hdr[ad_index1].lpData = ad_buffer[ad_index1]; /* ���̓o�b�t�@�̎w�� */
                  ad_wave_hdr[ad_index1].dwBufferLength = AD_BUFFER_SIZE; /* ���̓o�b�t�@�̃T�C�Y */
                  ad_wave_hdr[ad_index1].dwFlags = 0; /* �t���O�̃N���A */
                  
                  /* ���̓o�b�t�@�̃��b�N */
                  waveInPrepareHeader(wave_in, &ad_wave_hdr[ad_index1], sizeof(WAVEHDR));
                  
                  /* ���̓o�b�t�@����͑҂��L���[�ɒǉ����� */
                  waveInAddBuffer(wave_in, &ad_wave_hdr[ad_index1], sizeof(WAVEHDR));
                  
                  ad_index1++; /* ad_index1�̃C���N�������g */
                  if (ad_index1 == NUMBER_OF_AD_BUFFER)
                  {
                     ad_index1 = 0;
                     /* ad_index1��0�ɖ߂��C���̓o�b�t�@���ė��p���� */
                  }
               }
            }
            
            if (kbhit()) /* �L�[�{�[�h����̓��͂�����ꍇ */
            {
               key = getch(); /* �L�[�̃`�F�b�N */
               if (key == 13) /* Enter�L�[�������ꂽ�Ƃ� */
               {
                  waveInStop(wave_in); /* �����f�[�^�̓��͂��~���� */
                  for (ad_index0 = 0; ad_index0 < NUMBER_OF_AD_BUFFER; ad_index0++)
                  {
                     /* ���̓o�b�t�@�̃A�����b�N */
                     if (ad_wave_hdr[ad_index0].dwFlags & WHDR_PREPARED)
                     {
                        waveInUnprepareHeader(wave_in, &ad_wave_hdr[ad_index0], sizeof(WAVEHDR ));
                     }
                  }
                  waveInClose(wave_in); /* ���̓f�o�C�X�̃N���[�Y */
                  
                  waveOutPause(wave_out); /* �����f�[�^�̏o�͂��~���� */
                  for (da_index0 = 0; da_index0 < NUMBER_OF_DA_BUFFER; da_index0++)
                  {
                     /* �o�̓o�b�t�@�̃A�����b�N */
                     if (da_wave_hdr[da_index0].dwFlags & WHDR_PREPARED)
                     {
                        waveOutUnprepareHeader(wave_out, &da_wave_hdr[da_index0], sizeof(WAVEHDR));
                     }
                  }
                  waveOutClose(wave_out); /* �o�̓f�o�C�X�̃N���[�Y */
                  
                  /* �ؒf���N�G�X�g�𑗐M���� */
                  memset(&call_packet, 0, sizeof(call_packet));
                  call_packet.call_type = htons(DISCONNECT_REQUEST);
                  sendto(s10000, (const char *)&call_packet, 2, 0, (LPSOCKADDR)&dst0, sizeof(dst0));
                  
                  /* �I�t���C���̏�ԂɑJ�ڂ��� */
                  printf("Call To:");
                  cursor = 0;
                  memset(str_IP, 0, sizeof(str_IP));
                  phone_state = OFFLINE; /* phone_state��OFFLINE�ɂ��� */
               }
            }
            Sleep(1); /* 1ms�̃X���[�v���͂��ނ��ƂŁC�v���O�����̎��s���x�𒲐߂��� */
            break;
      }
   }
}
