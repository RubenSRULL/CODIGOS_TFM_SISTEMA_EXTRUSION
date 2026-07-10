/*
 * motor_guide_control.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_MOTOR_GUIDE_CONTROL_H_
#define INC_MOTOR_GUIDE_CONTROL_H_

// ====================== //
// Librerias              //
// ====================== //
#include <stdint.h>

// ====================== //
// Definiciones           //
// ====================== // 
#define MOTOR_GUIDE_CONTROL_PERIOD_MS    10U            // Periodo de control del motor guía en milisegundos
#define GUIDE_LIMIT_DEBOUNCE_MS          40U            // Tiempo de debounce para los límites del motor guía en milisegundos
#define GUIDE_TIMER_CLK_HZ               1000000.0f     // Frecuencia del reloj del temporizador en Hz
#define GUIDE_STEPS_PER_REV              200.0f         // Número de pasos por revolución del motor guía
#define GUIDE_RPM_MAX                    120.0f         // Velocidad máxima del motor guía en RPM
#define GUIDE_RPM                        40.0f          // Velocidad actual del motor guía en RPM
#define GUIDE_TIMER_ARR_MAX              4294967295U    // Valor máximo del registro ARR del temporizador (32 bits)

// ====================== //
// Prototipos de funciones //
// ====================== //
void MotorGuideControl_Init(void);
void MotorGuideControl_Update(void);


#endif /* INC_MOTOR_GUIDE_CONTROL_H_ */
