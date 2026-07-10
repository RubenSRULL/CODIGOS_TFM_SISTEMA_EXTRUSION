/*
 * process_data.c
 *
 *  Created on: 21 jun 2026
 *      Author: rsahu
 */

// ========================= //
// Librerias
// ========================= //
#include "process_data.h"
#include "cmsis_os2.h"
#include <string.h>

// ========================= //
// Variables privadas
// ========================= //
static ProcessData_t g_process;
static osMutexId_t g_process_mutex;

static void ProcessData_Lock(void)
{
    if (g_process_mutex != NULL)
    {
        osMutexAcquire(g_process_mutex, osWaitForever);
    }
}

static void ProcessData_Unlock(void)
{
    if (g_process_mutex != NULL)
    {
        osMutexRelease(g_process_mutex);
    }
}

void ProcessData_Init(void)
{
    const osMutexAttr_t mutex_attr = {
        .name = "ProcessDataMutex"
    };

    g_process_mutex = osMutexNew(&mutex_attr);

    ProcessData_Lock();

    memset(&g_process, 0, sizeof(g_process));

    g_process.winder_state = 0;
    g_process.rpm_setpoint = 0.0f;

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

void ProcessData_SetRpmSetpoint(float rpm)
{
    if (rpm < 0.0f)
    {
        rpm = 0.0f;
    }

    ProcessData_Lock();
    g_process.rpm_setpoint = rpm;
    ProcessData_Unlock();
}

void ProcessData_SetFSMOutputs(
    bool guide,
    bool winder,
    bool alarm
)
{
    ProcessData_Lock();

    g_process.guide_enable = guide;
    g_process.winder_enable = winder;
    g_process.alarm_active = alarm;

    ProcessData_Unlock();
}
