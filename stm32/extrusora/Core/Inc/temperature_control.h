/*
 * temperature_control.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */


#ifndef INC_TEMPERATURE_CONTROL_H_
#define INC_TEMPERATURE_CONTROL_H_

/* ========================================================= */
/* Librerías                                                 */
/* ========================================================= */
#include "main.h"
#include <stdbool.h>

/* Inicializa sensor, PID y salida SSR */
void TemperatureControl_Init(SPI_HandleTypeDef *hspi);
/* Actualización periódica del control de temperatura */
void TemperatureControl_Update(void);
/* Apaga el sistema de control de temperatura */
void TemperatureControl_Off(void);
/* Indica si el sensor de temperatura está funcionando correctamente */
bool TemperatureControl_IsSensorOk(void);

#endif /* INC_TEMPERATURE_CONTROL_H_ */
