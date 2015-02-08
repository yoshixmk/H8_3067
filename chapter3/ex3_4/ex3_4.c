#include "H8_register.h" /* H8/3067�̃��W�X�^ */
#include "delay_function.h" /* ���ԑ҂��̊֐� */
#include "LCD_function.h" /* SC1602B�̊֐� */

void main(void)
{
   char str[] = "Hello, world!"; /* �t���f�B�X�v���C�ɕ\�����镶���� */
   
   PBDDR = 0xFF; /* PB0�`PB7���o�͒[�q�Ƃ��Đݒ肷�� */
   PBDR = 0xFF; /* PB0�`PB7�̏����l��1�ɐݒ肷�� */
   
   /* �^�C�}1�̐ݒ� */
   TCR1 |= 0x20; /* CCLR1��0�CCCLR0��1�ɐݒ肷�� */
   GRA1 = 0x0013; /* 1us�̐ݒ� (1/20MHz * (0x0013 + 0x0001)) */
   
   /* �^�C�}2�̐ݒ� */
   TCR2 |= 0x23; /* CCLR1��0�CCCLR0��1�CTPSC2��0�CTPSC1��1, TPSC0��1�ɐݒ肷�� */
   GRA2 = 0x09C3; /* 1ms�̐ݒ� (1/20MHz * 8 * (0x09C3 + 0x0001)) */
   
   LCD_init(); /* SC1602B�̏����� */
   
   while (1) /* �������[�v */
   {
      LCD_clear(); /* �\���N���A */
      LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
      LCD_print(str); /* �t���f�B�X�v���C�ɕ������\������ */
      delay_ms(1000); /* 1000ms�i1�b�j�������ԑ҂����� */
      
      LCD_clear(); /* �\���N���A */
      LCD_control(0xC0); /* �J�[�\����1�s�ڂ̐擪�Ɉړ����� */
      LCD_print(str); /* �t���f�B�X�v���C�ɕ������\������ */
      delay_ms(1000); /* 1000ms�i1�b�j�������ԑ҂����� */
   }
}
