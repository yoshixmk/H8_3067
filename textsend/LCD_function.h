void LCD_write(char data, char RS)
{
   /* SC1602B�̒[�q: DB7, DB6, DB5, DB4,   x,   x,  RS,   E */
   /* H8/3067�̒[�q: PB7, PB6, PB5, PB4, PB3, PB2, PB1, PB0 */
   
   PBDR = data & 0xF0; /* DB4�`DB7��data�̏��4bit�CRS��E��0���Z�b�g���� */
   
   if (RS == 1) /* �����R�[�h�̏ꍇ */
   {
      PBDR |= 0x02; /* RS��1���Z�b�g���� */
   }
   else /* ����R�[�h�̏ꍇ */
   {
      PBDR &= 0xF0; /* RS��0���Z�b�g���� */
   }
   
   delay_us(1); /* 1us�i40ns�ȏ�j�̎��ԑ҂� */
   PBDR |= 0x01; /* E��1���Z�b�g���� */
   delay_us(1); /* 1us�i230ns�ȏ�j�̎��ԑ҂� */
   PBDR &= 0xFE; /* E��0���Z�b�g���� */
}

void LCD_display(char code)
{
   LCD_write(code, 1); /* �����R�[�h�̏��4bit��]������ */
   LCD_write(code << 4, 1); /* �����R�[�h�̉���4bit��]������ */
   delay_us(50); /* 50us�i40us�ȏ�j�̎��ԑ҂� */
}

void LCD_control(char code)
{
   LCD_write(code, 0); /* ����R�[�h�̏��4bit��]������ */
   LCD_write(code << 4, 0); /* ����R�[�h�̉���4bit��]������ */
   delay_us(50); /* 50us�i40us�ȏ�j�̎��ԑ҂� */
}

void LCD_clear(void)
{
   LCD_write(0x01, 0); /* 0x01�̏��4bit��]������ */
   LCD_write(0x01 << 4, 0); /* 0x01�̉���4bit��]������ */
   delay_ms(2); /* 2ms�i1.64ms�ȏ�j�̎��ԑ҂� */
}

void LCD_init(void)
{
   delay_ms(20); /* 20ms�i15ms�ȏ�j�̎��ԑ҂� */
   LCD_write(0x30, 0); /* 8bit���[�h�ݒ� */
   delay_ms(5); /* 5ms�i4.1ms�ȏ�j�̎��ԑ҂� */
   LCD_write(0x30, 0); /* 8bit���[�h�ݒ� */
   delay_ms(5); /* 5ms�i100us�ȏ�j�̎��ԑ҂� */
   LCD_write(0x30, 0); /* 8bit���[�h�ݒ� */
   delay_ms(5); /* 5ms�i4.1ms�ȏ�j�̎��ԑ҂� */
   LCD_write(0x20, 0); /* 4bit���[�h�ݒ� */
   delay_ms(5); /* 5ms�i40us�ȏ�j�̎��ԑ҂� */
   LCD_control(0x28); /* 4bit���[�h��2�s�\���ɐݒ肷�� */
   LCD_control(0x08); /* �����\�����I�t�ɂ��� */
   LCD_control(0x0C); /* �����\�����I���ɂ��� */
   LCD_control(0x06); /* �J�[�\���ړ����E�����ɐݒ肷�� */
}

void LCD_print(char *str)
{
   while (*str != 0x00) /* �i�������i0x00�j�������܂� */
   {
      LCD_display(*str); /* �t���f�B�X�v���C�ɕ�����\������ */
      str++;
   }
}
