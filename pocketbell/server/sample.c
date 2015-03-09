unsigned char src_IP[4]; /* ���M��IP�A�h���X */
unsigned char src_MAC[6]; /* ���M��MAC�A�h���X */
unsigned char dst_IP[4]; /* ����IP�A�h���X */
unsigned char dst_MAC[6]; /* ����MAC�A�h���X */
unsigned char packet[256]; /* ����M�p�P�b�g�p */

#include <string.h>
#include "H8_register.h"
#include "H8_register_function.h"
#include "RTL8019AS_register.h"
#include "timer_function.h"
#include "RTL8019AS_function.h"
#include "lcd_function.h"
#include "matrix_key_function.h"
#include "utility_function.h"
#include "global_variables.h"
#include "ARP_function.h"
#include "ping_function.h"

void main(void)
{
	ARP_PACKET *arp_packet; /* ARP�p�P�b�g�\���� */
	UDP_PACKET *udp_packet; /* ping�p�P�b�g�\���� */
	unsigned char packet_type; /* �p�P�b�g�^�C�v����p */
	char lcd_data[25];
	int i,j,k;
	/* ���M��IP�A�h���X  10.1.68.160*/
	src_IP[0]=10;
	src_IP[1]=1;
	src_IP[2]=69;
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
			udp_packet = (UDP_PACKET *)packet; /* packet��UDP_PACKET�\���̂ɓ��Ă͂߂� */
			if ((arp_packet -> eth_ethernet_type == 0x0806) && 
				/* ARP�̃p�P�b�g�̂Ƃ� */
				(IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0) && 
				/* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
				(arp_packet -> arp_operation == 1)
				/* ARP Request�̂Ƃ� */
				)
			{
				ARP_reply(packet);
			}
			if ((udp_packet -> eth_ethernet_type == 0x0800) && 
				/* IP�̃p�P�b�g�̂Ƃ� */
				(IP_compare(udp_packet -> ip_dst_IP, src_IP) == 0) && 
				/* �p�P�b�g�ɋL�q����Ă��鈶��IP�A�h���X�����M��IP�A�h���X�Ɉ�v�����Ƃ� */
				(udp_packet -> ip_protocol == 17) && 
				/* ICMP�̃p�P�b�g�̂Ƃ� */
				(udp_packet -> udp_dst_port == 30000)
				/* UDP�|�[�g��12345�̂Ƃ� */
				)
			{
				LCD_control(0x80);
				for(i=0;i<25;i++)
				{
					if(i<8)lcd_data[i] = packet[42+i];
					else lcd_data[i] = ' ';
				}
                LCD_print(lcd_data);
                LCD_control(0x80);
                ms_timer(1500);
			}
		}
	}
}
