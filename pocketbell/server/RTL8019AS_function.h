unsigned char NIC_read(unsigned char address)
{
   unsigned char data;
   
   P1DR = address; /* �|�[�g1�Ƀ��W�X�^�̃A�h���X���Z�b�g���� */
   P3DDR = 0x00; /* �|�[�g3����̓|�[�g�Ƃ��Đݒ肷�� */
   P6DR &= 0xEF; /* P64��0���Z�b�g���āC�f�[�^�̓ǂݎ��������� */
   data = P3DR; /* ���W�X�^�̃f�[�^��ǂݎ�� */
   P6DR |= 0x10; /* P64��1���Z�b�g���āC�f�[�^�̓ǂݎ����֎~���� */
   return data; /* data��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
}

void NIC_write(unsigned char address, unsigned char data)
{
   P1DR = address; /* �|�[�g1�Ƀ��W�X�^�̃A�h���X���Z�b�g���� */
   P3DDR = 0xFF; /* �|�[�g3���o�̓|�[�g�Ƃ��Đݒ肷�� */
   P6DR &= 0xDF; /* P65��0���Z�b�g���āC�f�[�^�̏������݂������� */
   P3DR = data; /* ���W�X�^�Ƀf�[�^���������� */
   P6DR |= 0x20; /* P65��1���Z�b�g���āC�f�[�^�̏������݂��֎~���� */
}

void NIC_init(void)
{
   unsigned short i;
   unsigned char data;
   
   /* �n�[�h�E�F�A�E���Z�b�g */
   PADR |= 0x80; /* RSTDRV��1���o�� */
   ms_timer(10); /* 10ms�̎��ԑ҂� */
   PADR &= 0x7F; /* RSTDRV��0���o�� */
   ms_timer(10); /* 10ms�̎��ԑ҂� */
   
   /* �\�t�g�E�F�A�E���Z�b�g */
   data = NIC_read(RP); /* Reset Port�̃f�[�^��ǂݎ�� */
   NIC_write(RP, data); /* Reset Port�Ƀf�[�^���������� */
   ms_timer(10); /* 10ms�̎��ԑ҂� */
   
   NIC_write(CR, 0x21); /* ���W�X�^�E�y�[�W0�CNIC���X�g�b�v������ */
   NIC_write(DCR, 0x4A); /* �f�[�^�`���̐ݒ� */
   NIC_write(RBCR0, 0); /* �����[�gDMA�ɂ�����f�[�^�E�T�C�Y�i���ʃo�C�g�j�̃N���A */
   NIC_write(RBCR1, 0); /* �����[�gDMA�ɂ�����f�[�^�E�T�C�Y�i��ʃo�C�g�j�̃N���A */
   NIC_write(RCR, 0x20); /* ���j�^�E���[�h�i��M�j */
   NIC_write(TCR, 0x02); /* ���[�v�o�b�N�E���[�h�i���M�j */
   NIC_write(TPSR, 0x40); /* ���M�o�b�t�@�̊J�n�y�[�W */
   NIC_write(PSTART, 0x46); /* ��M�o�b�t�@�̊J�n�y�[�W */
   NIC_write(BNRY, 0x46); /* ��M�o�b�t�@�̋��E�y�[�W */
   NIC_write(PSTOP, 0x60); /* ��M�o�b�t�@�̏I���y�[�W */
   NIC_write(IMR, 0x00); /* ���荞�݋֎~ */
   NIC_write(ISR, 0xFF); /* ���荞�݃t���O�̃N���A */
   
   NIC_write(RBCR0, 12); /* MAC�A�h���X�̃T�C�Y�i���ʃo�C�g�j */
   NIC_write(RBCR1, 0); /* MAC�A�h���X�̃T�C�Y�i��ʃo�C�g�j */
   NIC_write(RSAR0, 0x00); /* MAC�A�h���X�̐擪�A�h���X�i���ʃo�C�g�j */
   NIC_write(RSAR1, 0x00); /* MAC�A�h���X�̐擪�A�h���X�i��ʃo�C�g�j */
   NIC_write(CR, 0x0A); /* NIC���X�^�[�g�����C�����[�gDMA�ǂݎ��������� */
   for (i = 0; i < 6; i += 2)
   {
      src_MAC[i + 1] = NIC_read(RDMAP); /* MAC�A�h���X��ǂݎ�� */
      NIC_read(RDMAP); /* �_�~�[�E�f�[�^��ǂݎ�� */
      src_MAC[i] = NIC_read(RDMAP); /* MAC�A�h���X��ǂݎ�� */
      NIC_read(RDMAP); /* �_�~�[�E�f�[�^��ǂݎ�� */
   }
   do
   {
      data = NIC_read(ISR); /* ISR��ǂݎ�� */
   } while ((data & 0x40) == 0x00); /* �����[�gDMA����~����̂�҂� */
   
   NIC_write(CR, 0x61); /* ���W�X�^�E�y�[�W1�CNIC���X�g�b�v������ */
   NIC_write(PAR0, src_MAC[0]); /* MAC�A�h���X�̐ݒ� */
   NIC_write(PAR1, src_MAC[1]); /* MAC�A�h���X�̐ݒ� */
   NIC_write(PAR2, src_MAC[2]); /* MAC�A�h���X�̐ݒ� */
   NIC_write(PAR3, src_MAC[3]); /* MAC�A�h���X�̐ݒ� */
   NIC_write(PAR4, src_MAC[4]); /* MAC�A�h���X�̐ݒ� */
   NIC_write(PAR5, src_MAC[5]); /* MAC�A�h���X�̐ݒ� */
   
   NIC_write(CURR, 0x47); /* ��M�p�P�b�g�̊J�n�y�[�W�̏����l */
   
   NIC_write(MAR0, 0); /* �}���`�L���X�g�E�A�h���X�̐ݒ� */
   NIC_write(MAR1, 0); /* �}���`�L���X�g�E�A�h���X�̐ݒ� */
   NIC_write(MAR2, 0); /* �}���`�L���X�g�E�A�h���X�̐ݒ� */
   NIC_write(MAR3, 0); /* �}���`�L���X�g�E�A�h���X�̐ݒ� */
   NIC_write(MAR4, 0); /* �}���`�L���X�g�E�A�h���X�̐ݒ� */
   NIC_write(MAR5, 0); /* �}���`�L���X�g�E�A�h���X�̐ݒ� */
   NIC_write(MAR6, 0); /* �}���`�L���X�g�E�A�h���X�̐ݒ� */
   NIC_write(MAR7, 0); /* �}���`�L���X�g�E�A�h���X�̐ݒ� */
   
   NIC_write(CR, 0x21); /* ���W�X�^�E�y�[�W0�CNIC���X�g�b�v������ */
   NIC_write(RCR, 0x04); /* �u���[�h�L���X�g�E�p�P�b�g�ɑΉ����� */
   NIC_write(CR, 0x22); /* ���W�X�^�E�y�[�W0�CNIC���X�^�[�g������ */
   NIC_write(TCR, 0x00); /* �ʏ푗�M���[�h */
   NIC_write(IMR, 0x00); /* ���荞�݋֎~ */
}

