/*
 * max31856.c
 *
 *  Created on: 17 jun 2026
 *      Author: rsahu
 */


// .c transcrito de librería de Arduino Adafruit_MAX31856


#include "max31856_driver.h"
#include <math.h>


#define MAX31856_WRITE_BIT      0x80
#define MAX31856_READ_MASK      0x7F
#define MAX31856_SPI_TIMEOUT    100U


static void MAX31856_Select(MAX31856_t *dev)
{
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
}


static void MAX31856_Unselect(MAX31856_t *dev)
{
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}


static void MAX31856_ReadRegisterN(MAX31856_t *dev, uint8_t addr, uint8_t buffer[], uint8_t n)
{
    addr &= MAX31856_READ_MASK;
    MAX31856_Select(dev);
    HAL_SPI_Transmit(dev->hspi, &addr, 1, MAX31856_SPI_TIMEOUT);
    HAL_SPI_Receive(dev->hspi,buffer,n, MAX31856_SPI_TIMEOUT);
    MAX31856_Unselect(dev);
}


static uint8_t MAX31856_ReadRegister8(MAX31856_t *dev, uint8_t addr)
{
    uint8_t ret = 0;
    MAX31856_ReadRegisterN(dev, addr, &ret,1);
    return ret;
}


static uint16_t MAX31856_ReadRegister16(MAX31856_t *dev, uint8_t addr)
{
    uint8_t buffer[2] = {0, 0};
    uint16_t ret;
    MAX31856_ReadRegisterN(dev, addr, buffer, 2);
    ret = buffer[0];
    ret <<= 8;
    ret |= buffer[1];
    return ret;
}


static uint32_t MAX31856_ReadRegister24(MAX31856_t *dev, uint8_t addr)
{
    uint8_t buffer[3] = {0, 0, 0};
    uint32_t ret;
    MAX31856_ReadRegisterN(dev, addr, buffer, 3);
    ret = buffer[0];
    ret <<= 8;
    ret |= buffer[1];
    ret <<= 8;
    ret |= buffer[2];
    return ret;
}


static void MAX31856_WriteRegister8(MAX31856_t *dev, uint8_t addr, uint8_t data)
{
    uint8_t buffer[2];
    addr |= MAX31856_WRITE_BIT;
    buffer[0] = addr;
    buffer[1] = data;
    MAX31856_Select(dev);
    HAL_SPI_Transmit(dev->hspi, buffer, 2, MAX31856_SPI_TIMEOUT);
    MAX31856_Unselect(dev);
}

void MAX31856_InitDevice(MAX31856_t *dev, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    if (dev == NULL)
    {
        return;
    }
    dev->hspi = hspi;
    dev->cs_port = cs_port;
    dev->cs_pin = cs_pin;
    dev->initialized = false;
    dev->conversionMode = MAX31856_ONESHOT;
    MAX31856_Unselect(dev);
}


bool MAX31856_Begin(MAX31856_t *dev)
{
    if ((dev == NULL) || (dev->hspi == NULL))
    {
        return false;
    }
    MAX31856_Unselect(dev);
    HAL_Delay(10);
    MAX31856_WriteRegister8(dev, MAX31856_MASK_REG, 0x00);
    MAX31856_WriteRegister8(dev, MAX31856_CR0_REG, MAX31856_CR0_OCFAULT0);
    MAX31856_WriteRegister8(dev, MAX31856_CJTO_REG, 0x00);
    MAX31856_SetThermocoupleType(dev, MAX31856_TCTYPE_K);
    MAX31856_SetConversionMode(dev, MAX31856_ONESHOT);
    dev->initialized = true;
    return true;
}

void MAX31856_SetConversionMode(MAX31856_t *dev, max31856_conversion_mode_t mode)
{
    uint8_t t;
    if ((dev == NULL) || (dev->initialized == false))
    {
        return;
    }
    dev->conversionMode = mode;
    t = MAX31856_ReadRegister8(dev, MAX31856_CR0_REG);
    if (mode == MAX31856_CONTINUOUS)
    {
        t |= MAX31856_CR0_AUTOCONVERT;
        t &= (uint8_t)(~MAX31856_CR0_1SHOT);
    }
    else
    {
        t &= (uint8_t)(~MAX31856_CR0_AUTOCONVERT);
        t |= MAX31856_CR0_1SHOT;
    }
    MAX31856_WriteRegister8(dev, MAX31856_CR0_REG, t);
}


max31856_conversion_mode_t MAX31856_GetConversionMode(MAX31856_t *dev)
{
    if (dev == NULL)
    {
        return MAX31856_ONESHOT;
    }
    return dev->conversionMode;
}


void MAX31856_SetThermocoupleType(MAX31856_t *dev, max31856_thermocoupletype_t type)
{
    uint8_t t;
    if (dev == NULL)
    {
        return;
    }
    t = MAX31856_ReadRegister8(dev, MAX31856_CR1_REG);
    t &= 0xF0;
    t |= ((uint8_t)type & 0x0F);
    MAX31856_WriteRegister8(dev, MAX31856_CR1_REG, t);
}


