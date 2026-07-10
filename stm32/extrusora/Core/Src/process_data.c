/*
 * procee_data.c
 *
 *  Created on: 21 jun 2026
 *      Author: rsahu
 */

// ================================= //
// Librerias
// ================================= //
#include "process_data.h"
#include "cmsis_os2.h"
#include <string.h>

// ================================= //
// Variables globales
// ================================= //
static ProcessData_t g_process;
static osMutexId_t g_process_mutex;

// -- Bloqueo de acceso a la estructura de datos del proceso con mutex
static void ProcessData_Lock(void)
{
    if (g_process_mutex != NULL)
    {
        osMutexAcquire(g_process_mutex, osWaitForever);
    }
}

// -- Desbloqueo de acceso a la estructura de datos del proceso con mutex
static void ProcessData_Unlock(void)
{
    if (g_process_mutex != NULL)
    {
        osMutexRelease(g_process_mutex);
    }
}

// ================================= //
// Funciones de acceso a la estructura de datos del proceso
// ================================= //
void ProcessData_Init(void)
{
    const osMutexAttr_t mutex_attr = {
        .name = "ProcessDataMutex"
    };

    g_process_mutex = osMutexNew(&mutex_attr);

    ProcessData_Lock();

    memset(&g_process, 0, sizeof(g_process));

    g_process.extruder_state = 0;
    g_process.winder_state = 0;
    g_process.target_temp_c = 0.0f;
    g_process.target_diameter_mm = 1.75f;
    g_process.rpm_int_setpoint = 0.0f;
    g_process.rpm_ext_setpoint = 0.0f;

    ProcessData_Unlock();
}

void ProcessData_GetSnapshot(ProcessData_t *out)
{
    if (out == NULL)
    {
        return;
    }

    ProcessData_Lock();
    *out = g_process;
    ProcessData_Unlock();
}

void ProcessData_SetTemperature(float temp_c)
{
    ProcessData_Lock();
    g_process.temp_c = temp_c;
    ProcessData_Unlock();
}

void ProcessData_SetTargetTemp(float target_c)
{
    ProcessData_Lock();
    g_process.target_temp_c = target_c;
    ProcessData_Unlock();
}

void ProcessData_SetHeaterPercent(float percent)
{
    if (percent < 0.0f)
    {
        percent = 0.0f;
    }

    if (percent > 100.0f)
    {
        percent = 100.0f;
    }

    ProcessData_Lock();
    g_process.heater_percent = percent;
    ProcessData_Unlock();
}

void ProcessData_SetDiameter(float diameter_mm)
{
    ProcessData_Lock();
    g_process.diameter_mm = diameter_mm;
    ProcessData_Unlock();
}

void ProcessData_SetTargetDiameter(float target_diameter_mm)
{
    ProcessData_Lock();
    g_process.target_diameter_mm = target_diameter_mm;
    ProcessData_Unlock();
}

void ProcessData_SetRpmIntSetpoint(float rpm)
{
    if (rpm < 0.0f)
    {
        rpm = 0.0f;
    }

    ProcessData_Lock();
    g_process.rpm_int_setpoint = rpm;
    ProcessData_Unlock();
}

void ProcessData_SetRpmExtSetpoint(float rpm)
{
    if (rpm < 0.0f)
    {
        rpm = 0.0f;
    }

    ProcessData_Lock();
    g_process.rpm_ext_setpoint = rpm;
    ProcessData_Unlock();
}

void ProcessData_SetExtruderState(uint8_t state)
{
    ProcessData_Lock();
    g_process.extruder_state = state;
    ProcessData_Unlock();
}

void ProcessData_SetWinderState(uint8_t state)
{
    ProcessData_Lock();
    g_process.winder_state = state;
    ProcessData_Unlock();
}

void ProcessData_SetAlarmFlags(uint32_t flags)
{
    ProcessData_Lock();
    g_process.alarm_flags = flags;
    ProcessData_Unlock();
}

void ProcessData_SetWinderAlarmFlags(uint32_t flags)
{
    ProcessData_Lock();
    g_process.winder_alarm_flags = flags;
    ProcessData_Unlock();
}

void ProcessData_SetFSMOutputs(
    bool heater,
    bool extruder,
    bool winder,
    bool manual,
    bool alarm
)
{
    ProcessData_Lock();

    g_process.heater_enable = heater;
    g_process.extruder_enable = extruder;
    g_process.winder_enable = winder;
    g_process.manual_enable = manual;
    g_process.alarm_active = alarm;

    ProcessData_Unlock();
}
