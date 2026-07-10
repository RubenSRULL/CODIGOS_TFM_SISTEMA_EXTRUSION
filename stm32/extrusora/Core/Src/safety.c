/*
 * safety.c
 *
 *  Created on: 21 jun 2026
 *      Author: rsahu
 */

// =========================== //
// Librerias
// =========================== //
#include "safety.h"
#include "process_data.h"

// =========================== //
// Variables globales
// =========================== //
static uint32_t g_alarm_flags = ALARM_NONE;

void Safety_Init(void)
{
    g_alarm_flags = ALARM_NONE;
    ProcessData_SetAlarmFlags(g_alarm_flags);
}

void Safety_Update(void)
{

    ProcessData_SetAlarmFlags(g_alarm_flags);
}

bool Safety_HasFault(void)
{
    return g_alarm_flags != ALARM_NONE;
}

uint32_t Safety_GetAlarmFlags(void)
{
    return g_alarm_flags;
}

void Safety_SetFault(uint32_t flags)
{
    g_alarm_flags |= flags;
    ProcessData_SetAlarmFlags(g_alarm_flags);
}

void Safety_ClearFault(uint32_t flags)
{
    g_alarm_flags &= ~flags;
    ProcessData_SetAlarmFlags(g_alarm_flags);
}

void Safety_ClearAll(void)
{
    g_alarm_flags = ALARM_NONE;
    ProcessData_SetAlarmFlags(g_alarm_flags);
}
