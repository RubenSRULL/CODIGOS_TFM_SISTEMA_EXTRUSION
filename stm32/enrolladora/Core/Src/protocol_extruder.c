/*
 * protocol_extruder.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

// ============================= //
// Librerias
// ============================= //
#include "protocol_extruder.h"
#include "process_data.h"
#include "wrapper_fsm.h"
#include "safety.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ========================================================= //
// Variables privadas del módulo UART driver
// ========================================================= //
static UartDriver_t *g_uart_extruder = NULL;


// ========================================================= //
// Funciones privadas del módulo
// ========================================================= //

// -- Envia una línea de texto a través del UART del extrusor
static void send_line(const char *line)
{
    if ((g_uart_extruder != NULL) && (line != NULL))
    {
        UartDriver_SendLine(g_uart_extruder, line, 0);
    }
}

// -- Comprueba si un texto comienza con un prefijo dado
static bool starts_with(const char *text, const char *prefix)
{
    if ((text == NULL) || (prefix == NULL))
    {
        return false;
    }

    return strncmp(text, prefix, strlen(prefix)) == 0;
}

// -- Analiza un valor flotante después de un prefijo en una línea de texto
static bool parse_float_after_prefix(
    const char *line,
    const char *prefix,
    float *out_value
)
{
    char *endptr;
    const char *value_str;

    if ((line == NULL) || (prefix == NULL) || (out_value == NULL))
    {
        return false;
    }

    if (!starts_with(line, prefix))
    {
        return false;
    }

    value_str = line + strlen(prefix);

    if (*value_str == '\0')
    {
        return false;
    }

    *out_value = strtof(value_str, &endptr);

    if (endptr == value_str)
    {
        return false;
    }

    return true;
}

// ========================================================= //
// Funciones públicas del módulo
// ========================================================= //

// -- Inicializa el módulo de protocolo de bobinadora a extrusora con el UART correspondiente
void ProtocolExtruder_Init(UartDriver_t *uart_extruder)
{
    g_uart_extruder = uart_extruder;
}

// -- Envía el estado actual de bobinadora a extrusor a través del UART
void ProtocolExtruder_SendStatus(void)
{
    ProcessData_t data;
    char msg[128];

    ProcessData_GetSnapshot(&data);

    snprintf(
        msg,
        sizeof(msg),
        "WINDER:STATE=%u;RPM=%.1f;ALARM=%lu",
        data.winder_state,
        data.rpm_setpoint,
        (unsigned long)data.alarm_flags
    );

    send_line(msg);
}

// -- Procesa una línea de comando recibida a través del UART del bobinador y ejecuta la acción correspondiente
void ProtocolExtruder_ProcessLine(const char *line)
{
    float value;

    if (line == NULL)
    {
        return;
    }

    if (strcmp(line, "PING") == 0)
    {
        send_line("WINDER:PONG");
        return;
    }

    if (strcmp(line, "STATE?") == 0)
    {
        ProtocolExtruder_SendStatus();
        return;
    }

    if (strcmp(line, "HOME") == 0)
    {
        ProcessData_SetRpmSetpoint(0.0f);
        WinderFSM_CommandHome();
        send_line("OK");();
        return;
    }

    if (strcmp(line, "RUN") == 0)
    {
        WinderFSM_CommandRun();
        send_line("OK");();
        return;
    }

    if (strcmp(line, "STOP") == 0)
    {
        ProcessData_SetRpmSetpoint(0.0f);
        WinderFSM_CommandStop();
        send_line("OK");();
        return;
    }

    if (strcmp(line, "RESET") == 0)
    {
        Safety_ClearAll();
        WinderFSM_CommandReset();
        send_line("OK");();
        return;
    }

    if (parse_float_after_prefix(line, "SET_RPM:", &value))
    {
        ProcessData_SetRpmSetpoint(value);
        send_line("OK");();
        return;
    }

    if (strcmp(line, "FAULT") == 0)
    {
        Safety_SetFault(WINDER_ALARM_EMERGENCY_STOP);
        send_line("OK");();
        return;
    }

    if (strcmp(line, "CLEAR_FAULT") == 0)
    {
        Safety_ClearAll();
        WinderFSM_CommandReset();
        send_line("OK");();;
        return;
    }

    send_line("ERR");();
}
