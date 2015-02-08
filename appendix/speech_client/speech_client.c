#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSock�o�[�W����2.0 */
#include <windows.h> /* Windows�̊�{�֐� */
#include <mmsystem.h> /* �}���`���f�B�A�֐� */
#include <conio.h> /* �R���\�[���֐� */

#define NUMBER_OF_AD_BUFFER 8 /* ���̓o�b�t�@�̐� */
#define AD_BUFFER_SIZE 160 /* ���̓o�b�t�@�̃T�C�Y */

int main(void)
{
   WSADATA wsa_data;
   SOCKET src_socket; /* �N���C�A���g�̃\�P�b�g */
   SOCKADDR_IN src; /* �N���C�A���g�̃\�P�b�g�E�A�h���X */
   SOCKADDR_IN dst; /* �T�[�o�̃\�P�b�g�E�A�h���X */
   char key;
   unsigned short i;
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
   
   WAVEHDR ad_wave_hdr[NUMBER_OF_AD_BUFFER]; /* �o�̓o�b�t�@�̃w�b�_ */
   HWAVEIN wave_in = 0; /* �o�̓f�o�C�X�̃n���h�� */
   unsigned char ad_buffer[NUMBER_OF_AD_BUFFER][AD_BUFFER_SIZE]; /* �o�̓o�b�t�@ */
   unsigned short ad_index0; /* �o�̓o�b�t�@�̃C���f�b�N�X */
   unsigned short ad_index1; /* �o�̓o�b�t�@�̃C���f�b�N�X */
   
   /* ���̓f�o�C�X�̃I�[�v�� */
   waveInOpen(&wave_in, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);
   
   /* 0���������ނ��Ƃ�ad_wave_hdr���N���A���� */
   for (ad_index0 = 0; ad_index0 < NUMBER_OF_AD_BUFFER; ad_index0++)
   {
      memset(&ad_wave_hdr[ad_index0], 0, sizeof(WAVEHDR));
   }
   
   ad_index0 = 0; /* ad_index0�̏����l */
   ad_index1 = 0; /* ad_index1�̏����l */
   
   /* Winsock�̏����� */
   WSAStartup(MAKEWORD(2, 0), /* WinSock�o�[�W����2.0 */
              &wsa_data
             );
   
   /* �\�P�b�g�̍쐬 */
   src_socket = socket(AF_INET, /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
                       SOCK_DGRAM, /* �f�[�^�O�����^�iUDP�j */
                       0
                      );
   
   /* �N���C�A���g�̃\�P�b�g�E�A�h���X */
   memset(&src, 0, sizeof(src)); /* 0���������ނ��Ƃ�src���N���A���� */
   src.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
   src.sin_addr.s_addr = htonl(INADDR_ANY); /* ������l�b�g���[�N�@�킩��̐ڑ��������� */
   src.sin_port = htons(10000); /* �N���C�A���g�̃|�[�g�ԍ� */
   
   /* �|�[�g�ԍ��̐ݒ� */
   bind(src_socket, (LPSOCKADDR)&src, sizeof(src)); /* src_socket�ɃN���C�A���g�̃|�[�g�ԍ���ݒ肷�� */
   
   /* �T�[�o�̃\�P�b�g�E�A�h���X */
   memset(&dst, 0, sizeof(dst)); /* 0���������ނ��Ƃ�dst���N���A���� */
   dst.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
   printf("server IP address:");
   gets(input_buffer); /* ���[�U�[�ɂ��IP�A�h���X�̎w�� */
   dst.sin_addr.s_addr = inet_addr(input_buffer); /* �T�[�o��IP�A�h���X */
   printf("server port:");
   gets(input_buffer); /* ���[�U�[�ɂ��|�[�g�ԍ��̎w�� */
   dst.sin_port = htons(atoi(input_buffer)); /* �T�[�o�̃|�[�g�ԍ� */
   
   while (1) /* �������[�v */
   {
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
            
            /* ���̓o�b�t�@����̉����f�[�^�̓ǂݎ�� */
            for (i = 0; i < AD_BUFFER_SIZE; i++)
            {
               speech_data[i] = ad_wave_hdr[ad_index1].lpData[i];
            }
            
            /* �����f�[�^�̃p�P�b�g�𑗐M���� */
            sendto(src_socket, speech_data, 160, 0, (LPSOCKADDR)&dst, sizeof(dst));
            
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
            
            break;
         }
      }
      
      Sleep(1); /* 1ms�̃X���[�v���͂��ނ��ƂŁC�v���O�����̎��s���x�𒲐߂��� */
   }
   
   closesocket(src_socket); /* �\�P�b�g�̃N���[�Y */
   WSACleanup(); /* WinSock�̃��\�[�X��������� */
   
   return 0;
}