void packet_send(unsigned char *packet, unsigned short size)
{
   unsigned short i;
   unsigned char data;
   unsigned char size_H, size_L;
   
   size_L = (unsigned char)(size & 0x00FF); /* ���M�p�P�b�g�̃T�C�Y�i���ʃo�C�g�j */
   size_H = (unsigned char)(size >> 8); /* ���M�p�P�b�g�̃T�C�Y�i��ʃo�C�g�j */
   
   NIC_write(CR, 0x22); /* ���W�X�^�E�y�[�W0 */
   NIC_write(RBCR0, size_L); /* ���M�p�P�b�g�̃T�C�Y�i���ʃo�C�g�j */
   NIC_write(RBCR1, size_H); /* ���M�p�P�b�g�̃T�C�Y�i��ʃo�C�g�j */
   NIC_write(RSAR0, 0x00); /* ���M�p�P�b�g�̐擪�A�h���X�i���ʃo�C�g�j */
   NIC_write(RSAR1, 0x40); /* ���M�p�P�b�g�̐擪�A�h���X�i��ʃo�C�g�j */
   NIC_write(CR, 0x12); /* �����[�gDMA�������݂������� */
   for (i = 0; i < size; i++)
   {
      NIC_write(RDMAP, packet[i]); /* ���M�o�b�t�@�Ƀp�P�b�g�̃f�[�^���������� */
   }
   do
   {
      data = NIC_read(ISR); /* ISR��ǂݎ�� */
   } while ((data & 0x40) == 0x00); /* �����[�gDMA����~����̂�҂� */
   
   NIC_write(CR, 0x22); /* ���W�X�^�E�y�[�W0 */
   NIC_write(TBCR0, size_L); /* ���M�p�P�b�g�̃T�C�Y�i���ʃo�C�g�j */
   NIC_write(TBCR1, size_H); /* ���M�p�P�b�g�̃T�C�Y�i��ʃo�C�g�j */
   NIC_write(TPSR, 0x40); /* ���M�o�b�t�@�̊J�n�y�[�W */
   NIC_write(CR, 0x26); /* �p�P�b�g�𑗐M���� */
   do
   {
      data = NIC_read(CR); /* CR��ǂݎ�� */
   } while ((data & 0x04) == 0x04); /* �p�P�b�g�̑��M����������̂�҂� */
}

