#ifndef __SHT_H_
#define __SHT_H_

#include "stm32f10x.h"
#include "delay.h"
#include "bluetooth.h"
/*******************

P0.1 -->  PC1(SDA)
P1.7 -->  PD7(SCL)

*******************/

typedef struct sht_port_t
{
    GPIO_TypeDef *scl_gpio;
    uint16_t scl_gpio_pin;
    GPIO_TypeDef *sda_gpio;
    uint16_t sda_gpio_pin;
} sht_port_t;

uint8_t  sht_read(sht_port_t *port, float *temp, float *humi);

#endif
