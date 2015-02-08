#include "H8_register.h" /* H8/3067�̃��W�X�^ */
#include "delay_function.h" /* ���ԑ҂��̊֐� */

void buzzer(unsigned short us)
{
   PADR = 0xFF; /* PA4��1�ɂ��� */
   delay_us(us); /* PA4��1�ɂ����܂܁Cus�P�ʂŎ��ԑ҂����� */
   PADR = 0xEF; /* PA4��0�ɂ��� */
   delay_us(us); /* PA4��0�ɂ����܂܁Cus�P�ʂŎ��ԑ҂����� */
}

void main(void)
{
   P5DDR = 0xF0; /* P50�`P53����͒[�q�Ƃ��Đݒ肷�� */
   P5PCR = 0xFF; /* P50�`P53�̃v���A�b�v��H��L���ɂ��� */
   
   PADDR = 0xFF; /* PA0�`PA7���o�͒[�q�Ƃ��Đݒ肷�� */
   PADR = 0xFF; /* PA0�`PA7�̏����l��1�ɐݒ肷�� */
   
   /* �^�C�}1�̐ݒ� */
   TCR1 |= 0x20; /* CCLR1��0�CCCLR0��1�ɐݒ肷�� */
   GRA1 = 0x0013; /* 1us�̐ݒ� (1/20MHz * (0x0013 + 0x0001)) */
   
   /* �^�C�}2�̐ݒ� */
   TCR2 |= 0x23; /* CCLR1��0�CCCLR0��1�CTPSC2��0�CTPSC1��1, TPSC0��1�ɐݒ肷�� */
   GRA2 = 0x09C3; /* 1ms�̐ݒ� (1/20MHz * 8 * (0x09C3 + 0x0001)) */
   
   while (1) /* �������[�v */
   {
      if (P5DR == 0xFD) /* ON�X�C�b�`�������������Ƃ� */
      {
         buzzer(500); /* ������1000us�i= 500us * 2�j�ƂȂ�g�`�p�^�[�����o�͂��� */
      }
      else if (P5DR == 0xFE) /* OFF�X�C�b�`�������������Ƃ� */
      {
         buzzer(1000); /* ������2000us�i= 1000us * 2�j�ƂȂ�g�`�p�^�[�����o�͂��� */
      }
   }
}
