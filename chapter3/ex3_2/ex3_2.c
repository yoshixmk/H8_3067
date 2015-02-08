#include "H8_register.h" /* H8/3067�̃��W�X�^ */

void main(void)
{
   P5DDR = 0xF0; /* P50�`P53����͒[�q�Ƃ��Đݒ肷�� */
   P5PCR = 0xFF; /* P50�`P53�̃v���A�b�v��H��L���ɂ��� */
   
   PADDR = 0xFF; /* PA0�`PA7���o�͒[�q�Ƃ��Đݒ肷�� */
   PADR = 0xFF; /* PA0�`PA7�̏����l��1�ɐݒ肷�� */
   
   while (1) /* �������[�v */
   {
      if (P5DR == 0xFD) /* ON�X�C�b�`�������������Ƃ� */
      {
         PADR = 0xFB; /* LED(D8)�������������� */
      }
      else if (P5DR == 0xFE) /* OFF�X�C�b�`�������������Ƃ� */
      {
         PADR = 0xF7; /* LED(D9)�������������� */
      }
      else /* ����ȊO�̂Ƃ� */
      {
         PADR = 0xFF; /* �ǂ����LED���������Ȃ� */
      }
   }
}