max31856_thermocoupletype_t MAX31856_GetThermocoupleType(MAX31856_t *dev)
{
    uint8_t t;
    if (dev == NULL)
    {
        return MAX31856_TCTYPE_K;
    }
    t = MAX31856_ReadRegister8(dev, MAX31856_CR1_REG);
    t &= 0x0F;
    return (max31856_thermocoupletype_t)t;
}


uint8_t MAX31856_ReadFault(MAX31856_t *dev)
{
    if (dev == NULL)
    {
        return 0xFF;
    }
    return MAX31856_ReadRegister8(dev, MAX31856_SR_REG);
}


void MAX31856_ClearFault(MAX31856_t *dev)
{
    uint8_t t;
    if (dev == NULL)
    {
        return;
    }
    t = MAX31856_ReadRegister8(dev, MAX31856_CR0_REG);
    t |= MAX31856_CR0_FAULTCLR;
    MAX31856_WriteRegister8(dev, MAX31856_CR0_REG, t);
}


void MAX31856_TriggerOneShot(MAX31856_t *dev)
{
    uint8_t t;
    if ((dev == NULL) || (dev->conversionMode == MAX31856_CONTINUOUS))
    {
        return;
    }
    t = MAX31856_ReadRegister8(dev, MAX31856_CR0_REG);
    t &= (uint8_t)(~MAX31856_CR0_AUTOCONVERT);
    t |= MAX31856_CR0_1SHOT;
    MAX31856_WriteRegister8(dev, MAX31856_CR0_REG, t);
}


bool MAX31856_ConversionComplete(MAX31856_t *dev)
{
    uint8_t t;
    if (dev == NULL)
    {
        return false;
    }
    if (dev->conversionMode == MAX31856_CONTINUOUS)
    {
        return true;
    }
    t = MAX31856_ReadRegister8(dev, MAX31856_CR0_REG);
    if ((t & MAX31856_CR0_1SHOT) == 0)
    {
        return true;
    }
    return false;
}


float MAX31856_ReadCJTemperature(MAX31856_t *dev)
{
    if (dev == NULL)
    {
        return NAN;
    }
    return (float)MAX31856_ReadRegister16(dev, MAX31856_CJTH_REG) / 256.0f;
}


float MAX31856_ReadThermocoupleTemperature(MAX31856_t *dev)
{
    int32_t temp24;
    uint32_t start;
    if (dev == NULL)
    {
        return NAN;
    }
    if (dev->conversionMode == MAX31856_ONESHOT)
    {
        MAX31856_TriggerOneShot(dev);
        start = HAL_GetTick();
        while (MAX31856_ConversionComplete(dev) == false)
        {
            if ((HAL_GetTick() - start) > 250U)
            {
                return NAN;
            }
            HAL_Delay(10);
        }
    }
    temp24 = (int32_t)MAX31856_ReadRegister24(dev, MAX31856_LTCBH_REG);
    if ((temp24 & 0x800000) != 0)
    {
        temp24 |= 0xFF000000;
    }
    temp24 >>= 5;
    return (float)temp24 * 0.0078125f;
}


void MAX31856_SetTempFaultThreshholds(MAX31856_t *dev, float flow, float fhigh)
{
    int16_t low;
    int16_t high;
    if (dev == NULL)
    {
        return;
    }
    flow *= 16.0f;
    low = (int16_t)flow;
    fhigh *= 16.0f;
    high = (int16_t)fhigh;
    MAX31856_WriteRegister8(dev, MAX31856_LTHFTH_REG, (uint8_t)(high >> 8));
    MAX31856_WriteRegister8(dev, MAX31856_LTHFTL_REG, (uint8_t)high);
    MAX31856_WriteRegister8(dev, MAX31856_LTLFTH_REG, (uint8_t)(low >> 8));
    MAX31856_WriteRegister8(dev, MAX31856_LTLFTL_REG, (uint8_t)low);
}

void MAX31856_SetColdJunctionFaultThreshholds(MAX31856_t *dev, int8_t low, int8_t high)
{
    if (dev == NULL)
    {
        return;
    }
    MAX31856_WriteRegister8(dev, MAX31856_CJLF_REG, (uint8_t)low);
    MAX31856_WriteRegister8(dev, MAX31856_CJHF_REG, (uint8_t)high);
}

void MAX31856_SetNoiseFilter(MAX31856_t *dev, max31856_noise_filter_t noiseFilter)
{
    uint8_t t;
    if (dev == NULL)
    {
        return;
    }
    t = MAX31856_ReadRegister8(dev, MAX31856_CR0_REG);
    if (noiseFilter == MAX31856_NOISE_FILTER_50HZ)
    {
        t |= 0x01;
    }
    else
    {
        t &= 0xFE;
    }
    MAX31856_WriteRegister8(dev, MAX31856_CR0_REG, t);
}
