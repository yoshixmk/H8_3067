void delay_us(unsigned short us)
{
   unsigned short t;
   
   TSTR |= 0x02; /* STR1��1�ɂ��ă^�C�}1���X�^�[�g������ */
   for (t = 0; t < us; t++)
   {
      while ((TISRA & 0x02) == 0x00) /* IMFA1��1�ɂȂ�̂�҂� */
      {
         /* TCNT1��GRA1�̃R���y�A�E�}�b�`���N�����IMFA1��1�ɂȂ� */
      }
      TISRA &= 0xFD; /* IMFA1��0�ɖ߂� */
      
      /* �ȏ�̏�����us��J��Ԃ� */
   }
   TSTR &= 0xFD; /* STR1��0�ɂ��ă^�C�}1���X�g�b�v������ */
}

void delay_ms(unsigned short ms)
{
   unsigned short t;
   
   TSTR |= 0x04; /* STR2��1�ɂ��ă^�C�}2���X�^�[�g������ */
   for (t = 0; t < ms; t++)
   {
      while ((TISRA & 0x04) == 0x00) /* IMFA2��1�ɂȂ�̂�҂� */
      {
         /* TCNT2��GRA2�̃R���y�A�E�}�b�`���N�����IMFA2��1�ɂȂ� */
      }
      TISRA &= 0xFB; /* IMFA2��0�ɖ߂� */
      
      /* �ȏ�̏�����ms��J��Ԃ� */
   }
   TSTR &= 0xFB; /* STR2��0�ɂ��ă^�C�}2���X�g�b�v������ */
}
