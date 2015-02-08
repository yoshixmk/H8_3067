#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSock�o�[�W����2.0 */
#include <windows.h> /* Windows�̊�{�֐� */
#include <mmsystem.h> /* �}���`���f�B�A�֐� */
#include <conio.h> /* �R���\�[���֐� */

#define NUMBER_OF_DA_BUFFER 8 /* �o�̓o�b�t�@�̐� */
#define DA_BUFFER_SIZE 160 /* �o�̓o�b�t�@�̃T�C�Y */

int main(void)
{
   WSADATA wsa_data;
   SOCKET src_socket; /* �T�[�o�̃\�P�b�g */
   SOCKADDR_IN src; /* �T�[�o�̃\�P�b�g�E�A�h���X */
   SOCKADDR_IN dst; /* �N���C�A���g�̃\�P�b�g�E�A�h���X */
   char key;
   unsigned short i;
   unsigned long argp;
   int dst_size;
   char input_buffer[256];
   char speech_data[160]; /* �����f�[�^�i160�T���v���j */
   
   /* �����f�[�^�̃t�H�[�}�b�g */
   WAVEFORMATEX wave_format_ex = {WAVE_FORMAT_PCM, /* �����������iPCM�j */
                                  1, /* ���m���� */
                                  8000, /* �W�{�����g���i8000Hz�j */
                                  8000, /*1�b������̉����f�[�^�̃T�C�Y�i8000byte�j */
                                  1, /* �����f�[�^�̍ŏ��P�ʁi1byte�j */
                                  8, /* �ʎq�����x�i8bit�j */
                                  0 /* �I�v�V�������̃T�C�Y�i0byte�j */
                                 };
   
   WAVEHDR da_wave_hdr[NUMBER_OF_DA_BUFFER]; /* �o�̓o�b�t�@�̃w�b�_ */
   HWAVEOUT wave_out = 0; /* �o�̓f�o�C�X�̃n���h�� */
   unsigned char da_buffer[NUMBER_OF_DA_BUFFER][DA_BUFFER_SIZE]; /* �o�̓o�b�t�@ */
   unsigned short da_index0; /* �o�̓o�b�t�@�̃C���f�b�N�X */
   unsigned short da_index1; /* �o�̓o�b�t�@�̃C���f�b�N�X */
   
   /* �o�̓f�o�C�X�̃I�[�v�� */
   waveOutOpen(&wave_out, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
   
   /* �o�̓f�o�C�X�̈ꎞ��~ */
   waveOutPause(wave_out);
   
   /* 0���������ނ��Ƃ�da_wave_hdr���N���A���� */
   for (da_index0 = 0; da_index0 < NUMBER_OF_DA_BUFFER; da_index0++)
   {
      memset(&da_wave_hdr[da_index0], 0, sizeof(WAVEHDR));
   }
   
   da_index0 = 0; /* da_index0�̏����l */
   da_index1 = 0; /* da_index1�̏����l */
   
   /* Winsock�̏����� */
   WSAStartup(MAKEWORD(2, 0), /* WinSock�o�[�W����2.0 */
              &wsa_data
             );
   
   /* �\�P�b�g�̍쐬 */
   src_socket = socket(AF_INET, /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
                       SOCK_DGRAM, /* �f�[�^�O�����^�iUDP�j */
                       0
                      );
   
   /* �T�[�o�̃\�P�b�g�E�A�h���X */
   memset(&src, 0, sizeof(src)); /* 0���������ނ��Ƃ�src���N���A���� */
   src.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
   src.sin_addr.s_addr = htonl(INADDR_ANY); /* ������l�b�g���[�N�@�킩��̐ڑ��������� */
   printf("server port:");
   gets(input_buffer); /* ���[�U�[�ɂ��|�[�g�ԍ��̎w�� */
   src.sin_port = htons(atoi(input_buffer)); /* �T�[�o�̃|�[�g�ԍ� */
   
   /* �|�[�g�ԍ��̐ݒ� */
   bind(src_socket, (LPSOCKADDR)&src, sizeof(src)); /* src_socket�ɃT�[�o�̃|�[�g�ԍ���ݒ肷�� */
   
   /* �\�P�b�g�̃u���b�L���O���������� */
   argp = 1;
   ioctlsocket(src_socket, FIONBIO, &argp);
   
   while (1) /* �������[�v */
   {
      dst_size = sizeof(dst);
      if (recvfrom(src_socket, speech_data, 160, 0, (LPSOCKADDR)&dst, &dst_size) > 0)
      /* �����f�[�^�̃p�P�b�g����M�����Ƃ� */
      {
         /* �����f�[�^�̏o�� */
         if (da_index0 < NUMBER_OF_DA_BUFFER)
         {
            /* �o�̓o�b�t�@�ɑ΂��鉹���f�[�^�̏������� */
            for (i = 0; i < DA_BUFFER_SIZE; i++)
            {
               da_buffer[da_index0][i] = speech_data[i];
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
                  da_buffer[da_index1][i] = speech_data[i];
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
      
      if (kbhit()) /* �L�[�{�[�h����̓��͂�����ꍇ */
      {
         key = getch(); /* �L�[�̃`�F�b�N */
         if (key == 13) /* Enter�L�[�������ꂽ�Ƃ� */
         {
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
            
            break;
         }
      }
      
      Sleep(1); /* 1ms�̃X���[�v���͂��ނ��ƂŁC�v���O�����̎��s���x�𒲐߂��� */
   }
   
   closesocket(src_socket); /* �\�P�b�g�̃N���[�Y */
   WSACleanup(); /* WinSock�̃��\�[�X��������� */
   
   return 0;
}
