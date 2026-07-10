/*
 * ssr50da_driver.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_SSR50DA_DRIVER_H_
#define INC_SSR50DA_DRIVER_H_

// ============================ //
// Librerias
// ============================ //
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// ============================ //
// Estructura de datos de control del SSR50DA
// ============================ //
typedef struct
{
    GPIO_TypeDef *gpio_port;            // Puerto GPIO al que está conectado el SSR50DA
    uint16_t gpio_pin;                  // Pin GPIO al que está conectado el SSR50DA
    float duty;                         // Ciclo de trabajo actual (0.0 a 1.0)
    uint32_t window_start_tick;         // Marca de tiempo del inicio de la ventana de control
    bool initialized;                   // Indica si el SSR50DA ha sido inicializado correctamente

} SSR50DA_t;

// ============================ //
// Prototipos de funciones
// ============================ //
void SSR50DA_Init(SSR50DA_t *ssr, GPIO_TypeDef *gpio_port, uint16_t gpio_pin);
void SSR50DA_SetDuty(SSR50DA_t *ssr, float duty);
void SSR50DA_Update(SSR50DA_t *ssr);
void SSR50DA_Off(SSR50DA_t *ssr);
float SSR50DA_GetDuty(SSR50DA_t *ssr);

#endif /* INC_SSR50DA_DRIVER_H_ */
