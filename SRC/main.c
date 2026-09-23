#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "LIB.h"


HC_SR04 Distance_Calculator;



void SYS_INIT(void)
{
    HC_SR04_init(&Distance_Calculator, 3, 2);
}

int main(void)
{
    stdio_init_all();

    SYS_INIT();

    while (true)
    {
        get_distance(&Distance_Calculator);
        printf("Distance: %.2f mm\n", Distance_Calculator.Distance);
    }

    return 0;
}