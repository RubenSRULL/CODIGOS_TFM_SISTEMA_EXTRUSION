/*
 * motor_control.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_

// ========================== //
// Librerias
// ========================== //
#include <stdint.h>

// ========================== //
// Definiciones
// ========================== //
#define MOTOR_CONTROL_PERIOD_MS          10U            // Periodo de control del motor en milisegundos
#define MOTOR_CONTROL_MIN_MELT_TEMP_C    180.0f         // Temperatura mínima de fusión en grados Celsius
#define MOTOR_CONTROL_TEMP_HYST_C        5.0f           // Histeresis de temperatura en grados Celsius

// ========================== //
// Prototipos de funciones
// ========================== //
void MotorControl_Init(void);
void MotorControl_Update(void);

#endif /* INC_MOTOR_CONTROL_H_ */
