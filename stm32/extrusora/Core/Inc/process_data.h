/*
 * process_data.h
 *
 *  Created on: 21 jun 2026
 *      Author: rsahu
 */

#ifndef INC_PROCESS_DATA_H_
#define INC_PROCESS_DATA_H_

// =========================== //
// Librerias
// =========================== //
#include <stdint.h>
#include <stdbool.h>

// ========================== //
// Estructura de datos
// ========================== //
typedef struct
{
    /* Temperatura */
    float temp_c;
    float target_temp_c;
    float heater_percent;

    /* Diámetro */
    float diameter_mm;
    float target_diameter_mm;

    /* Velocidades */
    float rpm_int_setpoint;
    float rpm_ext_setpoint;

    /* Estados FSM */
    uint8_t extruder_state;
    uint8_t winder_state;

    /* Alarmas */
    uint32_t alarm_flags;
    uint32_t winder_alarm_flags;

    /* Salidas FSM */
    bool heater_enable;
    bool extruder_enable;
    bool winder_enable;
    bool manual_enable;
    bool alarm_active;

} ProcessData_t;

// ========================== //
// Prototipos de funciones
// ========================== //

void ProcessData_Init(void);
void ProcessData_GetSnapshot(ProcessData_t *out);

/* Temperatura */
void ProcessData_SetTemperature(float temp_c);
void ProcessData_SetTargetTemp(float target_c);
void ProcessData_SetHeaterPercent(float percent);

/* Diámetro */
void ProcessData_SetDiameter(float diameter_mm);
void ProcessData_SetTargetDiameter(float target_diameter_mm);

/* RPM */
void ProcessData_SetRpmIntSetpoint(float rpm);
void ProcessData_SetRpmExtSetpoint(float rpm);

/* Estados */
void ProcessData_SetExtruderState(uint8_t state);
void ProcessData_SetWinderState(uint8_t state);

/* Alarmas */
void ProcessData_SetAlarmFlags(uint32_t flags);
void ProcessData_SetWinderAlarmFlags(uint32_t flags);

/* Salidas FSM */
void ProcessData_SetFSMOutputs(
    bool heater,
    bool extruder,
    bool winder,
    bool manual,
    bool alarm
);

#endif /* INC_PROCESS_DATA_H_ */
