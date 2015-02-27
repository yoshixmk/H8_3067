unsigned char src_IP[4]; /* ���M��IP�A�h���X */
unsigned char src_MAC[6]; /* ���M��MAC�A�h���X */
unsigned char dst_IP[4]; /* ����IP�A�h���X */
unsigned char dst_MAC[6]; /* ����MAC�A�h���X */
unsigned char packet[256]; /* ����M�p�P�b�g�p */
typedef struct {
	/* �C�[�T�l�b�g�E�w�b�_�i14byte�j */
	unsigned char eth_dst_MAC[6];
	unsigned char eth_src_MAC[6];
	unsigned short eth_ethernet_type;

	/* ARP���b�Z�[�W�i28byte�j */
	unsigned short arp_hardware_type;
	unsigned short arp_protocol_type;
	unsigned char arp_hardware_length;
	unsigned char arp_protocol_length;
	unsigned short arp_operation;
	unsigned char arp_src_MAC[6];
	unsigned char arp_src_IP[4];
	unsigned char arp_dst_MAC[6];
	unsigned char arp_dst_IP[4];
} ARP_PACKET;

typedef struct
{
	/* �C�[�T�l�b�g�E�w�b�_�i14byte�j */
	unsigned char eth_dst_MAC[6];
	unsigned char eth_src_MAC[6];
	unsigned short eth_ethernet_type;

	/* IP�w�b�_�i20byte�j */
	unsigned char ip_version_length;
	unsigned char ip_service_type;
	unsigned short ip_total_length;
	unsigned short ip_id;
	unsigned short ip_flags_fragment_offset;
	unsigned char ip_time_to_live;
	unsigned char ip_protocol;
	unsigned short ip_checksum;
	unsigned char ip_src_IP[4];
	unsigned char ip_dst_IP[4];

	/* ping���b�Z�[�W�i40byte�j */
	unsigned char ping_type;
	unsigned char ping_code;
	unsigned short ping_checksum;
	unsigned short ping_id;
	unsigned short ping_sequence_number;
	unsigned char ping_data[32];/*�f�[�^�T�C�Y��32byte�̏ꍇ*/
} PING_PACKET;

typedef struct
{
	unsigned char eth_dst_MAC[6];
	unsigned char eth_src_MAC[6];
	unsigned short eth_ethernet_type;

	unsigned char ip_version_length;
	unsigned char ip_service_type;
	unsigned short ip_total_length;
	unsigned short ip_id;
	unsigned short ip_flags_fragment_offset;
	unsigned char ip_time_to_live;
	unsigned char ip_protocol;
	unsigned short ip_checksum;
	unsigned char ip_src_IP[4];
	unsigned char ip_dst_IP[4];

	unsigned short udp_src_port;
	unsigned short udp_dst_port;
	unsigned short udp_length;
	unsigned short udp_checksum;

	/* �e�L�X�g�E�f�[�^(16byte) */
	char text_data[16];
} UDP_PACKET;

#include "H8_register.h"
#include "H8_register_function.h"
#include "RTL8019AS_register.h"
#include "timer_function.h"
#include "RTL8019AS_function.h"
#include "lcd_function.h"
#include "matrix_key_function.h"
#include "utility_function.h"
#include "ARP_function.h"
#include "ping_function.h"

void main(void)
{
	ARP_PACKET *arp_packet; /* ARP�p�P�b�g�\���� */
	PING_PACKET *ping_packet; /* ping�p�P�b�g�\���� */
	UDP_PACKET *udp_packet;
	unsigned char packet_type; /* �p�P�b�g�^�C�v����p */

	src_IP[0]=10;
	src_IP[1]=1;
	src_IP[2]=68;
	src_IP[3]=139;

	H8_register_init(); /* H8���W�X�^�������E�ݒ� */

	LCD_init(); /* LCD������ */

	NIC_init(); /* NIC������ */
	LCD_control(0x01);/*LCD clear*/
	LCD_control(0x80); /* �J�[�\����1�s�ڂ̐擪�Ɉړ����� */
	ms_timer(1000); /* Wait */

	while(1){
		packet_type = 0;

		if (packet_receive(packet) != 1) /* �p�P�b�g����M�����Ƃ� */
		{
			arp_packet = (ARP_PACKET *)packet; /* packet��ARP_PACKET�\���̂ɓ��Ă͂߂� */
			if ((arp_packet -> eth_ethernet_type == 0x0806) && 
				/* ARP�̃p�P�b�g�̂Ƃ� */
				(IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0) && 
				/* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
				(arp_packet -> arp_operation == 1)
				/* ARP Request�̂Ƃ� */
				)
			{
				packet_type = 'a'; /* packet_type��a�ɂ��� */
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
			udp_packet =  (UDP_PACKET *)packet;
            if((udp_packet -> eth_ethernet_type == 0x0800) && (IP_compare(udp_packet -> ip_dst_IP, src_IP) == 0) && (udp_packet -> udp_dst_port == 30000)){
                packet_type = 'u';
                UDP_text_receive(packet);
            }

			switch (packet_type) /* packet_type�ɂ��������ď������s�� */
			{
			case 'a': /* packet_type��'a'�̂Ƃ� */
				ARP_reply(packet); /* ARP���v���C */
				break;
			case 'p': /* packet_type��'p'�̂Ƃ� */
				LCD_display('!'); /* �t���f�B�X�v���C��packet_type��\������ */
				ping_reply(packet); /* ping���v���C */
				break;
			case 'u':
			    /*LCD_control(0xc0);*/
			    LCD_print(udp_packet -> text_data);
			    break;
			}
		}
	}
}
