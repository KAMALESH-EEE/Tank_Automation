#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "LIB.h"

void GPIO__INIT(uint8_t pin, uint8_t fun)
{

  volatile uint32_t * REG_ptr;

  REG_ptr = (volatile uint32_t*) (BASE_IO_BANK + (pin * 8) + 4); // pointing GPIO_CTRL reg of <pin> pin

  *REG_ptr = fun; // setting to SIO function

}

void GPIO__DIR(uint8_t pin, bool dir)
{

  volatile uint32_t * REG_ptr;

  IO_DIR_REG &= ~(1 << pin); // clear the direction

  IO_DIR_REG |= ((dir & 0x1) << pin); // setting new direction

  if (! dir) {

    REG_ptr = (volatile uint32_t*) (BASE_PAD + (pin * 4) + 4); // pointing GPIO_CTRL reg of <pin> pin

    *REG_ptr |= (1 << 6); // Input Enable

  }

}

void GPIO__OUT(uint8_t pin, bool sts)
{

  IO_OUT_REG &= ~(1 << pin); // clear the state

  IO_OUT_REG |= ((sts & 0x1) << pin); // setting new state

}


uint64_t get_time_uS ()
{
  return (uint64_t)((((uint64_t)TIM_RAW_H_REG) << 32) | TIM_RAW_L_REG);
}

void usleep (int64_t uS)
{

  uint64_t tt;

  tt = get_time_uS () + uS;

  while (true)    if (tt <= get_time_uS())  break;

}


float get_distance (HC_SR04 *self)
{

  uint64_t st = 0, time_out = 5000000;

  /* Pulse for 10 uSec*/
  GPIO__OUT (self->TRIG_PIN, GPIO__HIGH);

  INTR0_REG = (0xc) << (self->ECHO_PIN * 4); // Clearing the Interrupt

  usleep(10);
  GPIO__OUT (self->TRIG_PIN, GPIO__LOW);

  /*wait for Echo pin High and latch the time*/

  while (time_out > 0)
  {
    if (((INTR0_REG >> ((self->ECHO_PIN * 4) + 3 ) ) & 0x1) == 0x1)
    {

      st = get_time_uS ();
      time_out = 5000000;
      break;
    }
    time_out -= 1;
  }


  /*wait for Echo pin LOW and Calculate the time*/
  while (time_out > 0)
  {
    if (((INTR0_REG >> ((self->ECHO_PIN * 4) + 2 )) & 0x1) == 0x1)
    {

      st = (get_time_uS() - st);
      self->Distance = (float) st / 5.8 ;
      return 1;
    }

    time_out -= 1;

  }

  self->Distance = 100000.0;
  return 0;
}



void HC_SR04_init (HC_SR04 *self, uint32_t TRIG_PIN, uint32_t ECHO_PIN)
{



  GPIO__INIT (TRIG_PIN, FUN_SIO);
  GPIO__INIT (ECHO_PIN, FUN_SIO);

  GPIO__DIR (TRIG_PIN, GPIO__CONFIG_OUT);
  GPIO__DIR (ECHO_PIN, GPIO__CONFIG_IN);

  C0_INTE0_REG |= (0xc << ((ECHO_PIN * 4)));

  self->TRIG_PIN = TRIG_PIN;
  self->ECHO_PIN = ECHO_PIN;


  self->Distance = 100;

  self->get_distance = &get_distance;

}


void i2c_write(I2C1 *self, uint8_t Addr, uint8_t Data)
{

  I2C1_TAR_REG = self -> SlaveID;


  while (true)    if ((I2C1_TX_FL_REG & 0xff) < 0xff) break;
  I2C1_CMD_REG = (I2C_RESTART | I2C_WRITE) | (Addr & 0xff);


  while (true)    if ((I2C1_TX_FL_REG & 0xff) < 0xff) break;
  I2C1_CMD_REG = (I2C_STOP | I2C_WRITE) | (Data & 0xff);
  usleep(1000);

}

