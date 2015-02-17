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

int main(void)
{
    WSADATA wsa_data;
    SOCKET src_socket; /* �N���C�A���g�̃\�P�b�g */
    SOCKADDR_IN src; /* �N���C�A���g�̃\�P�b�g�E�A�h���X */
    SOCKADDR_IN dst; /* �T�[�o�̃\�P�b�g�E�A�h���X */
    int enter_count = 0;
    unsigned short i;
    char key, c;
    unsigned short cursor;
    unsigned long argp;
    char input_buffer[256];
    char text_data[16];
    char text_buffer[256];

    /* Winsock�̏����� *//* WinSock�o�[�W����2.0 */
    WSAStartup(MAKEWORD(2, 0), &wsa_data);
    /* �\�P�b�g�̍쐬 */
    src_socket = socket(AF_INET, SOCK_DGRAM,0);

    /* �N���C�A���g�̃\�P�b�g�E�A�h���X */
    memset(&src, 0, sizeof(src)); /* 0���������ނ��Ƃ�src���N���A���� */
    src.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */
    src.sin_addr.s_addr = htonl(INADDR_ANY); /* ������l�b�g���[�N�@�킩��̐ڑ��������� */
    src.sin_port = htons(20000); /* �N���C�A���g�̃|�[�g�ԍ� */

    /* �|�[�g�ԍ��̐ݒ� */
    bind(src_socket, (LPSOCKADDR)&src, sizeof(src)); /* src_socket�ɃN���C�A���g�̃|�[�g�ԍ���ݒ肷�� */

    /* �T�[�o�̃\�P�b�g�E�A�h���X �i�������j*/
    memset(&dst, 0, sizeof(dst)); /* 0���������ނ��Ƃ�dst���N���A���� */
    dst.sin_family = AF_INET; /* �C���^�[�l�b�g�̃A�h���X�E�t�@�~�� */

    printf("POCKETBELL server IP address:");/*���͂𑣂�*/
    while (1) /* �������[�v */
    {
        if (kbhit()) /* �L�[�{�[�h����̓��͂�����ꍇ */
        {
           gets(input_buffer);
           printf("�ݒ肵�܂����B%s\n", input_buffer);
           dst.sin_addr.s_addr = inet_addr(input_buffer); /* �T�[�o��IP�A�h���X */
           printf("�|�[�g��30000�ł��BEnter�������Ă��������B\n");
           dst.sin_port = htons(30000); /* �T�[�o�̃|�[�g�ԍ� */
           key = getch(); /* �L�[�̃`�F�b�N */
           if (key == 13) /* Enter�L�[�������ꂽ�Ƃ� */
               printf("!\n");
               break;
        }
    }
          /* �|�P�x���ւ̑��M�f�[�^�̓��� */
          printf("send text data:");

	for(i=0; i < 16; i++){
		c = getchar();
		if(c == '\n'){
			enter_count++;
			i--; /*���s�����͊܂߂Ȃ�����*/
		}
		else if(c < 48 || 57 < c){
			i--; /*1-9�ȊO�͊܂߂Ȃ�����*/
		}
		else{
			enter_count = 0;
			text_data[i] = c;
		}
		if(enter_count == 2){
			text_data[++i] = '\0';
			break;
		}
	}




          /* �e�L�X�g�E�f�[�^�̃p�P�b�g�̑��M */
          sendto(src_socket,
                 text_data, /* �e�L�X�g�E�f�[�^ */
                 16, /* �e�L�X�g�E�f�[�^�̃T�C�Y�i16byte�j */
                 0,
                 (LPSOCKADDR)&dst, /* �T�[�o�̃\�P�b�g�E�A�h���X */
                 sizeof(dst)
                );

       return 0;
}
