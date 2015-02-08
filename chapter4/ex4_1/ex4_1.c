#include "H8_register.h" /* H8/3067�̃��W�X�^ */
#include "RTL8019AS_register.h" /* RTL8019AS�̃��W�X�^ */
#include "global_variables.h" /* �O���[�o���ϐ��̒�` */
#include "H8_register_function.h" /* H8/3067�̃��W�X�^�̐ݒ� */
#include "delay_function.h" /* ���ԑ҂��̊֐� */
#include "LCD_function.h" /* SC1602B�̊֐� */
#include "NIC_function.h" /* RTL8019AS�̊֐� */
#include "matrix_key_function.h" /* �}�g���N�X�E�L�[�̊֐� */
#include "utility_function.h" /* ���̂ق��̊֐� */

void main(void)
{
   char str_IP[16];
   char str_MAC[13];
   
   H8_register_init(); /* H8/3067�̃��W�X�^�̐ݒ� */
   LCD_init(); /* SC1602B�̏����� */
   NIC_init(); /* RTL8019AS�̏����� */
   
   /* ���M��IP�A�h���X�̐ݒ� */
   src_IP[0] = 192;
   src_IP[1] = 168;
   src_IP[2] = 0;
   src_IP[3] = 100 + (P2DR & 0x07); /* �f�B�b�v�E�X�C�b�`�ɂ���đ��M��IP�A�h���X��ݒ肷�� */
   
   LCD_clear(); /* �\���N���A */
   
   LCD_control(0x80); /* �J�[�\����0�s�ڂ̐擪�Ɉړ����� */
   IP_to_str(src_IP, str_IP); /* src_IP��str_IP�i������j�ɕϊ����� */
   LCD_print(str_IP); /* �t���f�B�X�v���C��str_IP��\������ */
   
   LCD_control(0xC0); /* �J�[�\����1�s�ڂ̐擪�Ɉړ����� */
   MAC_to_str(src_MAC, str_MAC); /* src_MAC��str_MAC�i������j�ɕϊ����� */
   LCD_print(str_MAC); /* �t���f�B�X�v���C��str_MAC��\������ */
   
   while (1) /* �������[�v */
   {
      /* �������Ȃ� */
   }
}
