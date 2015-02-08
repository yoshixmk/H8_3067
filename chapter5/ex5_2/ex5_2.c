#include "H8_register.h" /* H8/3067�̃��W�X�^ */
#include "RTL8019AS_register.h" /* RTL8019AS�̃��W�X�^ */
#include "packet_definition.h" /* �p�P�b�g�̒�` */
#include "global_variables.h" /* �O���[�o���ϐ��̒�` */
#include "H8_register_function.h" /* H8/3067�̃��W�X�^�̐ݒ� */
#include "delay_function.h" /* ���ԑ҂��̊֐� */
#include "LCD_function.h" /* SC1602B�̊֐� */
#include "NIC_function.h" /* RTL8019AS�̊֐� */
#include "matrix_key_function.h" /* �}�g���N�X�E�L�[�̊֐� */
#include "utility_function.h" /* ���̂ق��̊֐� */
#include "ARP_function.h" /* ARP�̊֐� */
#include "text_packet_function.h" /* �e�L�X�g�E�f�[�^�̃p�P�b�g�̊֐� */

void main(void)
{
   unsigned short i;
   unsigned char line;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   TEXT_PACKET *text_packet;
   
   H8_register_init(); /* H8/3067�̃��W�X�^�̐ݒ� */
   LCD_init(); /* SC1602B�̏����� */
   NIC_init(); /* RTL8019AS�̏����� */
   
   /* ���M��IP�A�h���X�̐ݒ� */
   src_IP[0] = 192;
   src_IP[1] = 168;
   src_IP[2] = 0;
   src_IP[3] = 100 + (P2DR & 0x07); /* �f�B�b�v�E�X�C�b�`�ɂ���đ��M��IP�A�h���X��ݒ肷�� */
   
   src_port = 20000; /* ���M���|�[�g�ԍ��̐ݒ� */
   
   LCD_clear(); /* �\���N���A */
   LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
   
   text_buffer[16] = 0x00; /* �e�L�X�g�E�o�b�t�@�̍Ō���i�������ɂ��� */
   line = 0; /* line�̏����l */
   
   while (1) /* �������[�v */
   {
      packet_type = 0; /* packet_type��0�ɂ��� */
      
      if (packet_receive(packet) != 1) /* �p�P�b�g����M�����Ƃ� */
      {
         arp_packet = (ARP_PACKET *)packet; /* packet��ARP_PACKET�\���̂ɓ��Ă͂߂� */
         if ((arp_packet -> eth_ethernet_type == 0x0806) && 
             /* ARP�̃p�P�b�g�̂Ƃ� */
             (IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
             (arp_packet -> arp_operation == 1)
             /* ARP���N�G�X�g�̂Ƃ� */
            )
         {
            packet_type = 'a'; /* packet_type��'a'�ɂ��� */
         }
         
         text_packet = (TEXT_PACKET *)packet; /* packet��TEXT_PACKET�\���̂ɓ��Ă͂߂� */
         if ((text_packet -> eth_ethernet_type == 0x0800) && 
             /* IP�̃p�P�b�g�̂Ƃ� */
             (IP_compare(text_packet -> ip_dst_IP, src_IP) == 0) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
             (text_packet -> ip_protocol == 17) && 
             /* UDP�̃p�P�b�g�̂Ƃ� */
             (text_packet -> udp_dst_port == src_port) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��|�[�g�ԍ������M���|�[�g�ԍ��Ɉ�v�����Ƃ� */
             (text_packet_error_check(packet) != 1)
             /* �p�P�b�g�ɃG���[���Ȃ��Ƃ� */
            )
         {
            packet_type = 't'; /* packet_type��'t'�ɂ��� */
         }
      }
      
      switch (packet_type) /* packet_type�ɂ��������ď������s�� */
      {
         case 'a': /* packet_type��'a'�̂Ƃ� */
            ARP_reply(packet); /* ARP���v���C */
            break;
         case 't': /* packet_type��'t'�̂Ƃ� */
            text_packet = (TEXT_PACKET *)packet; /* packet��TEXT_PACKET�\���̂ɓ��Ă͂߂� */
            for (i = 0; i < 16; i++)
            {
               text_buffer[i] = text_packet -> text_data[i];
               /* �p�P�b�g�ɓ��ڂ���Ă���e�L�X�g�E�f�[�^��text_buffer�Ɋi�[���� */
            }
            if (line == 0) /* line��0�̂Ƃ� */
            {
               LCD_clear(); /* �\���N���A */
               LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
            }
            else if (line == 1) /* line��1�̂Ƃ� */
            {
               LCD_control(0xC0); /* �J�[�\����1�s�ڂ̐擪�Ɉړ����� */
            }
            LCD_print(text_buffer); /* �t���f�B�X�v���C�Ƀe�L�X�g�E�o�b�t�@�̕������\������ */
            line ^= 1; /* XOR���Z�ɂ���āCline��0�̂Ƃ���1�Cline��1�̂Ƃ���0�ɂ��� */
            break;
      }
   }
}
