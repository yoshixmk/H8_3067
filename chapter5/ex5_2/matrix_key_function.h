char matrix_key_read(void)
{
   char key;
   unsigned char column, row;
   
   key = 0; /* keyの初期値 */
   column = 0x10; /* columnの初期値 */
   
   while (column <= 0x40) /* columnが0x40以下のとき */
   {
      P4DR = ~column; /* P44～P47は出力端子 */
      /* columnをNOT演算によって反転し，P44～P47に出力する */
      
      row = (~P4DR) & 0x0F; /* P40～P43は入力端子 */
      /* P40～P43から入力されたデータをNOT演算によって反転し，rowに代入する */
      
      if (row != 0) /* rowが0ではないとき */
      {
         /* columnとrowをOR演算で組み合わせたデータによってキーを判定する */
         switch (column | row)
         {
            case 0x11 : key = '1'; break; /* 1キーを押したとき */
            case 0x12 : key = '2'; break; /* 2キーを押したとき */
            case 0x14 : key = '3'; break; /* 3キーを押したとき */
            case 0x18 : key = '4'; break; /* 4キーを押したとき */
            
            case 0x21 : key = '5'; break; /* 5キーを押したとき */
            case 0x22 : key = '6'; break; /* 6キーを押したとき */
            case 0x24 : key = '7'; break; /* 7キーを押したとき */
            case 0x28 : key = '8'; break; /* 8キーを押したとき */
            
            case 0x41 : key = '9'; break; /* 9キーを押したとき */
            case 0x42 : key = '.'; break; /* *キーを押したとき */
            case 0x44 : key = '0'; break; /* 0キーを押したとき */
            case 0x48 : key = '#'; break; /* #キーを押したとき */
            
            default : key = 0; break; /* どのキーも押されていないとき */
         }
      }
      column = column << 1;
      /* columnが0x10のときは0x20，0x20のときは0x40，0x40のときは0x80にする */
   }
   return key; /* keyを戻り値として，呼び出し元の関数に戻る */
}
