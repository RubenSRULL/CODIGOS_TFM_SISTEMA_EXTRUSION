/*
 * protocol_winder.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */


/*
 * protocol_winder.c
 *
 * Comunicación Extrusora <-> Enrolladora
 */

 // ================================ //
 // Librerias
 // ================================ //
#include "protocol_winder.h"
#include "protocol_hmi.h"
#include "process_data.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ========================================================= */
// Variables globales
// ========================================================= */
static UartDriver_t *g_uart_winder = NULL;


/* ========================================================= */
/* Funciones internas                                        */
/* ========================================================= */

// -- Función interna para enviar una línea de texto a la enrolladora
static bool send_line(const char *line)
{
    if ((g_uart_winder != NULL) && (line != NULL))
    {
        return UartDriver_SendLine(g_uart_winder, line, 0);
    }

    return false;
}

// -- Función interna para verificar si un texto comienza con un prefijo
static bool starts_with(const char *text, const char *prefix)
{
    if ((text == NULL) || (prefix == NULL))
    {
        return false;
    }

    return strncmp(text, prefix, strlen(prefix)) == 0;
}

// -- Función interna para analizar un campo de tipo uint32_t en una línea de texto
static bool parse_uint_field(
    // Descripción: Analiza un campo de tipo uint32_t en una línea de texto, es decir, busca un campo con un nombre específico y extrae su valor numérico.
    const char *line,
    const char *key,
    uint32_t *out_value
)
{
    const char *pos;
    char *endptr;

    if ((line == NULL) || (key == NULL) || (out_value == NULL))
    {
        return false;
    }

    // Busca la posición del campo en la línea
    pos = strstr(line, key);

    if (pos == NULL)
    {
        return false;
    }

    // Avanza la posición después del nombre del campo
    pos += strlen(key);

    // Convierte el valor a uint32_t usando strtoul
    *out_value = strtoul(pos, &endptr, 10);

    if (endptr == pos)
    {
        return false;
    }

    return true;
}

// -- Función interna para analizar un campo de tipo float en una línea de texto
static bool parse_float_field(
    // Descripción: Analiza un campo de tipo float en una línea de texto, es decir, busca un campo con un nombre específico y extrae su valor numérico.
    const char *line,
    const char *key,
    float *out_value
)
{
    const char *pos;
    char *endptr;

    if ((line == NULL) || (key == NULL) || (out_value == NULL))
    {
        return false;
    }

    pos = strstr(line, key);

    if (pos == NULL)
    {
        return false;
    }

    pos += strlen(key);

    *out_value = strtof(pos, &endptr);

    if (endptr == pos)
    {
        return false;
    }

    return true;
}

// -- Función interna para procesar el estado de la enrolladora recibido en una línea de texto
static void process_winder_status(const char *line)
{
    uint32_t state;
    uint32_t alarm;
    float rpm;

    // Analiza los campos de la línea y actualiza el estado de la enrolladora
    if (parse_uint_field(line, "STATE=", &state))
    {
        ProcessData_SetWinderState((uint8_t)state);
    }

    // Analiza los campos de la línea y actualiza las alarmas de la enrolladora
    if (parse_uint_field(line, "ALARM=", &alarm))
    {
        ProcessData_SetWinderAlarmFlags(alarm);
    }

    // Analiza los campos de la línea y actualiza el setpoint de RPM de la enrolladora
    if (parse_float_field(line, "RPM=", &rpm))
    {
        ProcessData_SetRpmExtSetpoint(rpm);
    }
}


void ProtocolWinder_Init(UartDriver_t *uart_winder)
{
    g_uart_winder = uart_winder;
}


void ProtocolWinder_SendPing(void)
{
    bool ok;

    ProtocolHMI_SendRawLine("DBG:TX_WINDER:PING");

    ok = send_line("PING");

    if (!ok)
    {
        ProtocolHMI_SendRawLine("DBG:TX_WINDER_FAIL");
    }
}


void ProtocolWinder_SendStateRequest(void)
{
    bool ok;

    ProtocolHMI_SendRawLine("DBG:TX_WINDER:STATE?");

    ok = send_line("STATE?");

    if (!ok)
    {
        ProtocolHMI_SendRawLine("DBG:TX_WINDER_FAIL");
    }
}


void ProtocolWinder_SendHome(void)
{
    bool ok;

    ProtocolHMI_SendRawLine("DBG:TX_WINDER:HOME");

    ok = send_line("HOME");

    if (!ok)
    {
        ProtocolHMI_SendRawLine("DBG:TX_WINDER_FAIL");
    }
}


void ProtocolWinder_SendRun(void)
{
    bool ok;

    ProtocolHMI_SendRawLine("DBG:TX_WINDER:RUN");

    ok = send_line("RUN");

    if (!ok)
    {
        ProtocolHMI_SendRawLine("DBG:TX_WINDER_FAIL");
    }
}


void ProtocolWinder_SendStop(void)
{
    bool ok;

    ProtocolHMI_SendRawLine("DBG:TX_WINDER:STOP");

    ok = send_line("STOP");

    if (!ok)
    {
        ProtocolHMI_SendRawLine("DBG:TX_WINDER_FAIL");
    }
}


void ProtocolWinder_SendReset(void)
{
    bool ok;

    ProtocolHMI_SendRawLine("DBG:TX_WINDER:RESET");

    ok = send_line("RESET");

    if (!ok)
    {
        ProtocolHMI_SendRawLine("DBG:TX_WINDER_FAIL");
    }
}


void ProtocolWinder_SendSetRpm(float rpm)
{
    char msg[64];
    bool ok;

    if (rpm < 0.0f)
    {
        rpm = 0.0f;
    }

    snprintf(
        msg,
        sizeof(msg),
        "SET_RPM:%.1f",
        rpm
    );

    ProtocolHMI_SendRawLine("DBG:TX_WINDER:SET_RPM");

    ok = send_line(msg);

    if (!ok)
    {
        ProtocolHMI_SendRawLine("DBG:TX_WINDER_FAIL");
    }
}


void ProtocolWinder_ProcessLine(const char *line)
{
    if (line == NULL)
    {
        return;
    }

    /*
     * Todo lo recibido desde la enrolladora se reenvía a RPi5,
     * para que puedas ver si la comunicación funciona.
     */

    if (strcmp(line, "WINDER:PONG") == 0)
    {
        ProtocolHMI_SendRawLine("DBG:RX_WINDER:WINDER:PONG");
        ProtocolHMI_SendRawLine(line);
        return;
    }

    if (strcmp(line, "OK") == 0)
    {
        ProtocolHMI_SendRawLine("DBG:RX_WINDER:OK");
        ProtocolHMI_SendRawLine("WINDER_ACK:OK");
        return;
    }

    if (strcmp(line, "ERR") == 0)
    {
        ProtocolHMI_SendRawLine("DBG:RX_WINDER:ERR");
        ProtocolHMI_SendRawLine("WINDER_ACK:ERR");
        return;
    }

    if (starts_with(line, "WINDER:"))
    {
        process_winder_status(line);
        return;
    }

    /*
     * Si llega algo raro desde la enrolladora, también lo mostramos.
     */
    ProtocolHMI_SendRawLine("DBG:RX_WINDER:UNKNOWN");
    ProtocolHMI_SendRawLine(line);
}

void ProtocolWinder_PollState(void)
{
    send_line("STATE?");
}