unsigned char packet_receive(unsigned char *packet)
{
   unsigned short i;
   unsigned short size;
   unsigned char data;
   unsigned char size_H, size_L;
   unsigned char boundary_page, start_page, current_page;
   unsigned char header[4];
   
   NIC_write(CR, 0x22); /* ���W�X�^�E�y�[�W0 */
   boundary_page = NIC_read(BNRY); /* BNRY��ǂݎ�� */
   NIC_write(CR, 0x62); /* ���W�X�^�E�y�[�W1 */
   current_page = NIC_read(CURR); /* CURR��ǂݎ�� */
   
   if (current_page < boundary_page)
   {
      current_page += (0x60 - 0x46); /* ��M�o�b�t�@�������O�E�o�b�t�@�ł��邱�Ƃ��l������ */
   }
   if (current_page == boundary_page + 1) /* ��M�p�P�b�g�̔��� */
   {
      return 1; /* ��M�p�P�b�g�Ȃ� */
   }
   
   start_page = boundary_page + 1; /* ��M�p�P�b�g�̊J�n�y�[�W */
   if (start_page == 0x60)
   {
      start_page = 0x46; /* ��M�o�b�t�@�������O�E�o�b�t�@�ł��邱�Ƃ��l������ */
   }
   
   NIC_write(CR, 0x22); /* ���W�X�^�E�y�[�W0 */
   NIC_write(RBCR0, 4); /* �t���[���Ǘ��w�b�_�̃T�C�Y�i���ʃo�C�g�j */
   NIC_write(RBCR1, 0); /* �t���[���Ǘ��w�b�_�̃T�C�Y�i��ʃo�C�g�j */
   NIC_write(RSAR0, 0x00); /* �t���[���Ǘ��w�b�_�̐擪�A�h���X�i���ʃo�C�g�j */
   NIC_write(RSAR1, start_page); /* �t���[���Ǘ��w�b�_�̐擪�A�h���X�i��ʃo�C�g�j */
   NIC_write(CR, 0x0A); /* �����[�gDMA�ǂݎ��������� */
   for (i = 0; i < 4; i++)
   {
      header[i] = NIC_read(RDMAP); /* �t���[���Ǘ��w�b�_��ǂݎ�� */
   }
   do
   {
      data = NIC_read(ISR); /* ISR��ǂݎ�� */
   } while ((data & 0x40) == 0x00); /* �����[�gDMA����~����̂�҂� */
   
   NIC_write(CR, 0x22); /* ���W�X�^�E�y�[�W0 */
   size_L = header[2]; /* ��M�p�P�b�g�̃T�C�Y�i���ʃo�C�g�j */
   size_H = header[3]; /* ��M�p�P�b�g�̃T�C�Y�i��ʃo�C�g�j */
   size = ((unsigned short)size_H << 8) + (unsigned short)size_L; /* ��M�p�P�b�g�̃T�C�Y */
   NIC_write(RBCR0, size_L); /* ��M�p�P�b�g�̃T�C�Y�i���ʃo�C�g�j */
   NIC_write(RBCR1, size_H); /* ��M�p�P�b�g�̃T�C�Y�i��ʃo�C�g�j */
   NIC_write(RSAR0, 0x00); /* ��M�p�P�b�g�̐擪�A�h���X�i���ʃo�C�g�j */
   NIC_write(RSAR1, start_page); /* ��M�p�P�b�g�̐擪�A�h���X�i��ʃo�C�g�j */
   NIC_write(CR, 0x0A); /* �����[�gDMA�ǂݎ��������� */
   for (i = 0; i < 4; i++)
   {
      NIC_read(RDMAP); /* �_�~�[�E�f�[�^��ǂݎ�� */
   }
   for (i = 0; i < (size - 4); i++)
   {
      packet[i] = NIC_read(RDMAP); /* ��M�o�b�t�@����p�P�b�g�̃f�[�^��ǂݎ�� */
      
      if (i >= 256) /* �p�P�b�g�̃T�C�Y��256byte�����傫���Ƃ� */
      {
         NIC_read(RDMAP); /* �_�~�[�E�f�[�^��ǂݎ�� */
      }
   }
   do
   {
      data = NIC_read(ISR); /* ISR��ǂݎ�� */
   } while ((data & 0x40) == 0x00); /* �����[�gDMA����~����̂�҂� */
   
   NIC_write(CR, 0x22); /* ���W�X�^�E�y�[�W0 */
   boundary_page = current_page - 1;
   if (boundary_page >= 0x60)
   {
      boundary_page -= (0x60 - 0x46); /* ��M�o�b�t�@�������O�E�o�b�t�@�ł��邱�Ƃ��l������ */
   }
   NIC_write(BNRY, boundary_page); /* BNRY���X�V���� */
   
   return 0;
}
