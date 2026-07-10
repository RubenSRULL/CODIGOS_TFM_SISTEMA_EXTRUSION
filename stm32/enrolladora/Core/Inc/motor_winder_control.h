/*
 * motor_winder_control.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_MOTOR_WINDER_CONTROL_H_
#define INC_MOTOR_WINDER_CONTROL_H_

// ================================= //
// Definiciones                      //
// ================================= //
#define MOTOR_WINDER_CONTROL_PERIOD_MS    10U           // Tiempo de control del motor en milisegundos
#define WINDER_TIMER_CLK_HZ               1000000.0f    // Frecuencia del reloj del temporizador en Hz
#define WINDER_STEPS_PER_REV              200.0f        // Número de pasos por revolución del motor
#define WINDER_RPM_MAX                    120.0f        // Velocidad máxima del motor en RPM
#define WINDER_TIMER_ARR_MAX              65535U        // Valor máximo del registro ARR del temporizador (16 bits)

// ================================= //
// Prototipos de funciones             //
// ================================= //
void MotorWinderControl_Init(void);
void MotorWinderControl_Update(void);

#endif /* INC_MOTOR_WINDER_CONTROL_H_ */
