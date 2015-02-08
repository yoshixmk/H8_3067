char matrix_key_read(void)
{
   char key;
   unsigned char column, row;
   
   key = 0; /* key�̏����l */
   column = 0x10; /* column�̏����l */
   
   while (column <= 0x40) /* column��0x40�ȉ��̂Ƃ� */
   {
      P4DR = ~column; /* P44�`P47�͏o�͒[�q */
      /* column��NOT���Z�ɂ���Ĕ��]���CP44�`P47�ɏo�͂��� */
      
      row = (~P4DR) & 0x0F; /* P40�`P43�͓��͒[�q */
      /* P40�`P43������͂��ꂽ�f�[�^��NOT���Z�ɂ���Ĕ��]���Crow�ɑ������ */
      
      if (row != 0) /* row��0�ł͂Ȃ��Ƃ� */
      {
         /* column��row��OR���Z�őg�ݍ��킹���f�[�^�ɂ���ăL�[�𔻒肷�� */
         switch (column | row)
         {
            case 0x11 : key = '1'; break; /* 1�L�[���������Ƃ� */
            case 0x12 : key = '2'; break; /* 2�L�[���������Ƃ� */
            case 0x14 : key = '3'; break; /* 3�L�[���������Ƃ� */
            case 0x18 : key = '4'; break; /* 4�L�[���������Ƃ� */
            
            case 0x21 : key = '5'; break; /* 5�L�[���������Ƃ� */
            case 0x22 : key = '6'; break; /* 6�L�[���������Ƃ� */
            case 0x24 : key = '7'; break; /* 7�L�[���������Ƃ� */
            case 0x28 : key = '8'; break; /* 8�L�[���������Ƃ� */
            
            case 0x41 : key = '9'; break; /* 9�L�[���������Ƃ� */
            case 0x42 : key = '.'; break; /* *�L�[���������Ƃ� */
            case 0x44 : key = '0'; break; /* 0�L�[���������Ƃ� */
            case 0x48 : key = '#'; break; /* #�L�[���������Ƃ� */
            
            default : key = 0; break; /* �ǂ̃L�[��������Ă��Ȃ��Ƃ� */
         }
      }
      column = column << 1;
      /* column��0x10�̂Ƃ���0x20�C0x20�̂Ƃ���0x40�C0x40�̂Ƃ���0x80�ɂ��� */
   }
   return key; /* key��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
}
