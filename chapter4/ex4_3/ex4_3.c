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
#include "ping_function.h" /* ping�̊֐� */

void main(void)
{
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   PING_PACKET *ping_packet;
   
   H8_register_init(); /* H8/3067�̃��W�X�^�̐ݒ� */
   LCD_init(); /* SC1602B�̏����� */
   NIC_init(); /* RTL8019AS�̏����� */
   
   /* ���M��IP�A�h���X�̐ݒ� */
   src_IP[0] = 192;
   src_IP[1] = 168;
   src_IP[2] = 0;
   src_IP[3] = 100 + (P2DR & 0x07); /* �f�B�b�v�E�X�C�b�`�ɂ���đ��M��IP�A�h���X��ݒ肷�� */
   
   LCD_clear(); /* �\���N���A */
   LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
   
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
         
         ping_packet = (PING_PACKET *)packet; /* packet��PING_PACKET�\���̂ɓ��Ă͂߂� */
         if ((ping_packet -> eth_ethernet_type == 0x0800) && 
             /* IP�̃p�P�b�g�̂Ƃ� */
             (IP_compare(ping_packet -> ip_dst_IP, src_IP) == 0) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
             (ping_packet -> ip_protocol == 1) && 
             /* ICMP�̃p�P�b�g�̂Ƃ� */
             (ping_packet -> ping_type == 8)
             /* ping���N�G�X�g�̂Ƃ� */
            )
         {
            packet_type = 'p'; /* packet_type��'p'�ɂ��� */
         }
      }
      
      switch (packet_type) /* packet_type�ɂ��������ď������s�� */
      {
         case 'a': /* packet_type��'a'�̂Ƃ� */
            LCD_display(packet_type); /* �t���f�B�X�v���C��packet_type��\������ */
            ARP_reply(packet); /* ARP���v���C */
            break;
         case 'p': /* packet_type��'p'�̂Ƃ� */
            LCD_display(packet_type); /* �t���f�B�X�v���C��packet_type��\������ */
            ping_reply(packet); /* ping���v���C */
            break;
      }
   }
}
