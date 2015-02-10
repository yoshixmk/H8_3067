#include <sci.h>
#include <reg3067.h>
#include <machine.h>
#include <RTL8019AS_register.h>
#include <string.h>

unsigned char src_IP[4];
unsigned char src_MAC[6];
unsigned char dst_IP[4];
unsigned char packet[256];

typedef struct{
	/*イーサネットヘッダ(14byte)*/
	unsigned char eth_dst_MAC[6];
	unsigned char eth_src_MAC[6];
	unsigned short eth_ethernet_type;
	/* ARPメッセージ(28byte) */
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
	/* イーサネット・ヘッダ(14byte) */
	unsigned char eth_dst_MAC[6];
	unsigned char eth_src_MAC[6];
	unsigned short eth_ethernet_type;

	/* IPヘッダ(20byte) */
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

	/* pingメッセージ(40byte) */
	unsigned char ping_type;
	unsigned char ping_code;
	unsigned short ping_checksum;
	unsigned short ping_id;
	unsigned short ping_sequence_number;
	unsigned char ping_data[32];
} PING_PACKET;

void ms_timer(unsigned short ms)
{
	int j;

	TSTR |= 0x04;
	for(j=0;j<ms;j++){
		while((TISRA & 0x04) != 0x04);
		TISRA &= 0xFB;
	}
	TSTR &= 0xFB;
}

void us_timer(unsigned short us)
{
	int j;

	TSTR |= 0x02;
	for(j=0;j<us;j++){
		while((TISRA & 0x02) != 0x02);
		TISRA &= 0xFD;
	}
	TSTR &= 0xFD;
}

char matrix_key_read(void)
{
	char key;
	unsigned char column, row;

	key = 0;
	column = 0x10;

	while(column <= 0x40)
	{
		P4DR = ~column;

		row =(~P4DR) & 0x0f;

		if(row != 0)
		{
			switch(column | row)
			{
				case 0x11 : key ='1'; break;
				case 0x12 : key ='2'; break;
				case 0x14 : key ='3'; break;
				case 0x18 : key ='4'; break;

				case 0x21 : key ='5'; break;
				case 0x22 : key ='6'; break;
				case 0x24 : key ='7'; break;
				case 0x28 : key ='8'; break;

				case 0x41 : key ='9'; break;
				case 0x42 : key ='.'; break;
				case 0x44 : key ='0'; break;
				case 0x48 : key ='#'; break;

				default : key = 0; break;
			}
		}
		column = column << 1;
	}
	return key;
}



void LCD_write(char data, char RS)
{
	PBDR = data & 0xF0;
	if(RS == 1)
	{
		PBDR |= 0x02;/*RSに1をセットする*/
	}
	else
	{
		PBDR &= 0xFD;/*RSに0をセットする*/
	}

	ms_timer(1);/*40ns以上の時間待ち*/
	PBDR |= 0x01;/*Eに1をセットする*/
	ms_timer(1);/*230ns以上待ち*/
	PBDR &= 0xFE;/*Eに0をセットする*/
}

void LCD_display(char code)
{
	LCD_write(code,1);
	LCD_write(code << 4,1);
	ms_timer(1);/*40us以上待ち*/
}

void LCD_control(char code)
{
	LCD_write(code, 0);
	LCD_write(code << 4, 0);
	ms_timer(1);/*40us以上待ち*/
}

