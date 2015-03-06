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
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   TEXT_PACKET *udp_packet;
   
   H8_register_init(); /* H8/3067�̃��W�X�^�̐ݒ� */
   LCD_init(); /* SC1602B�̏����� */
   NIC_init(); /* RTL8019AS�̏����� */
   
   /* ���M��IP�A�h���X�̐ݒ� */
   src_IP[0] = 10;
   src_IP[1] = 1;
   src_IP[2] = 68;
   src_IP[3] = 139; /* �f�B�b�v�E�X�C�b�`�ɂ���đ��M��IP�A�h���X��ݒ肷�� */
   
   /* ����IP�A�h���X�̐ݒ� */
   dst_IP[0] = 10;
   dst_IP[1] = 1;
   dst_IP[2] = 68;
   dst_IP[3] = 149;
   
   src_port = 30000; /* ���M���|�[�g�ԍ��̐ݒ� */
   dst_port = 20000; /* ����|�[�g�ԍ��̐ݒ� */
   
   LCD_clear(); /* �\���N���A */
   LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
   
   delay_ms(2000); /* 2000ms�i2�b�j�̎��ԑ҂� */
   
   /*ARP_request(packet);  ARP���N�G�X�g */

   packet_type = 0; /* packet_type��0�ɂ��� */
   do
   {
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
            for (i = 0; i < 6; i++)
            {
               dst_MAC[i] = arp_packet -> arp_src_MAC[i];
               /* �p�P�b�g�ɋL�q����Ă��鑗�M��MAC�A�h���X��dst_MAC�Ɋi�[���� */
            }
            packet_type = 'a'; /* packet_type��'a'�ɂ��� */
         }
      }
   } while (packet_type != 'a'); /* packet_type��'a'�ɂȂ�܂ő҂� */
   ARP_reply(packet);
   LCD_display('!');

   while (1) /* �������[�v */
   {
       if((udp_packet -> eth_ethernet_type == 0x0800) && (IP_compare(udp_packet -> ip_dst_IP, src_IP) == 0) && (udp_packet -> udp_dst_port == 30000)){
                       UDP_text_receive(packet);
                       LCD_print(udp_packet -> text_data);
       }

   }
}
