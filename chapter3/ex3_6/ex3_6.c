#include "H8_register.h" /* H8/3067�̃��W�X�^ */
#include <machine.h> /* set_imask_ccr�֐� */

/* sampling_8khz�֐��́C�^�C�}���荞�݂����������Ƃ��ɌĂяo�����֐� */
#pragma interrupt(sampling_8khz)
void sampling_8khz(void)
{
   unsigned char speech;
   
   TISRA &= 0xFE; /* IMFA0��0�ɖ߂� */
   
   ADCSR |= 0x20; /* ADST��1�ɂ���A-D�ϊ����X�^�[�g������ */
   while ((ADCSR & 0x80) == 0x00) /* ADF��1�ɂȂ�̂�҂� */
   {
      /* A-D�ϊ������������ADF��1�ɂȂ� */
   }
   ADCSR &= 0x7F; /* ADF��0�ɖ߂� */
   
   speech = (unsigned char)(ADDRA >> 8); /* ADDRA�̏��8bit��speech�Ɋi�[���� */
   
   DADR0 = speech; /* speech��DA0�ɏo�͂��� */
}

void main(void)
{
   set_imask_ccr(0); /* H8/3067�����荞�݂��󂯕t�������Ԃɂ��� */
   
   /* A-D�R���o�[�^�̐ݒ� */
   ADCSR |= 0x00; /* CH2��0, CH1��0, CH0��0�ɐݒ肵�CAN0���A�i���O�M���̓��͒[�q�ɐݒ肷�� */
   
   /* D-A�R���o�[�^�̐ݒ� */
   DACR |= 0x40; /* DAOE0��1�ɐݒ肵�CDA0���A�i���O�M���̏o�͒[�q�ɐݒ肷�� */
   
   /* �^�C�}0�̐ݒ� */
   TCR0 |= 0x20; /* CCLR1��0�CCCLR0��1�ɐݒ肷�� */
   GRA0 = 0x09C3; /* 125us�̐ݒ� (1/20MHz * (0x09C3 +0x0001)) */
   TISRA |= 0x10; /* IMIEA0��1�ɐݒ肵�C�^�C�}���荞�݂�L���ɂ��� */
   
   TSTR |= 0x01; /* STR0��1�ɂ��ă^�C�}0���X�^�[�g������ */
   
   while (1) /* �������[�v */
   {
      /* 125us���ƂɃ^�C�}���荞�݂��������� */
   }
}
