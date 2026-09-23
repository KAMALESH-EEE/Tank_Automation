#ifndef MODULE_H
#define MODULE_H


// PIN Decleration


#define   DIS_TRIG    3
#define   DIS_ECHO    2

#define   PUS_BUTTON  13

#define   RLY_CTRL_IO 27

#define   LED_RD      20
#define   LED_GN      19
#define   LED_BU      18

#define   BUZZER      17


// Modules Decleration
HC_SR04 Distance_Calculator;

RELAY Pump_Switch;

STS_LED Status_LED;


#endif