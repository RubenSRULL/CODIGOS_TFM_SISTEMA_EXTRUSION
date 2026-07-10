/*
 * safety.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

// ============================= //
// Librerias
// ============================= //
#include "safety.h"
#include "process_data.h"

// ============================= //
// Variables privadas
// ============================= //
static uint32_t g_alarm_flags = WINDER_ALARM_NONE;

// -- Inicializa el módulo de seguridad --//
void Safety_Init(void)
{
    g_alarm_flags = WINDER_ALARM_NONE;
    ProcessData_SetAlarmFlags(g_alarm_flags);
}

// -- Actualiza el estado de seguridad --//
void Safety_Update(void)
{
    // No completado aun
    ProcessData_SetAlarmFlags(g_alarm_flags);
}

// -- Comprueba si hay alguna falla activa --//
bool Safety_HasFault(void)
{
    return g_alarm_flags != WINDER_ALARM_NONE;
}

// -- Obtiene las banderas de alarma actuales --//
uint32_t Safety_GetAlarmFlags(void)
{
    return g_alarm_flags;
}

// -- Establece una falla específica --//
void Safety_SetFault(uint32_t flags)
{
    g_alarm_flags |= flags;
    ProcessData_SetAlarmFlags(g_alarm_flags);
}

// -- Borra una falla específica --//
void Safety_ClearFault(uint32_t flags)
{
    g_alarm_flags &= ~flags;
    ProcessData_SetAlarmFlags(g_alarm_flags);
}

// -- Borra todas las fallas --//
void Safety_ClearAll(void)
{
    g_alarm_flags = WINDER_ALARM_NONE;
    ProcessData_SetAlarmFlags(g_alarm_flags);
}
