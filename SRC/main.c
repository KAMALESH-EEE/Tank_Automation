#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"

#include  "Module.h"


SYSTEM SWTC;


int main(void)
{
    stdio_init_all();

    SYS_init (&SWTC);
    

    SWTC.STATUS_LED.GN_ON(&SWTC.STATUS_LED);

    while (true)
    {
      SWTC.RUN(&SWTC);
    }

    return 0;
}