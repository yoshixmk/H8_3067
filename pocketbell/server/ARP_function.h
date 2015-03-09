void ARP_request(unsigned char *packet)
{
   unsigned short i;
   ARP_PACKET *arp_packet;
  
   arp_packet = (ARP_PACKET *)packet; /* packet��ARP_PACKET�\���̂ɓ��Ă͂߂� */
   
   for (i = 0; i < 6; i++)
   {
      arp_packet -> eth_dst_MAC[i] = 0xFF; /* �u���[�h�L���X�g�E�A�h���X */
   }
   for (i = 0; i < 6; i++)
   {
      arp_packet -> eth_src_MAC[i] = src_MAC[i]; /* ���M��MAC�A�h���X */
   }
   arp_packet -> eth_ethernet_type = 0x0806; /* ��ʃv���g�R���̎�ށiARP�j */
   arp_packet -> arp_hardware_type = 0x0001; /* �l�b�g���[�N�̕����}�̂̎�ށi�C�[�T�l�b�g�j */
   arp_packet -> arp_protocol_type = 0x0800; /* ��ʃv���g�R���̎�ށiIP�j */
   arp_packet -> arp_hardware_length = 6; /* �l�b�g���[�N�̕����}�̂̃A�h���X���iMAC�A�h���X�j */
   arp_packet -> arp_protocol_length = 4; /* ��ʃv���g�R���̃A�h���X���iIPv4�j */
   arp_packet -> arp_operation = 1; /* ARP�̓���iARP���N�G�X�g�j */
   for (i = 0; i < 6; i++)
   {
      arp_packet -> arp_src_MAC[i] = src_MAC[i]; /* ���M��MAC�A�h���X */
   }
   for (i = 0; i < 4; i++)
   {
      arp_packet -> arp_src_IP[i] = src_IP[i]; /* ���M��IP�A�h���X */
   }
   for (i = 0; i < 6; i++)
   {
      arp_packet -> arp_dst_MAC[i] = 0x00; /* ����MAC�A�h���X */
   }
   for (i = 0; i < 4; i++)
   {
      arp_packet -> arp_dst_IP[i] = dst_IP[i]; /* ����IP�A�h���X */
   }

   packet_send(packet, 60); /* �p�P�b�g�̃T�C�Y��60byte�Ɏw�肵�đ��M���� */
}

void ARP_reply(unsigned char *packet)
{
   unsigned short i;
   ARP_PACKET *arp_packet;
   
   arp_packet = (ARP_PACKET *)packet; /* packet��ARP_PACKET�\���̂ɓ��Ă͂߂� */
   
   for (i = 0; i < 6; i++)
   {
      arp_packet -> eth_dst_MAC[i] = arp_packet -> eth_src_MAC[i];
      arp_packet -> eth_src_MAC[i] = src_MAC[i];
      /* ����MAC�A�h���X�Ƒ��M��MAC�A�h���X�����ւ��� */
   }
   arp_packet -> arp_operation = 2; /* ARP�̓���iARP���v���C�j */
   for (i = 0; i < 6; i++)
   {
      arp_packet -> arp_dst_MAC[i] = arp_packet -> arp_src_MAC[i];
      arp_packet -> arp_src_MAC[i] = src_MAC[i];
      /* ����MAC�A�h���X�Ƒ��M��MAC�A�h���X�����ւ��� */
   }
   for (i = 0; i < 4; i++)
   {
      arp_packet -> arp_dst_IP[i] = arp_packet -> arp_src_IP[i];
      arp_packet -> arp_src_IP[i] = src_IP[i];
      /* ����IP�A�h���X�Ƒ��M��IP�A�h���X�����ւ��� */
   }
   
   packet_send(packet, 60); /* �p�P�b�g�̃T�C�Y��60byte�Ɏw�肵�đ��M���� */
}
