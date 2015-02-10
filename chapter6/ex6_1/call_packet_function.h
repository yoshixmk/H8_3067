void call_packet_send(unsigned char *packet, unsigned short call_type)
{
   unsigned short i;
   unsigned long sum;
   CALL_PACKET *call_packet;
   unsigned char header[12];
   PSEUDO_HEADER *pseudo_header;
   
   call_packet = (CALL_PACKET *)packet; /* packet��CALL_PACKET�\���̂ɓ��Ă͂߂� */
   
   for (i = 0; i < 6; i++)
   {
      call_packet -> eth_dst_MAC[i] = dst_MAC[i]; /* ����MAC�A�h���X */
   }
   for (i = 0; i < 6; i++)
   {
      call_packet -> eth_src_MAC[i] = src_MAC[i]; /* ���M��MAC�A�h���X */
   }
   call_packet -> eth_ethernet_type = 0x0800; /* ��ʃv���g�R���̎�ށiIP�j */
   call_packet -> ip_version_length = 0x45; /* IP�̃o�[�W������IP�w�b�_�̒��� */
   call_packet -> ip_service_type = 0; /* �p�P�b�g�̗D��x */
   call_packet -> ip_total_length = 20 + 8 + 2; /* IP�f�[�^�O�����̒��� */
   call_packet -> ip_id = src_ip_id; /* �p�P�b�g�̎��ʎq */
   call_packet -> ip_flags_fragment_offset = 0x4000; /* �p�P�b�g�̕����̗L���Ɩ{���̃f�[�^�ʒu */
   call_packet -> ip_time_to_live = 128; /* ���[�^�̒��p�񐔂̏�� */
   call_packet -> ip_protocol = 17; /* ��ʃv���g�R���̎�ށiUDP�j */
   call_packet -> ip_checksum = 0x0000; /* �`�F�b�N�T���̏����l */
   for (i = 0; i < 4; i++)
   {
      call_packet -> ip_src_IP[i] = src_IP[i]; /* ���M��IP�A�h���X */
   }
   for (i = 0; i < 4; i++)
   {
      call_packet -> ip_dst_IP[i] = dst_IP[i]; /* ����IP�A�h���X */
   }
   call_packet -> udp_src_port = src_port; /* ���M���|�[�g�ԍ� */
   call_packet -> udp_dst_port = dst_port; /* ����|�[�g�ԍ� */
   call_packet -> udp_length = 8 + 2; /* UDP�Z�O�����g�̒��� */
   call_packet -> udp_checksum = 0x0000; /* �`�F�b�N�T���̏����l */
   call_packet -> call_type = call_type; /* �Đ���̎�� */
   
   /* ���M���ɂ�����IP�w�b�_�̃`�F�b�N�T���̌v�Z */
   sum = ones_complement_sum(packet, 14, 20); /* 1�̕␔�a�iIP�w�b�_�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   call_packet -> ip_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   /* �[���w�b�_ */
   pseudo_header = (PSEUDO_HEADER *)header; /* header��PSEUDO_HEADER�\���̂ɓ��Ă͂߂� */
   for (i = 0; i < 4; i++)
   {
      pseudo_header -> ip_src_IP[i] = call_packet -> ip_src_IP[i]; /* ���M��IP�A�h���X */
      pseudo_header -> ip_dst_IP[i] = call_packet -> ip_dst_IP[i]; /* ����IP�A�h���X */
   }
   pseudo_header -> zero_padding = 0; /* �[���E�p�f�B���O */
   pseudo_header -> ip_protocol = call_packet -> ip_protocol; /* ��ʃv���g�R���̎�ށiUDP�j */
   pseudo_header -> udp_length = call_packet -> udp_length; /* UDP�Z�O�����g�̒��� */
   
   /* ���M���ɂ�����UDP�w�b�_�̃`�F�b�N�T���̌v�Z */
   sum = ones_complement_sum(header, 0, 12); /* 1�̕␔�a�i�[���w�b�_�j */
   sum += ones_complement_sum(packet, 34, (8 + 2)); /* 1�̕␔�a�iUDP�Z�O�����g�j */
   sum = (sum & 0xFFFF) + (sum >> 16); /* 1�̕␔�a */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   if (sum == 0x0000)
   {
      sum = 0xFFFF; /* �v�Z���ʂ�0x0000�ƂȂ��Ă��܂����ꍇ��0xFFFF�ɂ��� */
   }
   call_packet -> udp_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   packet_send(packet, 60); /* �p�P�b�g�̃T�C�Y��60byte�Ɏw�肵�đ��M���� */
}

unsigned char call_packet_error_check(unsigned char *packet)
{
   unsigned short i;
   unsigned long sum;
   CALL_PACKET *call_packet;
   unsigned char header[12];
   PSEUDO_HEADER *pseudo_header;
   
   call_packet = (CALL_PACKET *)packet; /* packet��CALL_PACKET�\���̂ɓ��Ă͂߂� */
   
   /* ��M���ɂ�����IP�w�b�_�̃`�F�b�N�T���̌v�Z */
   sum = ones_complement_sum(packet, 14, 20); /* 1�̕␔�a�iIP�w�b�_�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   if (sum != 0x0000)
   {
      return 1; /* �`�F�b�N�T����0x0000�ɂȂ�Ȃ��ꍇ�̓G���[���������Ƃ݂Ȃ���� */
   }
   
   if (call_packet -> udp_checksum != 0x0000) /* UDP�w�b�_�̃`�F�b�N�T����0x0000�ł͂Ȃ��Ƃ� */
   {
      /* �[���w�b�_ */
      pseudo_header = (PSEUDO_HEADER *)header; /* header��PSEUDO_HEADER�\���̂ɓ��Ă͂߂� */
      for (i = 0; i < 4; i++)
      {
         pseudo_header -> ip_src_IP[i] = call_packet -> ip_src_IP[i]; /* ���M��IP�A�h���X */
         pseudo_header -> ip_dst_IP[i] = call_packet -> ip_dst_IP[i]; /* ����IP�A�h���X */
      }
      pseudo_header -> zero_padding = 0; /* �[���E�p�f�B���O */
      pseudo_header -> ip_protocol = call_packet -> ip_protocol; /* ��ʃv���g�R���̎�ށiUDP�j */
      pseudo_header -> udp_length = call_packet -> udp_length; /* UDP�Z�O�����g�̒��� */
      
      /* ��M���ɂ�����UDP�w�b�_�̃`�F�b�N�T���̌v�Z */
      sum = ones_complement_sum(header, 0, 12); /* 1�̕␔�a�i�[���w�b�_�j */
      sum += ones_complement_sum(packet, 34, (8 + 2)); /* 1�̕␔�a�iUDP�Z�O�����g�j */
      sum = (sum & 0xFFFF) + (sum >> 16); /* 1�̕␔�a */
      sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
      if (sum != 0x0000)
      {
         return 1; /* �`�F�b�N�T����0x0000�ɂȂ�Ȃ��ꍇ�̓G���[���������Ƃ݂Ȃ���� */
      }
   }
   
   return 0; /* �G���[���Ȃ���΁C0��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
}