#include "H8_register.h" /* H8/3067�̃��W�X�^ */
#include "delay_function.h" /* ���ԑ҂��̊֐� */
#include "LCD_function.h" /* SC1602B�̊֐� */
#include "matrix_key_function.h" /* �}�g���N�X�E�L�[�̊֐� */

void main(void)
{
   char key, previous_key;
   unsigned char cursor, line;
   
   P4DDR = 0xF0; /* P40�`P43����͒[�q�CP44�`P47���o�͒[�q�Ƃ��Đݒ肷�� */
   P4PCR = 0x0F; /* P40�`P43�̃v���A�b�v��H��L���ɂ��� */
   
   PBDDR = 0xFF; /* PB0�`PB7���o�͒[�q�Ƃ��Đݒ肷�� */
   PBDR = 0xFF; /* PB0�`PB7�̏����l��1�ɐݒ肷�� */
   
   /* �^�C�}1�̐ݒ� */
   TCR1 |= 0x20; /* CCLR1��0�CCCLR0��1�ɐݒ肷�� */
   GRA1 = 0x0013; /* 1us�̐ݒ� (1/20MHz * (0x0013 + 0x0001)) */
   
   /* �^�C�}2�̐ݒ� */
   TCR2 |= 0x23; /* CCLR1��0�CCCLR0��1�CTPSC2��0�CTPSC1��1, TPSC0��1�ɐݒ肷�� */
   GRA2 = 0x09C3; /* 1ms�̐ݒ� (1/20MHz * 8 * (0x09C3 + 0x0001)) */
   
   LCD_init(); /* SC1602B�̏����� */
   
   LCD_clear(); /* �\���N���A */
   LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
   
   cursor = 0; /* cursor�̏����l */
   line = 0; /* line�̏����l */
   previous_key = 0; /* previous_key�̏����l */
   
   while (1) /* �������[�v */
   {
      while (cursor < 16) /* cursor��16�����������Ƃ� */
      {
         delay_ms(20);
         /* 20ms�̎��ԑ҂����͂��ނ��ƂŁC�`���^�����O�ɂ��X�C�b�`�̌댟�o��������� */
         
         key = matrix_key_read(); /* ���݂̃L�[���`�F�b�N���� */
         
         if ((key != 0) && (key != previous_key))
         /* ���݂̃L�[��0�ł͂Ȃ��C���O�̃L�[�Ƃ͈قȂ�Ƃ� */
         {
            if ((cursor == 0) && (line == 0)) /* cursor��0�Cline��0�̂Ƃ� */
            {
               LCD_clear(); /* �\���N���A */
               LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
            }
            else if ((cursor == 0) && (line == 1)) /* cursor��0�Cline��1�̂Ƃ� */
            {
               LCD_control(0xC0); /* �J�[�\����1�s�ڂ̐擪�Ɉړ����� */
            }
            LCD_display(key); /* �t���f�B�X�v���C�Ɍ��݂̃L�[��\������ */
            cursor++; /* cursor���C���N�������g���� */
         }
         previous_key = key;
         /* ���݂̃L�[���C���̃`�F�b�N�����ɂ����钼�O�̃L�[�Ƃ��Ē�`���� */
      }
      cursor = 0; /* cursor��0�ɖ߂� */
      line ^= 1; /* XOR���Z�ɂ���āCline��0�̂Ƃ���1�Cline��1�̂Ƃ���0�ɂ��� */
   }
}
