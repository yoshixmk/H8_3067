#include <sci.h>

#define P5DDR (*(volatile unsigned char *)0xEE004)
#define PADDR (*(volatile unsigned char *)0xEE009)
#define P5DR (*(volatile unsigned char *)0xFFFD4)
#define PADR (*(volatile unsigned char *)0xFFFD9)
#define P5PCR (*(volatile unsigned char *)0xEE03F)

void main(void)
{
  P5DDR = 0xF0;
  P5PCR = 0xFF;

  PADDR = 0xFF;
  PADR = 0xFF;

  sci_open();
  while(1)
  {
    if(P5DR == 0xFD){
      sci_puts("SW ON Pushed!!");
      PADR = 0xFB;
    }
    else if(P5DR == 0xFE){
      sci_puts("SW OFF Pushed!!");
      PADR = 0xF7;
    }
    else{
      PADR = 0xFF;
    }
  }
  sci_close();
}
