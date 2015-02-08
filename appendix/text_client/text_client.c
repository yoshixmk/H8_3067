#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSock�o�[�W����2.0 */

int main(void)
{
   WSADATA wsa_data;
   SOCKET src_socket; /* �N���C�A���g�̃\�P�b�g */
   SOCKADDR_IN src; /* �N���C�A���g�̃\�P�b�g�E�A�h���X */
   SOCKADDR_IN dst; /* �T�[�o�̃\�P�b�g�E�A�h���X */
   unsigned short i;
   char input_buffer[256];
   char text_data[16];
   char text_buffer[256];
   
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
      /* �e�L�X�g�E�f�[�^�̓��� */
      printf("text data:");
      gets(text_buffer);
      
      /* text_buffer��text_data�ɃR�s�[���� */
      for (i = 0; i < 16; i++)
      {
         text_data[i] = ' ';
      }
      i = 0;
      while (i < 16)
      {
         if (text_buffer[i] == '\0')
         {
            break;
         }
         text_data[i] = text_buffer[i];
         i++;
      }
      
      /* �e�L�X�g�E�f�[�^�̃p�P�b�g�̑��M */
      sendto(src_socket, 
             text_data, /* �e�L�X�g�E�f�[�^ */
             16, /* �e�L�X�g�E�f�[�^�̃T�C�Y�i16byte�j */
             0, 
             (LPSOCKADDR)&dst, /* �T�[�o�̃\�P�b�g�E�A�h���X */
             sizeof(dst)
            );
      
      /* ���M����������"end"�ł���΁C�������[�v���甲���� */
      if (strcmp(text_buffer, "end") == 0)
      {
         break;
      }
   }
   
   closesocket(src_socket); /* �\�P�b�g�̃N���[�Y */
   WSACleanup(); /* WinSock�̃��\�[�X��������� */
   
   return 0;
}
