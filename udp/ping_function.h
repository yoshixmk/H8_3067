void ping_reply(unsigned char *packet)
{
   unsigned short i;
   unsigned long sum;
   PING_PACKET *ping_packet;
   
   ping_packet = (PING_PACKET *)packet; /* packet��PING_PACKET�\���̂ɓ��Ă͂߂� */
   
   for (i = 0; i < 6; i++)
   {
      ping_packet -> eth_dst_MAC[i] = ping_packet -> eth_src_MAC[i];
      ping_packet -> eth_src_MAC[i] = src_MAC[i];
      /* ����MAC�A�h���X�Ƒ��M��MAC�A�h���X�����ւ��� */
   }
   for (i = 0; i < 4; i++)
   {
      ping_packet -> ip_dst_IP[i] = ping_packet -> ip_src_IP[i];
      ping_packet -> ip_src_IP[i] = src_IP[i];
      /* ����IP�A�h���X�Ƒ��M��IP�A�h���X�����ւ��� */
   }
   ping_packet -> ping_type = 0; /* ���b�Z�[�W�̎�ށiping���v���C�j */
   
   /* ���M���ɂ�����IP�w�b�_�̃`�F�b�N�T���̌v�Z */
   ping_packet -> ip_checksum = 0x0000; /* �`�F�b�N�T���̏����l��0x0000�ɂ��� */
   sum = ones_complement_sum(packet, 14, 20); /* 1�̕␔�a�iIP�w�b�_�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   ping_packet -> ip_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   /* ���M���ɂ�����ping���b�Z�[�W�̃`�F�b�N�T���̌v�Z */
   ping_packet -> ping_checksum = 0x0000; /* �`�F�b�N�T���̏����l��0x0000�ɂ��� */
   sum = ones_complement_sum(packet, 34, 72); /* 1�̕␔�a�iping���b�Z�[�W�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   ping_packet -> ping_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   packet_send(packet, (14 + 20 + 72)); /* �p�P�b�g�𑗐M���� */
}

void ping_request(unsigned char *packet, unsigned char count)
{
   unsigned short i,j=0;
   unsigned long sum;
   PING_PACKET *ping_packet;
   
   ping_packet = (PING_PACKET *)packet; /* packet��PING_PACKET�\���̂ɓ��Ă͂߂� */
   /* �C�[�T�l�b�g�E�w�b�_�i14byte�j */
   for (i = 0; i < 6; i++)
   {
      ping_packet -> eth_dst_MAC[i] = dst_MAC[i];
      ping_packet -> eth_src_MAC[i] = src_MAC[i];
      /* ����MAC�A�h���X�Ƒ��M��MAC�A�h���X���Z�b�g���� */
   }
   ping_packet -> eth_ethernet_type = 0x0800; /* Ether type (IP)���Z�b�g���� */

   /* IP�w�b�_�i20byte�j */
   ping_packet -> ip_version_length = 0x45; /* Ver=4 Length=20bytes */
   ping_packet -> ip_service_type = 0x00; /* ToS=0(normal) */
   ping_packet -> ip_total_length = 0x003c; /* TotalLength=60bytes */
   ping_packet -> ip_id = 0x0000 + count; /* ID=1,2,3,4 */
   ping_packet -> ip_flags_fragment_offset = 0x00; /* Flags=0(Don't Fragment) */
   ping_packet -> ip_time_to_live = 0x80; /* TTL=128 */
   ping_packet -> ip_protocol = 0x01; /* Protocol=1(ICMP) */
   for (i = 0; i < 4; i++)
   {
      ping_packet -> ip_dst_IP[i] = dst_IP[i];
      ping_packet -> ip_src_IP[i] = src_IP[i];
      /* ����IP�A�h���X�Ƒ��M��IP�A�h���X���Z�b�g���� */
   }
   /* ping���b�Z�[�W�i40byte�j */
   ping_packet -> ping_type = 0x08; /* Type=8(Echo request) */
   ping_packet -> ping_code = 0x00; /* Code=0 */
   ping_packet -> ping_id = 0x0001; /* ID=1 */
   ping_packet -> ping_sequence_number = 0x0000 + count; /* Seq=1,2,3,4 */
   for(i=0;i<32;i++){
      if(i>=23) {
         j=i-23;
      }else{
         j=i;
      }
      ping_packet -> ping_data[i] = 'a'+j;
   }


   
   /* ���M���ɂ�����IP�w�b�_�̃`�F�b�N�T���̌v�Z */
   ping_packet -> ip_checksum = 0x0000; /* �`�F�b�N�T���̏����l��0x0000�ɂ��� */
   sum = ones_complement_sum(packet, 14, 20); /* 1�̕␔�a�iIP�w�b�_�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   ping_packet -> ip_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   /* ���M���ɂ�����ping���b�Z�[�W�̃`�F�b�N�T���̌v�Z */
   ping_packet -> ping_checksum = 0x0000; /* �`�F�b�N�T���̏����l��0x0000�ɂ��� */
   sum = ones_complement_sum(packet, 34, 40); /* 1�̕␔�a�iping���b�Z�[�W�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   ping_packet -> ping_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   packet_send(packet, (14 + 20 + 40)); /* �p�P�b�g�𑗐M���� */
}
