/*
 * safety.h
 *
 *  Created on: 21 jun 2026
 *      Author: rsahu
 */

#ifndef INC_SAFETY_H_
#define INC_SAFETY_H_

// ======================== //
// Librerias
// ======================== //
#include <stdint.h>
#include <stdbool.h>

// ======================== //
// Definiciones
// ======================== //
#define ALARM_NONE              0x00000000UL        // Sin alarmas
#define ALARM_OVERTEMP          0x00000001UL        // Alarma de sobretemperatura
#define ALARM_TEMP_SENSOR       0x00000002UL        // Alarma de fallo en el sensor de temperatura
#define ALARM_MOTOR_FAULT       0x00000004UL        // Alarma de fallo en el motor
#define ALARM_WINDER_TIMEOUT    0x00000008UL        // Alarma de tiempo de espera en el enrolladora
#define ALARM_EMERGENCY_STOP    0x00000010UL        // Alarma de parada de emergencia

// ======================== //
// Prototipos de funciones
// ======================== //
void Safety_Init(void);
void Safety_Update(void);
bool Safety_HasFault(void);
uint32_t Safety_GetAlarmFlags(void);
void Safety_SetFault(uint32_t flags);
void Safety_ClearFault(uint32_t flags);
void Safety_ClearAll(void);

#endif /* INC_SAFETY_H_ */