uint8_t i2c_read(I2C1 *self, uint8_t Addr)
{

  I2C1_TAR_REG = self -> SlaveID;

  while (true)    if ((I2C1_TX_FL_REG & 0xff) < 0xff)    break;
  I2C1_CMD_REG = (I2C_RESTART | I2C_WRITE) | ((Addr) & 0xff);


  while (true)    if ((I2C1_TX_FL_REG & 0xff) < 0xff)    break;
  I2C1_CMD_REG = (I2C_RESTART | I2C_STOP | I2C_READ);


  while (true)    if ((I2C1_RX_FL_REG & 0xff))     break;
  usleep(1000);

  return (uint8_t)(I2C1_CMD_REG & 0xff);

}


void I2C_DEVICE (I2C1 *S_dev, uint8_t S_ID)

{

  GPIO__INIT (27, FUN_I2C);
  GPIO__INIT (26, FUN_I2C);



  S_dev->SlaveID = S_ID;

  S_dev->Read  = i2c_read ;
  S_dev->Write = i2c_write;

  I2C1_EN_REG = 0;

  I2C1_SCK_HCNT_REG = 600;
  I2C1_SCK_LCNT_REG = 600;

  I2C1_CTRL_REG     = (I2C_S_DIS | I2C_M_RESTART | I2C_SPEED_STD | I2C_M_EN);

  I2C1_EN_REG = 1;

}

void RELAY_TURN_ON (RELAY *self)
{

  GPIO__OUT   (self->CTRL_IO, GPIO__HIGH);

}


void RELAY_TURN_OFF (RELAY *self)
{

  GPIO__OUT   (self->CTRL_IO, GPIO__LOW);

}



void RELAY_init (RELAY *self, uint32_t IO)
{

  self->CTRL_IO     =   IO;


  GPIO__INIT (IO, FUN_SIO);
  GPIO__DIR  (IO, GPIO__CONFIG_OUT);
  GPIO__OUT  (IO, GPIO__HIGH);


  self->TURN_ON     =   &RELAY_TURN_ON;
  self->TURN_OFF    =   &RELAY_TURN_OFF;


}



void STS_LED_OFF(STS_LED *self)
{
  
  GPIO__OUT(self->RD_PIN,GPIO__LOW);
  GPIO__OUT(self->GN_PIN,GPIO__LOW);
  GPIO__OUT(self->BU_PIN,GPIO__LOW);

}

void STS_LED_RD_ON(STS_LED *self)
{

  GPIO__OUT(self->RD_PIN,GPIO__HIGH);
  GPIO__OUT(self->GN_PIN,GPIO__LOW);
  GPIO__OUT(self->BU_PIN,GPIO__LOW);

}

void STS_LED_GN_ON(STS_LED *self)
{

  GPIO__OUT(self->RD_PIN,GPIO__LOW);
  GPIO__OUT(self->GN_PIN,GPIO__HIGH);
  GPIO__OUT(self->BU_PIN,GPIO__LOW);

}

void STS_LED_BU_ON(STS_LED *self)
{

  GPIO__OUT(self->RD_PIN,GPIO__LOW);
  GPIO__OUT(self->GN_PIN,GPIO__LOW);
  GPIO__OUT(self->BU_PIN,GPIO__HIGH);

}


void STS_LED_YL_ON(STS_LED *self)
{

  GPIO__OUT(self->RD_PIN,GPIO__HIGH);
  GPIO__OUT(self->GN_PIN,GPIO__HIGH);
  GPIO__OUT(self->BU_PIN,GPIO__LOW);

}


void STS_LED_init (STS_LED *self, uint32_t RD, uint32_t GN, uint32_t BU)
{

  self->RD_PIN     =   RD;
  self->GN_PIN     =   GN;
  self->BU_PIN     =   BU;


  GPIO__INIT (RD, FUN_SIO);
  GPIO__INIT (GN, FUN_SIO);
  GPIO__INIT (BU, FUN_SIO);


  GPIO__DIR  (RD, GPIO__CONFIG_OUT);
  GPIO__DIR  (GN, GPIO__CONFIG_OUT);
  GPIO__DIR  (BU, GPIO__CONFIG_OUT);



  GPIO__OUT  (RD, GPIO__HIGH);
  GPIO__OUT  (GN, GPIO__HIGH);
  GPIO__OUT  (BU, GPIO__HIGH);



  self->OFF   = &STS_LED_OFF;
  self->RD_ON = &STS_LED_RD_ON;
  self->GN_ON = &STS_LED_GN_ON;
  self->BU_ON = &STS_LED_BU_ON;


  
}

