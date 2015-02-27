/************************************************
 *  Utility Function Header File                *
 ************************************************/

void MAC_to_str(unsigned char *MAC, char *str)
{
   unsigned short i, n;
   unsigned char nibble;
   
   n = 0;
   for (i = 0; i < 6; i++)
   {
      nibble = (MAC[i] & 0xF0) >> 4; /* MAC[i]の上位4bit */
      if (nibble < 10) /* nibbleが10よりも小さいとき */
      {
         str[n] = '0' + nibble;
         /* '0'の文字コードを基準として，nibbleを文字に変換する */
      }
      else /* MAC[i]の上位4bitが10以上のとき */
      {
         str[n] = 'A' + nibble - 10;
         /* 'A'の文字コードを基準として，nibbleを文字に変換する */
      }
      
      n++;
      nibble = MAC[i] & 0x0F; /* MAC[i]の下位4bit */
      if (nibble < 10) /* nibbleが10よりも小さいとき */
      {
         str[n] = '0' + nibble;
         /* '0'の文字コードを基準として，nibbleを文字に変換する */
      }
      else /* MAC[i]の下位4bitが10以上のとき */
      {
         str[n] = 'A' + nibble - 10;
         /* 'A'の文字コードを基準として，nibbleを文字に変換する */
      }
      
      n++;
   }
   str[n] = 0x00; /* 文字列の最後をナル文字にする */
}

void IP_to_str(unsigned char *IP, char *str)
{
   unsigned short i, n;
   
   n = 0;
   for (i = 0; i < 4; i++)
   {
      str[n] = '0' + (IP[i] / 100);
      /* '0'の文字コードを基準として，IP[i]を10進数で表したときの100の位を文字に変換する */
      
      n++;
      str[n] = '0' + ((IP[i] % 100) / 10);
      /* '0'の文字コードを基準として，IP[i]を10進数で表したときの10の位を文字に変換する */
      
      n++;
      str[n] = '0' + (IP[i] % 10);
      /* '0'の文字コードを基準として，IP[i]を10進数で表したときの1の位を文字に変換する */
      
      if (i < 3)
      {
         n++;
         str[n] = '.'; /* 区切り文字（ピリオド） */
      }
      
      n++;
   }
   str[n] = 0x00; /* 文字列の最後をナル文字にする */
}

unsigned char IP_compare(unsigned char *IP_a, unsigned char *IP_b)
{
   unsigned short i;
   
   for (i = 0; i < 4; i++)
   {
      if (IP_a[i] != IP_b[i])
      {
         return 1; /* IPアドレスが一致しなければ，1を戻り値として，呼び出し元の関数に戻る */
      }
   }
   
   return 0; /* IPアドレスが一致すれば，0を戻り値として，呼び出し元の関数に戻る */
}

/* 宛先IPアドレスをマトリックス・キーから入力 */
int IP_input(unsigned char *IP)
{
   char i=0;
   char key=0;
   char count=3;
   unsigned short sum=0;

   LCD_control(0x01);/*LCD clear*/
   LCD_control(0x80); /* カーソルを1行目の先頭に移動する */

   do{
	key = matrix_key_read(); /* マトリックス・キーの押されたキー */
	if(key>='0' && key<='9'){
		if(count==1){
			LCD_display(key);
			sum+=(key-'0');
			count--;
		}
		if(count==2){
			LCD_display(key);
			sum+=(key-'0')*10;
			count--;
		}
		if(count==3){
			LCD_display(key);
			sum =(key-'0')*100;
			count--;
		}
		ms_timer(500);
	}
	if(key == '.'){
		if(count==0 && sum<=255){
			IP[i]=sum;
			i++;
			count=3;
			LCD_display(key);
			ms_timer(500);
		}else{
			return 1;
		}
	}
	
   }while(i<4);
   return 0;
}

unsigned long ones_complement_sum(unsigned char *data, unsigned short offset, unsigned short size)
{
   unsigned short i;
   unsigned long sum;
   
   sum = 0;
   for (i = offset; i < (offset + size); i += 2)
   {
      sum += ((unsigned long)data[i] << 8) + (unsigned long)data[i + 1];
      /* data[i]を上位バイト，data[i + 1]を下位バイトとして，2byte単位で合計値を計算する */
      
      sum = (sum & 0xFFFF) + (sum >> 16);
      /* sumが0xFFFFよりも大きいときは，最上位の桁を消去し，あらためて1を加算する */
   }
   
   return sum; /* sumを戻り値として，呼び出し元の関数に戻る */
}
