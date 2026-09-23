#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "LIB.h"
#include  "Module.h"



void SYS_INIT(void)
{
    HC_SR04_init(&Distance_Calculator, DIS_TRIG, DIS_ECHO);


    RELAY_init (&Pump_Switch , RLY_CTRL_IO);

    STS_LED_init (&Status_LED , LED_RD, LED_GN, LED_BU);

    


}

int main(void)
{
    stdio_init_all();

    SYS_INIT();

    Status_LED.RD_ON(&Status_LED);

    while (true)
    {
        get_distance(&Distance_Calculator);
        printf("Distance: %.2f mm\n", Distance_Calculator.Distance);
    }

    return 0;
}