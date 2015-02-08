#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h> /* WinSock�o�[�W����2.0 */

int main(void)
{
   WSADATA wsa_data;
   SOCKET src_socket; /* �T�[�o�̃\�P�b�g */
   SOCKADDR_IN src; /* �T�[�o�̃\�P�b�g�E�A�h���X */
   SOCKADDR_IN dst; /* �N���C�A���g�̃\�P�b�g�E�A�h���X */
   int dst_size;
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
   
   /* �T�[�o�̃\�P�b�g�E�A�h���X */
   memset(&src, 0, sizeof(src)); /* 0���������ނ��Ƃ�src���N���A���� */
   src.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
   src.sin_addr.s_addr = htonl(INADDR_ANY); /* ������l�b�g���[�N�@�킩��̐ڑ��������� */
   printf("server port:");
   gets(input_buffer); /* ���[�U�[�ɂ��|�[�g�ԍ��̎w�� */
   src.sin_port = htons(atoi(input_buffer)); /* �T�[�o�̃|�[�g�ԍ� */
   
   /* �|�[�g�ԍ��̐ݒ� */
   bind(src_socket, (LPSOCKADDR)&src, sizeof(src)); /* src_socket�ɃT�[�o�̃|�[�g�ԍ���ݒ肷�� */
   
   while (1) /* �������[�v */
   {
      /* �e�L�X�g�E�f�[�^�̃p�P�b�g�̎�M */
      dst_size = sizeof(dst);
      recvfrom(src_socket, 
               text_data, /* �e�L�X�g�E�f�[�^ */
               16, /* �e�L�X�g�E�f�[�^�̃T�C�Y�i16byte�j */
               0, 
               (LPSOCKADDR)&dst, /* �N���C�A���g�̃\�P�b�g�E�A�h���X */
               &dst_size
              );
      
      /* text_data��text_buffer�ɃR�s�[���� */
      for (i = 0; i < 16; i++)
      {
         text_buffer[i] = text_data[i];
      }
      text_buffer[16] = '\0'; /* �e�L�X�g�E�f�[�^�̍Ō���i�������ɂ��� */
      
      /* �e�L�X�g�E�f�[�^�̕\�� */
      printf("%s > %s\n", 
             inet_ntoa(dst.sin_addr), /* �N���C�A���g��IP�A�h���X */
             text_buffer /* �e�L�X�g�E�f�[�^ */
            );
      
      /* ��M����������"0000000000000000"�ł���΁C�������[�v���甲���� */
      if (strcmp(text_buffer, "0000000000000000") == 0)
      {
         break;
      }
   }
   
   closesocket(src_socket); /* �\�P�b�g�̃N���[�Y */
   WSACleanup(); /* WinSock�̃��\�[�X��������� */
   
   return 0;
}
