#include "H8_register.h" /* H8/3067�̃��W�X�^ */
#include "RTL8019AS_register.h" /* RTL8019AS�̃��W�X�^ */
#include "symbol_definition.h" /* �萔�̒�` */
#include "packet_definition.h" /* �p�P�b�g�̒�` */
#include "global_variables.h" /* �O���[�o���ϐ��̒�` */
#include "H8_register_function.h" /* H8/3067�̃��W�X�^�̐ݒ� */
#include "delay_function.h" /* ���ԑ҂��̊֐� */
#include "LCD_function.h" /* SC1602B�̊֐� */
#include "NIC_function.h" /* RTL8019AS�̊֐� */
#include "matrix_key_function.h" /* �}�g���N�X�E�L�[�̊֐� */
#include "utility_function.h" /* ���̂ق��̊֐� */
#include "ARP_function.h" /* ARP�̊֐� */
#include "call_packet_function.h" /* �Đ���f�[�^�̃p�P�b�g�̊֐� */
#include "speech_packet_function.h" /* �����f�[�^�̃p�P�b�g�̊֐� */
#include <machine.h> /* set_imask_ccr�֐� */

/* sampling_8khz�֐��́C�^�C�}���荞�݂����������Ƃ��ɌĂяo�����֐� */
#pragma interrupt(sampling_8khz)
void sampling_8khz(void)
{
   TISRA &= 0xFE; /* IMFA0��0�ɖ߂� */
   
   switch (phone_state) /* phone_state�ɂ��������ď������s�� */
   {
      case OFFLINE : /* �I�t���C���̏�� */
         anti_chattering_counter++; /* anti_chattering_counter���C���N�������g���� */
         if (anti_chattering_counter == 160) /* anti_chattering_counter��160�ɂȂ����Ƃ� */
         {
            key = matrix_key_read(); /* ���݂̃L�[���`�F�b�N���� */
            if ((P5DR & 0x02) == 0x00) /* ���݂�ON�X�C�b�`���I���̂Ƃ� */
            {
               on_switch = 0; /* on_switch��0�ɂ��� */
            }
            else if ((P5DR & 0x02) == 0x02) /* ���݂�ON�X�C�b�`���I�t�̂Ƃ� */
            {
               on_switch = 1; /* on_switch��1�ɂ��� */
            }
            anti_chattering_counter = 0; /* anti_chattering_counter��0�ɖ߂� */
         }
         break;
      case ARP_REQUEST : /* ARP���N�G�X�g�̏�� */
         time_out_counter++; /* time_out_counter���C���N�������g���� */
         break;
      case OUTGOING : /* �Ăяo���̏�� */
         time_out_counter++; /* time_out_counter���C���N�������g���� */
         break;
      case INCOMING : /* ���M�̏�� */
         time_out_counter++; /* time_out_counter���C���N�������g���� */
         if ((time_out_counter & 0x2000) == 0) /* (time_out_counter & 0x2000)��0�̂Ƃ� */
         {
            PADR = 0x7F;
            /* PA2��1�iLED(D8)�͔������Ȃ��j�CPA3��1�iLED(D9)�͔������Ȃ��j�CPA4��1�i�u�U�[��5V�j */
         }
         else /* (time_out_counter & 0x2000)��1�̂Ƃ� */
         {
            if ((time_out_counter & 0x0200) == 0) /* (time_out_counter & 0x0200)��0�̂Ƃ� */
            {
               if ((time_out_counter & 0x0008) == 0) /* (time_out_counter & 0x0008)��0�̂Ƃ� */
               {
                  PADR = 0x7B;
                  /* PA2��0�iLED(D8)�͔�������j�CPA3��1�iLED(D9)�͔������Ȃ��j�CPA4��1�i�u�U�[��5V�j */
               }
               else /* (time_out_counter & 0x0008)��1�̂Ƃ� */
               {
                  PADR = 0x6B;
                  /* PA2��0�iLED(D8)�͔�������j�CPA3��1�iLED(D9)�͔������Ȃ��j�CPA4��0�i�u�U�[��0V�j */
               }
            }
            else /* (time_out_counter & 0x0200)��1�̂Ƃ� */
            {
               PADR = 0x77;
               /* PA2��1�iLED(D8)�͔������Ȃ��j�CPA3��0�iLED(D9)�͔�������j�CPA4��1�i�u�U�[��5V�j */
            }
         }
         
         anti_chattering_counter++; /* anti_chattering_counter���C���N�������g���� */
         if (anti_chattering_counter == 160) /* anti_chattering_counter��160�ɂȂ����Ƃ� */
         {
            if ((P5DR & 0x02) == 0x00) /* ���݂�ON�X�C�b�`���I���̂Ƃ� */
            {
               on_switch = 0; /* on_switch��0�ɂ��� */
            }
            else if ((P5DR & 0x02) == 0x02) /* ���݂�ON�X�C�b�`���I�t�̂Ƃ� */
            {
               on_switch = 1; /* on_switch��1�ɂ��� */
            }
            anti_chattering_counter = 0; /* anti_chattering_counter��0�ɖ߂� */
         }
         break;
      case ONLINE : /* �I�����C���̏�� */
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
         
         anti_chattering_counter++; /* anti_chattering_counter���C���N�������g���� */
         if (anti_chattering_counter == 160) /* anti_chattering_counter��160�ɂȂ����Ƃ� */
         {
            if ((P5DR & 0x01) == 0x00) /* ���݂�OFF�X�C�b�`���I���̂Ƃ� */
            {
               off_switch = 0; /* off_switch��0�ɂ��� */
            }
            else if ((P5DR & 0x01) == 0x01) /* ���݂�OFF�X�C�b�`���I�t�̂Ƃ� */
            {
               off_switch = 1; /* off_switch��1�ɂ��� */
            }
            anti_chattering_counter = 0; /* anti_chattering_counter��0�ɖ߂� */
         }
         break;
   }
}

