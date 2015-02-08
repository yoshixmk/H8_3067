void IP_to_str(unsigned char *IP, char *str)
{
   unsigned short i, n;
   
   n = 0;
   for (i = 0; i < 4; i++)
   {
      str[n] = '0' + (IP[i] / 100);
      /* '0'�̕����R�[�h����Ƃ��āCIP[i]��10�i���ŕ\�����Ƃ���100�̈ʂ𕶎��ɕϊ����� */
      
      n++;
      str[n] = '0' + ((IP[i] % 100) / 10);
      /* '0'�̕����R�[�h����Ƃ��āCIP[i]��10�i���ŕ\�����Ƃ���10�̈ʂ𕶎��ɕϊ����� */
      
      n++;
      str[n] = '0' + (IP[i] % 10);
      /* '0'�̕����R�[�h����Ƃ��āCIP[i]��10�i���ŕ\�����Ƃ���1�̈ʂ𕶎��ɕϊ����� */
      
      if (i < 3)
      {
         n++;
         str[n] = '.'; /* ��؂蕶���i�s���I�h�j */
      }
      
      n++;
   }
   str[n] = 0x00; /* ������̍Ō���i�������ɂ��� */
}

void MAC_to_str(unsigned char *MAC, char *str)
{
   unsigned short i, n;
   unsigned char nibble;
   
   n = 0;
   for (i = 0; i < 6; i++)
   {
      nibble = (MAC[i] & 0xF0) >> 4; /* MAC[i]�̏��4bit */
      if (nibble < 10) /* nibble��10�����������Ƃ� */
      {
         str[n] = '0' + nibble;
         /* '0'�̕����R�[�h����Ƃ��āCnibble�𕶎��ɕϊ����� */
      }
      else /* MAC[i]�̏��4bit��10�ȏ�̂Ƃ� */
      {
         str[n] = 'A' + nibble - 10;
         /* 'A'�̕����R�[�h����Ƃ��āCnibble�𕶎��ɕϊ����� */
      }
      
      n++;
      nibble = MAC[i] & 0x0F; /* MAC[i]�̉���4bit */
      if (nibble < 10) /* nibble��10�����������Ƃ� */
      {
         str[n] = '0' + nibble;
         /* '0'�̕����R�[�h����Ƃ��āCnibble�𕶎��ɕϊ����� */
      }
      else /* MAC[i]�̉���4bit��10�ȏ�̂Ƃ� */
      {
         str[n] = 'A' + nibble - 10;
         /* 'A'�̕����R�[�h����Ƃ��āCnibble�𕶎��ɕϊ����� */
      }
      
      n++;
   }
   str[n] = 0x00; /* ������̍Ō���i�������ɂ��� */
}

unsigned char str_to_IP(char *str, unsigned char *IP)
{
   unsigned short i, n;
   unsigned short sum;
   
   n = 0;
   
   for (i = 0; i < 4; i++)
   {
      sum = 0;
      while (n < 15)
      {
         sum *= 10;
         sum += (str[n] - '0'); /* '0'�̕����R�[�h����Ƃ��āC�����𐔎��ɕϊ����� */
         n++;
         if ((str[n] == '.') || (str[n] == ' ')) /* ��؂蕶���i�s���I�h�܂��͋󔒁j */
         {
            n++;
            break;
         }
      }
      if (sum < 256) /* �G���[�E�`�F�b�N */
      {
         IP[i] = (unsigned char)sum; /* �G���[���Ȃ���΁Csum��IP[i]�ɑ������ */
      }
      else
      {
         return 1; /* �G���[������΁C1��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
      }
   }
   
   return 0; /* �G���[���Ȃ���΁C0��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
}

unsigned char IP_compare(unsigned char *IP_a, unsigned char *IP_b)
{
   unsigned short i;
   
   for (i = 0; i < 4; i++)
   {
      if (IP_a[i] != IP_b[i])
      {
         return 1; /* IP�A�h���X����v���Ȃ���΁C1��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
      }
   }
   
   return 0; /* IP�A�h���X����v����΁C0��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
}

unsigned long ones_complement_sum(unsigned char *data, unsigned short offset, unsigned short size)
{
   unsigned short i;
   unsigned long sum;
   
   sum = 0;
   for (i = offset; i < (offset + size); i += 2)
   {
      sum += ((unsigned long)data[i] << 8) + (unsigned long)data[i + 1];
      /* data[i]����ʃo�C�g�Cdata[i + 1]�����ʃo�C�g�Ƃ��āC2byte�P�ʂō��v�l���v�Z���� */
      
      sum = (sum & 0xFFFF) + (sum >> 16);
      /* sum��0xFFFF�����傫���Ƃ��́C�ŏ�ʂ̌����������C���炽�߂�1�����Z���� */
   }
   
   return sum; /* sum��߂�l�Ƃ��āC�Ăяo�����̊֐��ɖ߂� */
}
