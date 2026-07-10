/*
 * process_data.h
 *
 *  Created on: 21 jun 2026
 *      Author: rsahu
 */

#ifndef INC_PROCESS_DATA_H_
#define INC_PROCESS_DATA_H_

// ========================== //
// Librerías
// ========================== //
#include <stdint.h>
#include <stdbool.h>

// ========================== //
// Estructura de datos
// ========================== //
typedef struct
{
    uint8_t winder_state;       // Estado de la bobinadora
    uint32_t alarm_flags;       // Flags de alarma
    float rpm_setpoint;         // Consigna de RPM
    bool guide_enable;          // Habilitación de guía
    bool winder_enable;         // Habilitación de bobinadora
    bool alarm_active;          // Estado de alarma activa

} ProcessData_t;

// ========================== //
// Prototipos de funciones
// ========================== //
void ProcessData_Init(void);
void ProcessData_GetSnapshot(ProcessData_t *out);
void ProcessData_SetWinderState(uint8_t state);
void ProcessData_SetAlarmFlags(uint32_t flags);
void ProcessData_SetRpmSetpoint(float rpm);
void ProcessData_SetFSMOutputs(bool guide,bool winder, bool alarm);

#endif /* INC_PROCESS_DATA_H_ */