void OFFLINE_state(void)
{
   unsigned short i;
   char str_to[] = "Call To"; /* �t���f�B�X�v���C�ɕ\�����镶���� */
   
   phone_state = OFFLINE; /* phone_state��OFFLINE�ɂ��� */
   anti_chattering_counter = 0; /* anti_chattering_counter�̏����l */
   cursor = 0; /* cursor�̏����l */
   for (i = 0; i < 16; i++)
   {
      str_IP[i] = ' '; /* str_IP���󔒂ŃN���A���� */
   }
   
   PADR = 0x7F; /* PA2��1�iLED(D8)�͔������Ȃ��j�CPA3��1�iLED(D9)�͔������Ȃ��j�CPA4��1�i�u�U�[��5V�j */
   LCD_clear(); /* �\���N���A */
   LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
   LCD_print(str_to); /* �t���f�B�X�v���C��str_to��\������ */
   LCD_control(0xC0); /* �J�[�\����1�s�ڂ̐擪�Ɉړ����� */
}

void ARP_REQUEST_state(void)
{
   phone_state = ARP_REQUEST; /* phone_state��ARP_REQUEST�ɂ��� */
   time_out_counter = 0; /* time_out_counter�̏����l */
}

void OUTGOING_state(void)
{
   phone_state = OUTGOING; /* phone_state��OUTGOING�ɂ��� */
   time_out_counter = 0; /* time_out_counter�̏����l */
}

void INCOMING_state(void)
{
   char str_from[] = "Call From"; /* �t���f�B�X�v���C�ɕ\�����镶���� */
   
   phone_state = INCOMING; /* phone_state��INCOMING�ɂ��� */
   anti_chattering_counter = 0; /* anti_chattering_counter�̏����l */
   time_out_counter = 0; /* time_out_counter�̏����l */
   
   LCD_clear(); /* �\���N���A */
   LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
   LCD_print(str_from); /* �t���f�B�X�v���C��str_from��\������ */
   LCD_control(0xC0); /* �J�[�\����1�s�ڂ̐擪�Ɉړ����� */
   IP_to_str(dst_IP, str_IP); /* dst_IP��str_IP�i������j�ɕϊ����� */
   LCD_print(str_IP); /* �t���f�B�X�v���C��str_IP��\������ */
}

