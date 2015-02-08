#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h> /* Windows�̊�{�֐� */
#include <mmsystem.h> /* �}���`���f�B�A�֐� */
#include <conio.h> /* �R���\�[���֐� */

#define NUMBER_OF_AD_BUFFER 8 /* ���̓o�b�t�@�̐� */
#define AD_BUFFER_SIZE 160 /* ���̓o�b�t�@�̃T�C�Y */

#define NUMBER_OF_DA_BUFFER 8 /* �o�̓o�b�t�@�̐� */
#define DA_BUFFER_SIZE 160 /* �o�̓o�b�t�@�̃T�C�Y */

int main(void)
{
   char key;
   unsigned short i;
   
   unsigned char speech_data[160]; /* �����f�[�^�i160�T���v���j */
   unsigned short flag; /* �����f�[�^�̏o�͂������邽�߂̃t���O */
   
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
   
   flag = 0; /* flag�̏����l */
   
   while (1) /* �������[�v */
   {
      if (flag == 1) /* flag��1�̂Ƃ� */
      {
         /* �����f�[�^�̏o�� */
         if (da_index0 < NUMBER_OF_DA_BUFFER)
         {
            /* �o�̓o�b�t�@�ɑ΂��鉹���f�[�^�̏������� */
            for (i = 0; i < DA_BUFFER_SIZE; i++)
            {
               da_buffer[da_index0][i] = speech_data[i];
            }
            
            flag = 0; /* flag��0�ɖ߂� */
            
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
               
               flag = 0; /* flag��0�ɖ߂� */
               
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
            
            /* ���̓o�b�t�@����̉����f�[�^�̓ǂݎ�� */
            for (i = 0; i < AD_BUFFER_SIZE; i++)
            {
               speech_data[i] = ad_wave_hdr[ad_index1].lpData[i];
            }
            
            flag = 1; /* flag��1�ɂ��� */
            
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
            
            return 0;
         }
      }
      
      Sleep(1); /* 1ms�̃X���[�v���͂��ނ��ƂŁC�v���O�����̎��s���x�𒲐߂��� */
   }
}
