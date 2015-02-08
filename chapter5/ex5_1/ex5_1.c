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
   char key, previous_key;
   unsigned char on_switch, previous_on_switch;
   unsigned char cursor, line;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   
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
   for (i = 0; i < 16; i++)
   {
      text_buffer[i] = ' '; /* �e�L�X�g�E�o�b�t�@���󔒂ŃN���A���� */
   }
   text_buffer[16] = 0x00; /* �e�L�X�g�E�o�b�t�@�̍Ō���i�������ɂ��� */
   cursor = 0; /* cursor�̏����l */
   line = 0; /* line�̏����l */
   previous_key = 0; /* previous_key�̏����l */
   previous_on_switch = 1; /* previous_on_switch�̏����l */
   
   while (1) /* �������[�v */
   {
      while (cursor < 16) /* cursor��16�����������Ƃ� */
      {
         delay_ms(20);
         /* 20ms�̎��ԑ҂����͂��ނ��ƂŁC�`���^�����O�ɂ��X�C�b�`�̌댟�o��������� */
         
         key = matrix_key_read(); /* ���݂̃L�[���`�F�b�N���� */
         
         if ((key != 0) && (key != previous_key))
         /* ���݂̃L�[��0�ł͂Ȃ��C���O�̃L�[�Ƃ͈قȂ�Ƃ� */
         {
            if ((cursor == 0) && (line == 0)) /* cursor��0�Cline��0�̂Ƃ� */
            {
               LCD_clear(); /* �\���N���A */
               LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
            }
            else if ((cursor == 0) && (line == 1)) /* cursor��0�Cline��1�̂Ƃ� */
            {
               LCD_control(0xC0); /* �J�[�\����1�s�ڂ̐擪�Ɉړ����� */
            }
            LCD_display(key); /* �t���f�B�X�v���C�Ɍ��݂̃L�[��\������ */
            text_buffer[cursor] = key; /* key��text_buffer�Ɋi�[���� */
            cursor++; /* cursor���C���N�������g���� */
         }
         previous_key = key;
         /* ���݂̃L�[���C���̃`�F�b�N�����ɂ����钼�O�̃L�[�Ƃ��Ē�`���� */
         
         if ((P5DR & 0x02) == 0x00) /* ���݂�ON�X�C�b�`���I���̂Ƃ� */
         {
            on_switch = 0; /* on_switch��0�ɂ��� */
         }
         else if ((P5DR & 0x02) == 0x02) /* ���݂�ON�X�C�b�`���I�t�̂Ƃ� */
         {
            on_switch = 1; /* on_switch��1�ɂ��� */
         }
         
         if ((on_switch == 0) && (on_switch != previous_on_switch))
         /* ���݂�ON�X�C�b�`���I���ŁC���O��ON�X�C�b�`�̏�ԂƂ͈قȂ�Ƃ� */
         {
            text_packet_send(packet); /* �e�L�X�g�E�f�[�^�̃p�P�b�g�𑗐M���� */
            src_ip_id++; /* ���̃p�P�b�g�̂��߂ɁCsrc_ip_id���C���N�������g���� */
            for (i = 0; i < 16; i++)
            {
               text_buffer[i] = ' '; /* �e�L�X�g�E�o�b�t�@���󔒂ŃN���A���� */
            }
            cursor = 0; /* cursor��0�ɖ߂� */
            line ^= 1; /* XOR���Z�ɂ���āCline��0�̂Ƃ���1�Cline��1�̂Ƃ���0�ɂ��� */
         }
         previous_on_switch = on_switch;
         /* ���݂�ON�X�C�b�`�̏�Ԃ��C���̃`�F�b�N�����ɂ�����ON�X�C�b�`�̏�ԂƂ��Ē�`���� */
      }
      
      /* cursor��16�ȏ�ɂȂ����Ƃ� */
      text_packet_send(packet); /* �e�L�X�g�E�f�[�^�̃p�P�b�g�𑗐M���� */
      src_ip_id++; /* ���̃p�P�b�g�̂��߂ɁCsrc_ip_id���C���N�������g���� */
      for (i = 0; i < 16; i++)
      {
         text_buffer[i] = ' '; /* �e�L�X�g�E�o�b�t�@���󔒂ŃN���A���� */
      }
      cursor = 0; /* cursor��0�ɖ߂� */
      line ^= 1; /* XOR���Z�ɂ���āCline��0�̂Ƃ���1�Cline��1�̂Ƃ���0�ɂ��� */
   }
}
