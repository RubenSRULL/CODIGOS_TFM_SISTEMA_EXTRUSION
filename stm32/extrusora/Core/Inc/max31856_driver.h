/*
 * max31856.h
 *
 *  Created on: 17 jun 2026
 *      Author: rsahu
 */


// .h transcrito y adaptado de librería de Arduino Adafruit_MAX31856

#ifndef INC_MAX31856_DRIVER_H_
#define INC_MAX31856_DRIVER_H_

#include "stm32g4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* ===== Registros MAX31856 ===== */
#define MAX31856_CR0_REG             0x00
#define MAX31856_CR0_AUTOCONVERT     0x80
#define MAX31856_CR0_1SHOT           0x40
#define MAX31856_CR0_OCFAULT1        0x20
#define MAX31856_CR0_OCFAULT0        0x10
#define MAX31856_CR0_CJ              0x08
#define MAX31856_CR0_FAULT           0x04
#define MAX31856_CR0_FAULTCLR        0x02

#define MAX31856_CR1_REG             0x01
#define MAX31856_MASK_REG            0x02
#define MAX31856_CJHF_REG            0x03
#define MAX31856_CJLF_REG            0x04
#define MAX31856_LTHFTH_REG          0x05
#define MAX31856_LTHFTL_REG          0x06
#define MAX31856_LTLFTH_REG          0x07
#define MAX31856_LTLFTL_REG          0x08
#define MAX31856_CJTO_REG            0x09
#define MAX31856_CJTH_REG            0x0A
#define MAX31856_CJTL_REG            0x0B
#define MAX31856_LTCBH_REG           0x0C
#define MAX31856_LTCBM_REG           0x0D
#define MAX31856_LTCBL_REG           0x0E
#define MAX31856_SR_REG              0x0F

/* ===== Flags de fallo ===== */

#define MAX31856_FAULT_CJRANGE       0x80
#define MAX31856_FAULT_TCRANGE       0x40
#define MAX31856_FAULT_CJHIGH        0x20
#define MAX31856_FAULT_CJLOW         0x10
#define MAX31856_FAULT_TCHIGH        0x08
#define MAX31856_FAULT_TCLOW         0x04
#define MAX31856_FAULT_OVUV          0x02
#define MAX31856_FAULT_OPEN          0x01

/* ===== Opciones de filtrado ===== */

typedef enum
{
    MAX31856_NOISE_FILTER_50HZ = 0,
    MAX31856_NOISE_FILTER_60HZ

} max31856_noise_filter_t;

/* ===== Tipos de termopar ===== */

typedef enum
{
    MAX31856_TCTYPE_B  = 0x00,
    MAX31856_TCTYPE_E  = 0x01,
    MAX31856_TCTYPE_J  = 0x02,
    MAX31856_TCTYPE_K  = 0x03,
    MAX31856_TCTYPE_N  = 0x04,
    MAX31856_TCTYPE_R  = 0x05,
    MAX31856_TCTYPE_S  = 0x06,
    MAX31856_TCTYPE_T  = 0x07,
    MAX31856_VMODE_G8  = 0x08,
    MAX31856_VMODE_G32 = 0x0C

} max31856_thermocoupletype_t;

/* ===== Modos de medición ===== */

typedef enum
{
    MAX31856_ONESHOT = 0,
    MAX31856_ONESHOT_NOWAIT,
    MAX31856_CONTINUOUS

} max31856_conversion_mode_t;

/* ===== Estructura del dispositivo ===== */

typedef struct
{
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;

    bool initialized;

    max31856_conversion_mode_t conversionMode;

} MAX31856_t;

/* ===== Funciones públicas ===== */

void MAX31856_InitDevice(MAX31856_t *dev, SPI_HandleTypeDef *hspi,GPIO_TypeDef *cs_port, uint16_t cs_pin);

bool MAX31856_Begin(MAX31856_t *dev);

void MAX31856_SetConversionMode(MAX31856_t *dev, max31856_conversion_mode_t mode);

max31856_conversion_mode_t MAX31856_GetConversionMode(MAX31856_t *dev);

void MAX31856_SetThermocoupleType(MAX31856_t *dev, max31856_thermocoupletype_t type);

max31856_thermocoupletype_t MAX31856_GetThermocoupleType(MAX31856_t *dev);

uint8_t MAX31856_ReadFault(MAX31856_t *dev);

void MAX31856_ClearFault(MAX31856_t *dev);

void MAX31856_TriggerOneShot(MAX31856_t *dev);

bool MAX31856_ConversionComplete(MAX31856_t *dev);

float MAX31856_ReadCJTemperature(MAX31856_t *dev);

float MAX31856_ReadThermocoupleTemperature(MAX31856_t *dev);

void MAX31856_SetTempFaultThreshholds(MAX31856_t *dev, float flow, float fhigh);

void MAX31856_SetColdJunctionFaultThreshholds(MAX31856_t *dev, int8_t low, int8_t high);

void MAX31856_SetNoiseFilter(MAX31856_t *dev, max31856_noise_filter_t noiseFilter);

#endif /* INC_MAX31856_DRIVER_H_ */
