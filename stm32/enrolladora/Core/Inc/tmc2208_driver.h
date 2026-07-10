/*
 * tmc2208_driver.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_TMC2208_DRIVER_H_
#define INC_TMC2208_DRIVER_H_

// ========================== //
// Librerias                  //
// ========================== //
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// ============================== //
// Estructura de datos de TMC2208 //
// ============================== //
typedef struct
{
    TIM_HandleTypeDef *htim;        // Handle del temporizador utilizado para generar la señal PWM
    uint32_t channel;               // Canal del temporizador utilizado para generar la señal PWM

    GPIO_TypeDef *dir_port;         // Puerto GPIO utilizado para la señal de dirección
    uint16_t dir_pin;               // Pin GPIO utilizado para la señal de dirección

    GPIO_TypeDef *en_port;          // Puerto GPIO utilizado para la señal de habilitación
    uint16_t en_pin;                // Pin GPIO utilizado para la señal de habilitación

    float timer_clk_hz;             // Frecuencia del reloj del temporizador en Hz
    float steps_per_rev;            // Número de pasos por revolución del motor paso a paso
    float rpm_max;                  // Velocidad máxima del motor en revoluciones por minuto   

    uint32_t arr_max;               // Valor máximo del registro ARR del temporizador

    bool invert_dir;                // Indica si la dirección del motor debe invertirse
    bool initialized;               // Indica si el controlador ha sido inicializado
    bool running;                   // Indica si el motor está en funcionamiento
    
    uint32_t last_arr;              // Último valor del registro ARR utilizado para generar la señal PWM

} TMC2208_t;

// ========================== //
// Prototipos de funciones //
// ========================== //
void TMC2208_Init(
    TMC2208_t *drv,
    TIM_HandleTypeDef *htim,
    uint32_t channel,
    GPIO_TypeDef *dir_port,
    uint16_t dir_pin,
    GPIO_TypeDef *en_port,
    uint16_t en_pin,
    float timer_clk_hz,
    float steps_per_rev,
    float rpm_max,
    uint32_t arr_max,
    bool invert_dir
);

void TMC2208_Stop(TMC2208_t *drv);
void TMC2208_SetRpm(TMC2208_t *drv, float rpm);
bool TMC2208_IsRunning(TMC2208_t *drv);

#endif /* INC_TMC2208_DRIVER_H_ */
