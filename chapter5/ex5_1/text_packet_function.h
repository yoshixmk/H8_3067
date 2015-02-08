void text_packet_send(unsigned char *packet)
{
   unsigned short i;
   unsigned long sum;
   TEXT_PACKET *text_packet;
   unsigned char header[12];
   PSEUDO_HEADER *pseudo_header;
   
   text_packet = (TEXT_PACKET *)packet; /* packet��TEXT_PACKET�\���̂ɓ��Ă͂߂� */
   
   for (i = 0; i < 6; i++)
   {
      text_packet -> eth_dst_MAC[i] = dst_MAC[i]; /* ����MAC�A�h���X */
   }
   for (i = 0; i < 6; i++)
   {
      text_packet -> eth_src_MAC[i] = src_MAC[i]; /* ���M��MAC�A�h���X */
   }
   text_packet -> eth_ethernet_type = 0x0800; /* ��ʃv���g�R���̎�ށiIP�j */
   text_packet -> ip_version_length = 0x45; /* IP�̃o�[�W������IP�w�b�_�̒��� */
   text_packet -> ip_service_type = 0; /* �p�P�b�g�̗D��x */
   text_packet -> ip_total_length = 20 + 8 + 16; /* IP�f�[�^�O�����̒��� */
   text_packet -> ip_id = src_ip_id; /* �p�P�b�g�̎��ʎq */
   text_packet -> ip_flags_fragment_offset = 0x4000; /* �p�P�b�g�̕����̗L���Ɩ{���̃f�[�^�ʒu */
   text_packet -> ip_time_to_live = 128; /* ���[�^�̒��p�񐔂̏�� */
   text_packet -> ip_protocol = 17; /* ��ʃv���g�R���̎�ށiUDP�j */
   text_packet -> ip_checksum = 0x0000; /* �`�F�b�N�T���̏����l */
   for (i = 0; i < 4; i++)
   {
      text_packet -> ip_src_IP[i] = src_IP[i]; /* ���M��IP�A�h���X */
   }
   for (i = 0; i < 4; i++)
   {
      text_packet -> ip_dst_IP[i] = dst_IP[i]; /* ����IP�A�h���X */
   }
   text_packet -> udp_src_port = src_port; /* ���M���|�[�g�ԍ� */
   text_packet -> udp_dst_port = dst_port; /* ����|�[�g�ԍ� */
   text_packet -> udp_length = 8 + 16; /* UDP�Z�O�����g�̒��� */
   text_packet -> udp_checksum = 0x0000; /* �`�F�b�N�T���̏����l */
   for (i = 0; i < 16; i++)
   {
      text_packet -> text_data[i] = text_buffer[i];
      /* text_buffer�Ɋi�[���ꂽ�e�L�X�g�E�f�[�^���p�P�b�g�ɓ��ڂ��� */
   }
   
   /* ���M���ɂ�����IP�w�b�_�̃`�F�b�N�T���̌v�Z */
   sum = ones_complement_sum(packet, 14, 20); /* 1�̕␔�a�iIP�w�b�_�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   text_packet -> ip_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   /* �[���w�b�_ */
   pseudo_header = (PSEUDO_HEADER *)header; /* header��PSEUDO_HEADER�\���̂ɓ��Ă͂߂� */
   for (i = 0; i < 4; i++)
   {
      pseudo_header -> ip_src_IP[i] = text_packet -> ip_src_IP[i]; /* ���M��IP�A�h���X */
      pseudo_header -> ip_dst_IP[i] = text_packet -> ip_dst_IP[i]; /* ����IP�A�h���X */
   }
   pseudo_header -> zero_padding = 0; /* �[���E�p�f�B���O */
   pseudo_header -> ip_protocol = text_packet -> ip_protocol; /* ��ʃv���g�R���̎�ށiUDP�j */
   pseudo_header -> udp_length = text_packet -> udp_length; /* UDP�Z�O�����g�̒��� */
   
   /* ���M���ɂ�����UDP�w�b�_�̃`�F�b�N�T���̌v�Z */
   sum = ones_complement_sum(header, 0, 12); /* 1�̕␔�a�i�[���w�b�_�j */
   sum += ones_complement_sum(packet, 34, (8 + 16)); /* 1�̕␔�a�iUDP�Z�O�����g�j */
   sum = (sum & 0xFFFF) + (sum >> 16); /* 1�̕␔�a */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   if (sum == 0x0000)
   {
      sum = 0xFFFF; /* �v�Z���ʂ�0x0000�ƂȂ��Ă��܂����ꍇ��0xFFFF�ɂ��� */
   }
   text_packet -> udp_checksum = (unsigned short)sum; /* �v�Z���ʂ��Z�b�g���� */
   
   packet_send(packet, 60); /* �p�P�b�g�̃T�C�Y��60byte�Ɏw�肵�đ��M���� */
}

unsigned char text_packet_error_check(unsigned char *packet)
{
   unsigned short i;
   unsigned long sum;
   TEXT_PACKET *text_packet;
   unsigned char header[12];
   PSEUDO_HEADER *pseudo_header;
   
   text_packet = (TEXT_PACKET *)packet; /* packet��TEXT_PACKET�\���̂ɓ��Ă͂߂� */
   
   /* ��M���ɂ�����IP�w�b�_�̃`�F�b�N�T���̌v�Z */
   sum = ones_complement_sum(packet, 14, 20); /* 1�̕␔�a�iIP�w�b�_�j */
   sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
   if (sum != 0x0000)
   {
      return 1; /* �`�F�b�N�T����0x0000�ɂȂ�Ȃ��ꍇ�̓G���[���������Ƃ݂Ȃ���� */
   }
   
   if (text_packet -> udp_checksum != 0x0000) /* UDP�w�b�_�̃`�F�b�N�T����0x0000�ł͂Ȃ��Ƃ� */
   {
      /* �[���w�b�_ */
      pseudo_header = (PSEUDO_HEADER *)header; /* header��PSEUDO_HEADER�\���̂ɓ��Ă͂߂� */
      for (i = 0; i < 4; i++)
      {
         pseudo_header -> ip_src_IP[i] = text_packet -> ip_src_IP[i]; /* ���M��IP�A�h���X */
         pseudo_header -> ip_dst_IP[i] = text_packet -> ip_dst_IP[i]; /* ����IP�A�h���X */
      }
      pseudo_header -> zero_padding = 0; /* �[���E�p�f�B���O */
      pseudo_header -> ip_protocol = text_packet -> ip_protocol; /* ��ʃv���g�R���̎�ށiUDP�j */
      pseudo_header -> udp_length = text_packet -> udp_length; /* UDP�Z�O�����g�̒��� */
      
      /* ��M���ɂ�����UDP�w�b�_�̃`�F�b�N�T���̌v�Z */
      sum = ones_complement_sum(header, 0, 12); /* 1�̕␔�a�i�[���w�b�_�j */
      sum += ones_complement_sum(packet, 34, (8 + 16)); /* 1�̕␔�a�iUDP�Z�O�����g�j */
      sum = (sum & 0xFFFF) + (sum >> 16); /* 1�̕␔�a */
      sum = (~sum) & 0xFFFF; /* �v�Z���ʂ�NOT���Z�ɂ���Ĕ��]���� */
      if (sum != 0x0000)
      {
         return 1; /* �`�F�b�N�T����0x0000�ɂȂ�Ȃ��ꍇ�̓G���[���������Ƃ݂Ȃ���� */
      }
   }
   
   return 0; /* �G���[���Ȃ���΁C0��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
}
