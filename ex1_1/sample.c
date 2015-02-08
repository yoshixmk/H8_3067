#include <sci.h>
#include <reg3067.h>

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

void main(void)
{
  P5DDR = 0xF0;
  P5PCR = 0xFF;
  PADDR = 0xFF;
  PADR = 0xFF;
  
  while(1)
  {
    PADR = 0xFB;
    ms_timer(500);
    PADR = 0xF7;
    ms_timer(500);
  }
}