void ONLINE_state(void)
{
   unsigned short i;
   
   phone_state = ONLINE; /* phone_state��ONLINE�ɂ��� */
   anti_chattering_counter = 0; /* anti_chattering_counter�̏����l */
   time_out_counter = 0; /* time_out_counter�̏����l */
   speech_packet_send_flag = 0; /* speech_packet_send_flag�̏����l */
   ad_buffer_flag = 0; /* ad_buffer_flag�̏����l */
   da_buffer_flag = 0; /* da_buffer_flag�̏����l */
   ad_counter = 0; /* ad_counter�̏����l */
   da_counter = 0; /* da_counter�̏����l */
   for (i = 0; i < 160; i++)
   {
      da_buffer0[i] = 128; /* da_buffer0�̏����l�i�����f�[�^�̃I�t�Z�b�g��128�j */
      da_buffer1[i] = 128; /* da_buffer1�̏����l�i�����f�[�^�̃I�t�Z�b�g��128�j */
   }
   src_rtp_sequence_number = 0; /* src_rtp_sequence_number�̏����l */
   src_rtp_time_stamp = 0; /* src_rtp_time_stamp�̏����l */
   src_rtp_ssrc = 0; /* src_rtp_ssrc�̏����l */
   PADR = 0x7F; /* PA2��1�iLED(D8)�͔������Ȃ��j�CPA3��1�iLED(D9)�͔������Ȃ��j�CPA4��1�i�u�U�[��5V�j */
}

