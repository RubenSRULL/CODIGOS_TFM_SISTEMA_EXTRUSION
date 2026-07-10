/*
 * edu301d_driver.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_EDU301D_DRIVER_H_
#define INC_EDU301D_DRIVER_H_

// =============================== //
// Librerias
// =============================== //
#include <stdint.h>
#include <stdbool.h>


// =============================== //
// Definiciones
// =============================== //
#define EDU301D_TIMER_CLK_HZ        1000000.0f      // Frecuencia del timer en Hz
#define EDU301D_STEPS_PER_REV         20000.0f      // Cantidad de pasos por vuelta del motor (microstepping)
#define EDU301D_GEAR_RATIO            2.25f         // Relación de engranajes del motor
#define EDU301D_RPM_MAX               20.0f         // Velocidad máxima del motor en RPM

// =============================== //
// Prototipos de funciones
// =============================== //
void EDU301D_Init(void);
void EDU301D_Stop(void);
void EDU301D_SetRpm(float rpm);
bool EDU301D_IsRunning(void);

#endif /* INC_EDU301D_DRIVER_H_ */
