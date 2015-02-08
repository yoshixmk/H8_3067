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
#include "speech_packet_function.h" /* �����f�[�^�̃p�P�b�g�̊֐� */
#include <machine.h> /* set_imask_ccr�֐� */

/* sampling_8khz�֐��́C�^�C�}���荞�݂����������Ƃ��ɌĂяo�����֐� */
#pragma interrupt(sampling_8khz)
void sampling_8khz(void)
{
   TISRA &= 0xFE; /* IMFA0��0�ɖ߂� */
   
   ADCSR |= 0x20; /* ADST��1�ɂ���A-D�ϊ����X�^�[�g������ */
   while ((ADCSR & 0x80) == 0x00) /* ADF��1�ɂȂ�̂�҂� */
   {
      /* A-D�ϊ������������ADF��1�ɂȂ� */
   }
   ADCSR &= 0x7F; /* ADF��0�ɖ߂� */
   
   if (ad_buffer_flag == 0) /* ad_buffer_flag��0�̂Ƃ� */
   {
      ad_buffer0[ad_counter] = (unsigned char)(ADDRA >> 8);
      /* ADDRA�̏��8bit��ad_buffer0�Ɋi�[���� */
   }
   else if (ad_buffer_flag == 1) /* ad_buffer_flag��1�̂Ƃ� */
   {
      ad_buffer1[ad_counter] = (unsigned char)(ADDRA >> 8);
      /* ADDRA�̏��8bit��ad_buffer1�Ɋi�[���� */
   }
   
   ad_counter++; /* ad_counter���C���N�������g���� */
   if (ad_counter == 160) /* ad_counter��160�ɂȂ����Ƃ� */
   {
      speech_packet_send_flag = 1;
      /* speech_packet_send_flag��1�ɂ��� */
      
      ad_buffer_flag ^= 1;
      /* XOR���Z�ɂ���āCad_buffer_flag��0�̂Ƃ���1�Cad_buffer_flag��1�̂Ƃ���0�ɂ��� */
      
      ad_counter = 0;
      /* ad_counter��0�ɖ߂� */
   }
}

void main(void)
{
   unsigned short i;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   
   set_imask_ccr(0); /* H8/3067�����荞�݂��󂯕t�������Ԃɂ��� */
   
   H8_register_init(); /* H8/3067�̃��W�X�^�̐ݒ� */
   LCD_init(); /* SC1602B�̏����� */
   NIC_init(); /* RTL8019AS�̏����� */
   
   /* ���M��IP�A�h���X�̐ݒ� */
   src_IP[0] = 192;
   src_IP[1] = 168;
   src_IP[2] = 0;
   src_IP[3] = 100 + (P2DR & 0x07); /* �f�B�b�v�E�X�C�b�`�ɂ���đ��M��IP�A�h���X��ݒ肷�� */
   
   /* ����IP�A�h���X�̐ݒ� */
   dst_IP[0] = 192;
   dst_IP[1] = 168;
   dst_IP[2] = 0;
   dst_IP[3] = 2;
   
   src_port = 10000; /* ���M���|�[�g�ԍ��̐ݒ� */
   dst_port = 20000; /* ����|�[�g�ԍ��̐ݒ� */
   
   LCD_clear(); /* �\���N���A */
   LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
   
   delay_ms(2000); /* 2000ms�i2�b�j�̎��ԑ҂� */
   
   ARP_request(packet); /* ARP���N�G�X�g */
   
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
             (arp_packet -> arp_operation == 2)
             /* ARP���v���C�̂Ƃ� */
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
   
   src_ip_id = 0; /* src_ip_id�̏����l */
   speech_packet_send_flag = 0; /* speech_packet_send_flag�̏����l */
   ad_buffer_flag = 0; /* ad_buffer_flag�̏����l */
   ad_counter = 0; /* ad_counter�̏����l */
   
   TISRA |= 0x10; /* IMIEA0��1�ɐݒ肵�C�^�C�}���荞�݂�L���ɂ��� */
   TSTR |= 0x01; /* STR0��1�ɂ��ă^�C�}0���X�^�[�g������ */
   
   while (1) /* �������[�v */
   {
      if (speech_packet_send_flag == 1) /* speech_packet_send_flag��1�̂Ƃ� */
      {
         speech_packet_send(packet); /* �����f�[�^�̃p�P�b�g�𑗐M���� */
         src_ip_id++; /* ���̃p�P�b�g�̂��߂ɁCsrc_ip_id���C���N�������g���� */
         speech_packet_send_flag = 0; /* speech_packet_send_flag��0�ɖ߂� */
      }
   }
}
