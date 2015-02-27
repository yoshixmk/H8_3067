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
   sum = ones_complement_sum(packet, 34, 40); /* 1�̕␔�a�iping���b�Z�[�W�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   ping_packet -> ping_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   packet_send(packet, (14 + 20 + 40)); /* �p�P�b�g�𑗐M���� */
}
