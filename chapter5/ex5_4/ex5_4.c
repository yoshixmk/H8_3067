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
   
   if (da_buffer_flag == 0) /* da_buffer_flag��0�̂Ƃ� */
   {
      DADR0 = da_buffer0[da_counter];
      /* da_buffer0�Ɋi�[����Ă��鉹���f�[�^��DA0�ɏo�͂��� */
   }
   else if (da_buffer_flag == 1) /* da_buffer_flag��1�̂Ƃ� */
   {
      DADR0 = da_buffer1[da_counter];
      /* da_buffer1�Ɋi�[����Ă��鉹���f�[�^��DA0�ɏo�͂��� */
   }
   
   da_counter++; /* da_counter���C���N�������g���� */
   if (da_counter == 160) /* da_counter��160�ɂȂ����Ƃ� */
   {
      da_buffer_flag ^= 1;
      /* XOR���Z�ɂ���āCda_buffer_flag��0�̂Ƃ���1�Cda_buffer_flag��1�̂Ƃ���0�ɂ��� */
      
      da_counter = 0;
      /* da_counter��0�ɖ߂� */
   }
}

void main(void)
{
   unsigned short i;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   SPEECH_PACKET *speech_packet;
   
   set_imask_ccr(0); /* H8/3067�����荞�݂��󂯕t�������Ԃɂ��� */
   
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
   
   da_buffer_flag = 0; /* da_buffer_flag�̏����l */
   da_counter = 0; /* da_counter�̏����l */
   for (i = 0; i < 160; i++)
   {
      da_buffer0[i] = 128; /* da_buffer0�̏����l�i�����f�[�^�̃I�t�Z�b�g��128�j */
      da_buffer1[i] = 128; /* da_buffer1�̏����l�i�����f�[�^�̃I�t�Z�b�g��128�j */
   }
   
   TISRA |= 0x10; /* IMIEA0��1�ɐݒ肵�C�^�C�}���荞�݂�L���ɂ��� */
   TSTR |= 0x01; /* STR0��1�ɂ��ă^�C�}0���X�^�[�g������ */
   
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
         
         speech_packet = (SPEECH_PACKET *)packet; /* packet��SPEECH_PACKET�\���̂ɓ��Ă͂߂� */
         if ((speech_packet -> eth_ethernet_type == 0x0800) && 
             /* IP�̃p�P�b�g�̂Ƃ� */
             (IP_compare(speech_packet -> ip_dst_IP, src_IP) == 0) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
             (speech_packet -> ip_protocol == 17) && 
             /* UDP�̃p�P�b�g�̂Ƃ� */
             (speech_packet -> udp_dst_port == src_port) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��|�[�g�ԍ������M���|�[�g�ԍ��Ɉ�v�����Ƃ� */
             (speech_packet_error_check(packet) != 1)
             /* �p�P�b�g�ɃG���[���Ȃ��Ƃ� */
            )
         {
            packet_type = 's'; /* packet_type��'s'�ɂ��� */
         }
      }
      
      switch (packet_type) /* packet_type�ɂ��������ď������s�� */
      {
         case 'a': /* packet_type��'a'�̂Ƃ� */
            ARP_reply(packet); /* ARP���v���C */
            break;
         case 's': /* packet_type��'s'�̂Ƃ� */
            speech_packet = (SPEECH_PACKET *)packet; /* packet��SPEECH_PACKET�\���̂ɓ��Ă͂߂� */
            if (da_buffer_flag == 0) /* da_buffer_flag��0�̂Ƃ� */
            {
               for (i = 0; i < 160; i++)
               {
                  da_buffer1[i] = speech_packet -> speech_data[i];
                  /* �p�P�b�g�ɓ��ڂ���Ă��鉹���f�[�^��da_buffer1�Ɋi�[���� */
               }
            }
            else if (da_buffer_flag == 1) /* da_buffer_flag��1�̂Ƃ� */
            {
               for (i = 0; i < 160; i++)
               {
                  da_buffer0[i] = speech_packet -> speech_data[i];
                  /* �p�P�b�g�ɓ��ڂ���Ă��鉹���f�[�^��da_buffer0�Ɋi�[���� */
               }
            }
            break;
      }
   }
}
