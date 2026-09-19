#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "LIB.h"


void main()
{
stdio_init_all();

printf("%x", *((volatile uint32_t *) (BASE_PSM  +   0x0)));

  while (true)
  {



  }  


}