void LCD_init(void)
{
	ms_timer(15);
	LCD_write(0x30,0);
	ms_timer(1);
	LCD_write(0x30,0);
	ms_timer(1);
	LCD_write(0x30,0);
	ms_timer(5);
	LCD_write(0x20,0);
	ms_timer(1);
	LCD_control(0x28);
	LCD_control(0x08);
	LCD_control(0x0C);
	LCD_control(0x06);
	LCD_control(0x01);
}
unsigned char NIC_read(address)
{
	unsigned char data;
	P1DR = address;
	P3DDR = 0x00;
	P6DR &= 0xEF;
	data = P3DR;
	P6DR |= 0x10;
	return data;
}
void NIC_write(unsigned char address, unsigned char data)
{
	P1DR = address;
	P3DDR = 0xFF;
	P6DR &= 0xDF;
	P3DR = data;
	P6DR |= 0x20;
}
void NIC_init(void)
{
	unsigned short i;
	unsigned char data;
	PADR |= 0x80;
	ms_timer(10);
	PADR &= 0x7F;
	ms_timer (10);
	data = NIC_read(RP);
	NIC_write(RP, data);
	ms_timer (10);
	NIC_write(CR, 0x21);
	NIC_write(DCR, 0x4A);
	NIC_write(RBCR0, 0);
	NIC_write(RBCR1, 0);
	NIC_write(RCR, 0x20);
	NIC_write(TCR, 0x02);
	NIC_write(TPSR, 0x40);
	NIC_write(PSTART, 0x46);
	NIC_write(BNRY, 0x46);
	NIC_write(PSTOP, 0x60);
	NIC_write(IMR, 0x00);
	NIC_write(ISR, 0xFF);
	NIC_write(RBCR0, 12);
	NIC_write(RBCR1, 0);
	NIC_write(RSAR0, 0x00);
	NIC_write(RSAR1, 0x00);
	NIC_write(CR, 0x0A);
	for (i = 0; i < 6; i += 2) {
		src_MAC[i + 1] = NIC_read(RDMAP);
		NIC_read(RDMAP);
		src_MAC[i] = NIC_read(RDMAP);
		NIC_read(RDMAP);
	}
	do {
		data = NIC_read(ISR);
	} while ((data & 0x40) == 0x00);
	NIC_write(CR, 0x61);
	NIC_write(PAR0, src_MAC[0]);
	NIC_write(PAR1, src_MAC[1]);
	NIC_write(PAR2, src_MAC[2]);
	NIC_write(PAR3, src_MAC[3]);
	NIC_write(PAR4, src_MAC[4]);
	NIC_write(PAR5, src_MAC[5]);
	NIC_write(CURR, 0x47);
	NIC_write(MAR0, 0);
	NIC_write(MAR1, 0);
	NIC_write(MAR2, 0);
	NIC_write(MAR3, 0);
	NIC_write(MAR4, 0);
	NIC_write(MAR5, 0);
	NIC_write(MAR6, 0);
	NIC_write(MAR7, 0);
	NIC_write(CR, 0x21);
	NIC_write(RCR, 0x04);
	NIC_write(CR, 0x22);
	NIC_write(TCR, 0x00);
	NIC_write(IMR, 0x00);
}

void MAC_to_str(unsigned char *MAC, char *str)
{
	unsigned short i, n;
	unsigned char nibble;
	n = 0;
	for(i = 0; i < 6; i++)
	{
		nibble = (MAC[i] & 0xF0) >> 4;
		if (nibble < 10)
		{
			str[n] = '0' + nibble;
		}
		else
		{
			str[n] = 'A' + nibble - 10;
		}
		n++;
		nibble = MAC[i] & 0x0F;
		if (nibble < 10)
		{
			str[n] = '0' + nibble;
		}
		else
		{
			str[n] = 'A' + nibble - 10;
		}
		n++;
	}
	str[n] = 0x00;
}

