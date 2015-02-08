#include <sci.h>
#include <reg3067.h>
#include <machine.h>
#pragma interrupt(int_ms_timer)

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
  static int i;
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

void main(void)
{
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
    
  }
}
