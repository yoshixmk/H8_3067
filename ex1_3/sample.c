#include <sci.h>
#include <reg3067.h>
#include <machine.h>
#pragma interrupt(int_ms_timer)

int i;
void ms_timer(unsigned short ms)
{
  int i;
  TCR2=0xA3;
  GRA2=2499;
  TSTR=0xFC;
  for(i = 0; i < ms; i++ ){
    while((TISRA & 0x04) != 0x04);
    TISRA&=0xFB;
  }
  TSTR&=0xFB;
}

void int_ms_timer(void)
{
  if(i>500){
    i=0;
    if(PADR == 0xFB)
      PADR = 0xF7;
    else 
      PADR = 0xFB;
  }
  i++;
  TISRA&=0xFB;
}

char matrix_key_read(void)
{
  char key;
  unsigned char column, row;
  key = 0;
  column = 0x10;
  while(column<=0x40)
  {
    P4DR=~column;
    row=(~P4DR)&0x0F;
    if(row!=0)
    {
      switch(column | row)
      {
	  case 0x11: key='1'; break;
	  case 0x12: key='2'; break;
	  case 0x14: key='3'; break;
	  case 0x18: key='4'; break;
	  case 0x21: key='5'; break;
	  case 0x22: key='6'; break;
	  case 0x24: key='7'; break;
	  case 0x28: key='8'; break;
	  case 0x41: key='9'; break;
	  case 0x42: key='*'; break;
	  case 0x44: key='0'; break;
	  case 0x48: key='#'; break;
	  default: key=0; break;
      }
    }
    column = column<<1;
  }
  return key;
}

void main(void)
{
  int state, pre_state;
  P4DDR = 0xF0;
  P5DDR = 0xF0;
  P5PCR = 0xFF;
  PADDR = 0xFF;
  PADR = 0xFF;
  TISRA |= 0x40;
  TCR2=0xA3;
  GRA2=2499;
  PADR = 0xFB;
  set_imask_ccr(0);
  TSTR=0xFC;
  while(1)
  {
    switch(state=matrix_key_read())
    {
	  case '1': GRA2=2499/5; break;
	  case '2': GRA2=2499*2/5; break;
	  case '3': GRA2=2499*3/5; break;
	  case '4': GRA2=2499*4/5; break;
	  case '5': GRA2=2499; break;
	  case '6': GRA2=2499*6/5; break;
	  case '7': GRA2=2499*7/5; break;
	  case '8': GRA2=2499*8/5; break;
	  case '9': GRA2=2499*9/5; break;
	  case '*': break;
	  case '0': break;
	  case '#': break;
	  default:  break;
    }
    if(state!=pre_state){
      i=0;
      TISRA&=0xFB;
      TSTR=0xFC;
    }
    pre_state = state;
  }
}