void LCD_print(char *str)
{
	for( ; *str!= '\0'; str++)
	{
		LCD_display(*str);
	}
}
void buzzer(void) /*using while()*/
{
	char key;
	key=matrix_key_read();
	PADR |= 0x10;
	switch(key){
		case '1' :
			us_timer(956);
			PADR &= 0xEF;
			us_timer(956); break;
		case '2' :
			us_timer(852);
			PADR &= 0xEF;
			us_timer(852); break;
		case '3' :
			us_timer(759);
			PADR &= 0xEF;
			us_timer(759); break;
		case '4' :
			us_timer(716);
			PADR &= 0xEF;
			us_timer(716); break;
		case '5' :
			us_timer(638);
			PADR &= 0xEF;
			us_timer(638); break;
		case '6' :
			us_timer(568);
			PADR &= 0xEF;
			us_timer(568); break;
		case '7' :
			us_timer(506);
			PADR &= 0xEF;
			us_timer(506); break;
		case '8' :
			us_timer(478);
			PADR &= 0xEF;
			us_timer(478); break;
		case '9' :
			us_timer(423);
			PADR &= 0xEF;
			us_timer(423); break;
		case '0' :
			us_timer(379);
			PADR &= 0xEF;
			us_timer(379); break;
		default : key = 0; break;
	}
}
void IP_to_str(unsigned char *IP, char *str)
{
	unsigned short i, n;
	n = 0;
	for (i = 0; i < 4; i++)
	{
		str[n] = '0' + (IP[i] / 100);
		n++;
		str[n] = '0' + (IP[i] % 100 / 10);
		n++;
		str[n] = '0' + (IP[i] % 10);
		if (i < 3)
		{
			n++;
			str[n] = '.';
		}
		n++;
	}
	str[n] = 0x00;
}
void packet_send(unsigned char *packet, unsigned short size)
{
	unsigned short i;
	unsigned char data;
	unsigned char size_H, size_L;
	size_L = (unsigned char)(size & 0x00FF);
	size_H = (unsigned char)(size >> 8);
	NIC_write(CR, 0x22);
	NIC_write(RBCR0, size_L);
	NIC_write(RBCR1, size_H);
	NIC_write(RSAR0, 0x00);
	NIC_write(RSAR1, 0x40);
	NIC_write(CR, 0x12);
	for (i = 0; i < size; i++)
	{
		NIC_write(RDMAP,packet[i]);
	}
	do
	{
		data = NIC_read(ISR);
	} while ((data & 0x40) == 0x00);
	NIC_write(CR, 0x22);
	NIC_write(TBCR0, size_L);
	NIC_write(TBCR1, size_H);
	NIC_write(TPSR, 0x40);
	NIC_write(CR, 0x26);
	do
	{
		data = NIC_read(CR);
	}while ((data & 0x04) == 0x04);
}
unsigned char packet_receive(unsigned char *packet)
{
	unsigned short i;
	unsigned short size;
	unsigned char data;
	unsigned char size_H,size_L;
	unsigned char boundary_page,start_page,current_page;
	unsigned char header[4];

	NIC_write(CR,0x22);
	boundary_page = NIC_read(BNRY);
	NIC_write(CR,0x62);
	current_page = NIC_read(CURR);

	if(current_page < boundary_page){
		current_page += (0x60 - 0x46);
	}
	if(current_page == boundary_page + 1){
		return 1;
	}
	start_page = boundary_page + 1;
	if(start_page == 0x60){
		start_page = 0x46;
	}

	NIC_write(CR,0x22);
	NIC_write(RBCR0,4);
	NIC_write(RBCR1,0);
	NIC_write(RSAR0,0);
	NIC_write(RSAR1,start_page);
	NIC_write(CR,0x0A);
	for(i=0;i<4;i++){
		header[i] = NIC_read(RDMAP);
	}
	do{
		data =NIC_read(ISR);
	}while((data & 0x40) == 0x00);

	NIC_write(CR,0x22);
	size_L = header[2];
	size_H = header[3];
	size = ((unsigned short)size_H << 8) + (unsigned short)size_L;
	NIC_write(RBCR0,size_L);
	NIC_write(RBCR1,size_H);
	NIC_write(RSAR0,0);
	NIC_write(RSAR1,start_page);
	NIC_write(CR, 0x0A);
	for(i=0;i<4;i++){
	    NIC_read(RDMAP);
    }
    for (i = 0; i < (size - 4); i++)
    {
		packet[i] = NIC_read(RDMAP);
		if(i >= 256){
			NIC_read(RDMAP);
		}
	}
	do{
		data = NIC_read(ISR);
	}while((data & 0x40) == 0x00);

	NIC_write(CR,0x22);
	boundary_page = current_page - 1;
	if(boundary_page >= 0x60){
		boundary_page -= (0x60 - 0x46);
	}
	NIC_write(BNRY,boundary_page);

	return 0;
}
void ARP_request(unsigned char *packet)
{
	unsigned short i;
	ARP_PACKET *arp_packet;
	arp_packet = (ARP_PACKET *)packet;
	for (i = 0; i< 6; i++)
	{
		arp_packet -> eth_dst_MAC[i] =0xFF;
	}
	for (i = 0; i < 6; i++)
	{
		arp_packet -> eth_src_MAC[i] = src_MAC[i];
	}
	arp_packet -> eth_ethernet_type = 0x0806;
	arp_packet -> arp_hardware_type = 0x0001;
	arp_packet -> arp_protocol_type = 0x0800;
	arp_packet -> arp_hardware_length = 6;
	arp_packet -> arp_protocol_length = 4;
	arp_packet -> arp_operation = 1;

	for (i = 0; i < 6; i++)
	{
		arp_packet -> arp_src_MAC[i] = src_MAC[i];
	}
	for (i = 0; i < 4; i++)
	{
		arp_packet -> arp_src_IP[i] = src_IP[i];
	}
	for (i = 0; i < 6; i++)
	{
		arp_packet -> arp_dst_MAC[i] = 0x00;
	}
	for (i = 0; i < 4; i++)
	{
		arp_packet -> arp_dst_IP[i] = dst_IP[i];
	}
	packet_send(packet, 60);
}
void ARP_reply(unsigned char *packet)
{
	unsigned short i;
	ARP_PACKET *arp_packet;

	arp_packet = (ARP_PACKET *)packet;

	for (i = 0; i < 6; i++){
		arp_packet -> eth_dst_MAC[i] = arp_packet -> eth_src_MAC[i];
		arp_packet -> eth_src_MAC[i] = src_MAC[i];
	}
	arp_packet -> arp_operation = 2;
	for (i = 0; i < 6; i++){
		arp_packet -> arp_dst_MAC[i] = arp_packet -> arp_src_MAC[i];
		arp_packet -> arp_src_MAC[i] = src_MAC[i];
	}
	for (i = 0; i < 4; i++){
		arp_packet -> arp_dst_IP[i] = arp_packet -> arp_src_IP[i];
		arp_packet -> arp_src_IP[i] = src_IP[i];
	}

	packet_send(packet, 60);
}