void main(void)
{
   unsigned short i;
   unsigned char packet_type;
   ARP_PACKET *arp_packet;
   CALL_PACKET *call_packet;
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
   
   src_ip_id = 0; /* src_ip_id�̏����l */
   
   key = 0; /* key�̏����l */
   previous_key = 0; /* previous_key�̏����l */
   on_switch = 1; /* on_switch�̏����l */
   previous_on_switch = 1; /* previous_on_switch�̏����l */
   off_switch = 1; /* off_switch�̏����l */
   previous_off_switch = 1; /* previous_off_switch�̏����l */
   
   OFFLINE_state(); /* �I�t���C���̏�ԂɑJ�ڂ��� */
   
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
             (IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0)
             /* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
            )
         {
            packet_type = 'a'; /* packet_type��'a'�ɂ��� */
         }
         
         call_packet = (CALL_PACKET *)packet; /* packet��CALL_PACKET�\���̂ɓ��Ă͂߂� */
         if ((call_packet -> eth_ethernet_type == 0x0800) && 
             /* IP�̃p�P�b�g�̂Ƃ� */
             (IP_compare(call_packet -> ip_dst_IP, src_IP) == 0) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
             (call_packet -> ip_protocol == 17) && 
             /* UDP�̃p�P�b�g�̂Ƃ� */
             (call_packet -> udp_dst_port == 10000) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��|�[�g�ԍ���10000�Ԃł���Ƃ� */
             (call_packet_error_check(packet) != 1)
             /* �p�P�b�g�ɃG���[���Ȃ��Ƃ� */
            )
         {
            packet_type = 'c'; /* packet_type��'c'�ɂ��� */
         }
         
         speech_packet = (SPEECH_PACKET *)packet; /* packet��SPEECH_PACKET�\���̂ɓ��Ă͂߂� */
         if ((speech_packet -> eth_ethernet_type == 0x0800) && 
             /* IP�̃p�P�b�g�̂Ƃ� */
             (IP_compare(speech_packet -> ip_dst_IP, src_IP) == 0) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
             (speech_packet -> ip_protocol == 17) && 
             /* UDP�̃p�P�b�g�̂Ƃ� */
             (speech_packet -> udp_dst_port == 20000) && 
             /* �p�P�b�g�ɋL�q����Ă��鈶��|�[�g�ԍ���20000�Ԃł���Ƃ� */
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
            arp_packet = (ARP_PACKET *)packet; /* packet��ARP_PACKET�\���̂ɓ��Ă͂߂� */
            switch (arp_packet -> arp_operation) /* arp_operation�ɂ��������ď������s�� */
            {
               case 1 : /* ARP���N�G�X�g�̂Ƃ� */
                  ARP_reply(packet); /* ARP���v���C */
                  break;
               case 2 : /* ARP���v���C�̂Ƃ� */
                  for (i = 0; i < 6; i++)
                  {
                     dst_MAC[i] = arp_packet -> arp_src_MAC[i];
                     /* �p�P�b�g�ɋL�q����Ă��鑗�M��MAC�A�h���X��dst_MAC�Ɋi�[���� */
                  }
                  src_port = 10000; /* ���M���|�[�g�ԍ��̐ݒ� */
                  dst_port = 10000; /* ����|�[�g�ԍ��̐ݒ� */
                  call_packet_send(packet, CONNECT_REQUEST); /* �ڑ����N�G�X�g�𑗐M���� */
                  src_ip_id++; /* ���̃p�P�b�g�̂��߂ɁCsrc_ip_id���C���N�������g���� */
                  OUTGOING_state(); /* �Ăяo���̏�ԂɑJ�ڂ��� */
                  break;
            }
            break;
         case 'c': /* packet_type��'c'�̂Ƃ� */
            call_packet = (CALL_PACKET *)packet; /* packet��CALL_PACKET�\���̂ɓ��Ă͂߂� */
            switch (call_packet -> call_type) /* call_type�ɂ��������ď������s�� */
            {
               case CONNECT_REQUEST : /* �ڑ����N�G�X�g�̂Ƃ� */
                  for (i = 0; i < 4; i++)
                  {
                     dst_IP[i] = call_packet -> ip_src_IP[i];
                     /* �p�P�b�g�ɋL�q����Ă��鑗�M��IP�A�h���X��dst_IP�Ɋi�[���� */
                  }
                  for (i = 0; i < 6; i++)
                  {
                     dst_MAC[i] = call_packet -> eth_src_MAC[i];
                     /* �p�P�b�g�ɋL�q����Ă��鑗�M��MAC�A�h���X��dst_MAC�Ɋi�[���� */
                  }
                  INCOMING_state(); /* ���M�̏�ԂɑJ�ڂ��� */
                  break;
               case CONNECT_REPLY : /* �ڑ����v���C�̂Ƃ� */
                  ONLINE_state(); /* �I�����C���̏�ԂɑJ�ڂ��� */
                  break;
               case DISCONNECT_REQUEST : /* �ؒf���N�G�X�g�̂Ƃ� */
                  src_port = 10000; /* ���M���|�[�g�ԍ��̐ݒ� */
                  dst_port = 10000; /* ����|�[�g�ԍ��̐ݒ� */
                  call_packet_send(packet, DISCONNECT_REPLY); /* �ؒf���v���C�𑗐M���� */
                  src_ip_id++; /* ���̃p�P�b�g�̂��߂ɁCsrc_ip_id���C���N�������g���� */
                  OFFLINE_state(); /* �I�t���C���̏�ԂɑJ�ڂ��� */
                  break;
               case DISCONNECT_REPLY : /* �ؒf���v���C�̂Ƃ� */
                  break;
            }
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
      
      switch (phone_state) /* phone_state�ɂ��������ď������s�� */
      {
         case OFFLINE : /* �I�t���C���̏�� */
            if ((key != 0) && (key != previous_key))
            /* ���݂̃L�[��0�ł͂Ȃ��C���O�̃L�[�Ƃ͈قȂ�Ƃ� */
            {
               if ((key == '#') || (cursor >= 16))
               /* ���݂̃L�[��'#'�C�܂��́Ccursor��16�ȏ�̂Ƃ� */
               {
                  OFFLINE_state(); /* �I�t���C���̏�ԂɑJ�ڂ��� */
               }
               else
               {
                  LCD_display(key); /* �t���f�B�X�v���C�Ɍ��݂̃L�[��\������ */
                  str_IP[cursor] = key; /* key��str_IP�Ɋi�[���� */
                  cursor++; /* cursor���C���N�������g���� */
               }
            }
            previous_key = key;
            /* ���݂̃L�[���C���̃`�F�b�N�����ɂ����钼�O�̃L�[�Ƃ��Ē�`���� */
            
            if ((on_switch == 0) && (on_switch != previous_on_switch))
            /* ���݂�ON�X�C�b�`���I���ŁC���O��ON�X�C�b�`�̏�ԂƂ͈قȂ�Ƃ� */
            {
               if (str_to_IP(str_IP, dst_IP) != 1)
               /* str_IP�iIP�A�h���X�̕�����j��dst_IP�ɐ������ϊ��ł����Ƃ� */
               {
                  ARP_request(packet); /* ARP���N�G�X�g */
                  ARP_REQUEST_state(); /* ARP���N�G�X�g�̏�ԂɑJ�ڂ��� */
               }
               else
               {
                  OFFLINE_state(); /* �I�t���C���̏�ԂɑJ�ڂ��� */
               }
            }
            previous_on_switch = on_switch;
            /* ���݂�ON�X�C�b�`�̏�Ԃ��C���̃`�F�b�N�����ɂ�����ON�X�C�b�`�̏�ԂƂ��Ē�`���� */
            break;
         case ARP_REQUEST : /* ARP���N�G�X�g�̏�� */
            if (time_out_counter == 0xFFFF) /* �^�C���A�E�g�̂Ƃ� */
            {
               OFFLINE_state(); /* �I�t���C���̏�ԂɑJ�ڂ��� */
            }
            break;
         case OUTGOING : /* �Ăяo���̏�� */
            if (time_out_counter == 0xFFFF) /* �^�C���A�E�g�̂Ƃ� */
            {
               OFFLINE_state(); /* �I�t���C���̏�ԂɑJ�ڂ��� */
            }
            break;
         case INCOMING : /* ���M�̏�� */
            if (time_out_counter == 0xFFFF) /* �^�C���A�E�g�̂Ƃ� */
            {
               OFFLINE_state(); /* �I�t���C���̏�ԂɑJ�ڂ��� */
            }
            
            if ((on_switch == 0) && (on_switch != previous_on_switch))
            /* ���݂�ON�X�C�b�`���I���ŁC���O��ON�X�C�b�`�̏�ԂƂ͈قȂ�Ƃ� */
            {
               src_port = 10000; /* ���M���|�[�g�ԍ��̐ݒ� */
               dst_port = 10000; /* ����|�[�g�ԍ��̐ݒ� */
               call_packet_send(packet, CONNECT_REPLY); /* �ڑ����v���C�𑗐M���� */
               src_ip_id++; /* ���̃p�P�b�g�̂��߂ɁCsrc_ip_id���C���N�������g���� */
               ONLINE_state(); /* �I�����C���̏�ԂɑJ�ڂ��� */
            }
            previous_on_switch = on_switch;
            /* ���݂�ON�X�C�b�`�̏�Ԃ��C���̃`�F�b�N�����ɂ�����ON�X�C�b�`�̏�ԂƂ��Ē�`���� */
            break;
         case ONLINE : /* �I�����C���̏�� */
            if (speech_packet_send_flag == 1) /* speech_packet_send_flag��1�̂Ƃ� */
            {
               src_port = 20000; /* ���M���|�[�g�ԍ��̐ݒ� */
               dst_port = 20000; /* ����|�[�g�ԍ��̐ݒ� */
               speech_packet_send(packet); /* �����f�[�^�̃p�P�b�g�𑗐M���� */
               src_ip_id++; /* ���̃p�P�b�g�̂��߂ɁCsrc_ip_id���C���N�������g���� */
               speech_packet_send_flag = 0; /* speech_packet_send_flag��0�ɖ߂� */
               src_rtp_sequence_number++; /* src_rtp_sequence_number���C���N�������g���� */
               src_rtp_time_stamp += 160; /* src_rtp_time_stamp��160�����Z���� */
            }
            
            if ((off_switch == 0) && (off_switch != previous_off_switch))
            /* ���݂�OFF�X�C�b�`���I���ŁC���O��OFF�X�C�b�`�̏�ԂƂ͈قȂ�Ƃ� */
            {
               src_port = 10000; /* ���M���|�[�g�ԍ��̐ݒ� */
               dst_port = 10000; /* ����|�[�g�ԍ��̐ݒ� */
               call_packet_send(packet, DISCONNECT_REQUEST); /* �ؒf���N�G�X�g�𑗐M���� */
               src_ip_id++; /* ���̃p�P�b�g�̂��߂ɁCsrc_ip_id���C���N�������g���� */
               OFFLINE_state(); /* �I�t���C���̏�ԂɑJ�ڂ��� */
            }
            previous_off_switch = off_switch;
            /* ���݂�OFF�X�C�b�`�̏�Ԃ��C���̃`�F�b�N�����ɂ�����OFF�X�C�b�`�̏�ԂƂ��Ē�`���� */
            break;
      }
   }
}