void Ping_reply(unsigned char *packet)
{
	unsigned short i;
	unsigned long sum;
	PING_PACKET *ping_packet;

	ping_packet = (PING_PACKET *)packet;

	for (i = 0; i < 6; i++){
		ping_packet -> eth_dst_MAC[i] = ping_packet -> eth_src_MAC[i];
		ping_packet -> eth_src_MAC[i] = src_MAC[i];
	}
	for (i = 0; i < 4; i++){
		ping_packet -> ip_dst_IP[i] = ping_packet -> ip_src_IP[i];
		ping_packet -> ip_src_IP[i] = src_IP[i];
	}
	ping_packet -> ping_type = 0; /*pingリプライ*/

	/* 送信側におけるIPヘッダのチェックサムの計算 */
	ping_packet -> ip_checksum = 0x0000; /* チェックサムの初期値を0x0000にする */
	sum = ones_complement_sum(packet, 14, 20); /* 1の補数和(IPヘッダ) */
	sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
	ping_packet -> ip_checksum = (unsigned short)sum; /* 計算結果をセットする */

	/* 送信側におけるpingメッセージのチェックサムの計算 */
	ping_packet -> ping_checksum = 0x0000; /* チェックサムの初期値を0x0000にする */
	sum = ones_complement_sum(packet, 34, 40); /* 1の補数和(pingメッセージ) */
	sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
	ping_packet -> ping_checksum = (unsigned short)sum; /* 計算結果をセットする */

	packet_send(packet, (14 + 20 + 40)); /* パケットを送信する */
}
void main(void)
{
	char str_src_MAC[13];
	char str_dst_IP[16];
	int i;
	ARP_PACKET *arp_packet;
	PING_PACKET *ping_packet;
	P1DDR |= 0x1F;
	P6DDR |= 0x30;
	PADDR |= 0x80;
	P4DDR = 0xF0;
	P4PCR = 0x0F;
	PBDDR = 0xF3;

	TCR1 = 0xA0;
	TCR2 = 0xA3;
	GRA1 = 0x13;
	GRA2 = 2499;
	TISRA |= 0x40;
	PADR = 0xFB;
	TSTR |= 0x04;
	src_IP[0] = 10;
	src_IP[1] = 1;
	src_IP[2] = 69;
	src_IP[3] = 139;
	dst_IP[0] = 10;
	dst_IP[1] = 1;
	dst_IP[2] = 69;
	dst_IP[3] = 149;
    NIC_init();
    LCD_init();
    ms_timer(3000);
	/*ARP_request(packet);*/
	while(1){/*エラー解消から。コメントの文字コードエラーっぽい*/
        if(packet_receive(packet) == 0){
            arp_packet = (ARP_PACKET *)packet;
            ping_packet = (PING_PACKET *)packet;
            if(arp_packet -> eth_ethernet_type == 0x0806 && (strcmp(arp_packet -> arp_dst_IP, src_IP) == 0)){
                IP_to_str(arp_packet -> arp_src_IP, str_dst_IP);
                MAC_to_str(arp_packet -> arp_src_MAC, str_src_MAC);
                ARP_reply(packet);

                LCD_print(str_dst_IP);
                LCD_control(0xc0);
                LCD_print(str_src_MAC);
            }
            else if(ping_packet -> eth_ethernet_type == 0x0800 && (strcmp(arp_packet -> arp_dst_IP, src_IP) == 0)) && ping_packet -> ping_type == 8){
                LCD_display('!');
                Ping_reply(packet);
            }
        }
    }

    while(1);
}