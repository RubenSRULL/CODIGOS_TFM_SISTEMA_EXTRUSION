/*
 * protocol_hmi.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

/* ========================================= */
/* Librerías                                 */
/* ========================================= */
#include "protocol_hmi.h"
#include "protocol_winder.h"
#include "process_data.h"
#include "wrapper_fsm.h"
#include "safety.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/* Interfaz UART asociada al HMI / Raspberry */
static UartDriver_t *g_uart_hmi = NULL;


/* ========================================================= */
/* Funciones internas                                        */
/* ========================================================= */

/* Envío de línea hacia el HMI */
static void send_line(const char *line)
{
    /* Si la interfaz UART y la línea no son nulas */
    if ((g_uart_hmi != NULL) && (line != NULL))
    {
        /* Envía la línea a través de la interfaz UART */
        UartDriver_SendLine(g_uart_hmi, line, 0);
    }
}

/* Envío de respuesta "OK" */
static void send_ok(void)
{
    send_line("OK");
}

/* Envío de respuesta "ERR" */
static void send_err(void)
{
    send_line("ERR");
}

/* Verifica si una cadena comienza con un prefijo */
static bool starts_with(const char *text, const char *prefix)
{
    if ((text == NULL) || (prefix == NULL))
    {
        return false;
    }

    return strncmp(text, prefix, strlen(prefix)) == 0;
}

/* Analiza un valor de punto flotante después de un prefijo
    - Si la línea comienza con el prefijo, intenta convertir el resto a float
    - Devuelve true si la conversión es exitosa, false en caso contrario */
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


/* ========================================================= */
/* API pública                                               */
/* ========================================================= */

/* Inicialización del protocolo HMI */
void ProtocolHMI_Init(UartDriver_t *uart_hmi)
{
    g_uart_hmi = uart_hmi;
}


/* Envío de línea hacia el HMI */
void ProtocolHMI_SendRawLine(const char *line)
{
    send_line(line);
}

/* Envío de estado */
void ProtocolHMI_SendState(void)
{
    ProcessData_t data;
    /* Buffer para el mensaje */
    char msg[160];

    /* Obtiene una instantánea de los datos actuales del sistema */
    ProcessData_GetSnapshot(&data);

    /* Formatea el mensaje de estado */
    snprintf(
        msg,
        sizeof(msg),
        "OK:EXTRUDER_STATE=%u;EXTRUDER_ALARM=%lu;WINDER_STATE=%u;WINDER_ALARM=%lu",
        data.extruder_state,
        (unsigned long)data.alarm_flags,
        data.winder_state,
        (unsigned long)data.winder_alarm_flags
    );

    /* Envía el mensaje de estado */
    send_line(msg);
}

/* Envío de telemetría */
void ProtocolHMI_SendTelemetry(void)
{
    ProcessData_t data;
    /* Buffer para el mensaje */
    char msg[256];

    /* Obtiene una instantánea de los datos actuales del sistema */
    ProcessData_GetSnapshot(&data);

    /* Formatea el mensaje de telemetría */
    snprintf(
        msg,
        sizeof(msg),
        "TEL:EXTRUDER_STATE=%u;"
        "TEMP=%.1f;"
        "TARGET=%.1f;"
        "HEATER=%.1f;"
        "RPM_INT=%.1f;"
        "RPM_EXT=%.1f;"
        "DIAM=%.3f;"
        "TARGET_DIAM=%.3f;"
        "WINDER_STATE=%u;"
        "WINDER_ALARM=%lu;"
        "EXTRUDER_ALARM=%lu",
        data.extruder_state,
        data.temp_c,
        data.target_temp_c,
        data.heater_percent,
        data.rpm_int_setpoint,
        data.rpm_ext_setpoint,
        data.diameter_mm,
        data.target_diameter_mm,
        data.winder_state,
        (unsigned long)data.winder_alarm_flags,
        (unsigned long)data.alarm_flags
    );

    /* Envía el mensaje de telemetría */
    send_line(msg);
}

/* Procesamiento de línea recibida desde el HMI */
void ProtocolHMI_ProcessLine(const char *line)
{
    float value;

    if (line == NULL)
    {
        return;
    }

    /* Si el comando es PING */
    if (strcmp(line, "PING") == 0)
    {
        send_line("PONG");
        return;
    }

    /* Si es un comando es STATE? */
    if (strcmp(line, "STATE?") == 0)
    {
        ProtocolHMI_SendState();
        return;
    }

    /* Si es un comando es TEL? */
    if (strcmp(line, "TEL?") == 0)
    {
        ProtocolHMI_SendTelemetry();
        return;
    }

    /* Si es un comando es AUTO */
    if (strcmp(line, "AUTO") == 0)
    {
        ExtruderFSM_CommandAuto();
        ProcessData_SetRpmExtSetpoint(0.0f);
        ProtocolWinder_SendHome();
        send_ok();
        return;
    }

    /* Si es un comando es MANUAL */
    if (strcmp(line, "MANUAL") == 0)
    {
        ExtruderFSM_CommandManual();
        ProcessData_SetRpmExtSetpoint(0.0f);
        ProtocolWinder_SendHome();
        send_ok();
        return;
    }

    /* Si es un comando es PREHEAT */
    if (strcmp(line, "PREHEAT") == 0)
    {
        ExtruderFSM_CommandPreheat();
        send_ok();
        return;
    }

    /* Si es un comando es START */
    if (strcmp(line, "START") == 0)
    {
        ProtocolWinder_SendRun();
        ExtruderFSM_CommandStart();
        send_ok();
        return;
    }

    /* Si es un comando es STOP */
    if (strcmp(line, "STOP") == 0)
    {
        ExtruderFSM_CommandStop();

        ProcessData_SetTargetTemp(0.0f);
        ProcessData_SetHeaterPercent(0.0f);
        ProcessData_SetRpmIntSetpoint(0.0f);
        ProcessData_SetRpmExtSetpoint(0.0f);

        ProtocolWinder_SendStop();
        send_ok();
        return;
    }

    /* Si es un comando es RESET */
    if (strcmp(line, "RESET") == 0)
    {
        Safety_ClearAll();
        ExtruderFSM_CommandReset();
        ProtocolWinder_SendReset();
        send_ok();
        return;
    }

    /* Si es un comando es RESET_AUTO */
    if (strcmp(line, "RESET_AUTO") == 0)
    {
        ExtruderFSM_CommandStop();
        send_ok();
        return;
    }

    /* Si es un comando es SET_TEMP */
    if (parse_float_after_prefix(line, "SET_TEMP:", &value))
    {
        ProcessData_SetTargetTemp(value);
        send_ok();
        return;
    }

    /* Si es un comando es HEATER_OFF */
    if (strcmp(line, "HEATER_OFF") == 0)
    {
        ProcessData_SetTargetTemp(0.0f);
        ProcessData_SetHeaterPercent(0.0f);
        send_ok();
        return;
    }

    /* Si es un comando es DIAM */
    if (parse_float_after_prefix(line, "DIAM:", &value))
    {
        ProcessData_SetDiameter(value);
        send_ok();
        return;
    }

    /* Si es un comando es SET_DIAM_TARGET */
    if (parse_float_after_prefix(line, "SET_DIAM_TARGET:", &value))
    {
        ProcessData_SetTargetDiameter(value);
        send_ok();
        return;
    }

    /* Si es un comando es SET_INT_RPM */
    if (parse_float_after_prefix(line, "SET_INT_RPM:", &value))
    {
        ProcessData_SetRpmIntSetpoint(value);
        send_ok();
        return;
    }

    /* Si es un comando es STOP_INT */
    if (strcmp(line, "STOP_INT") == 0)
    {
        ProcessData_SetRpmIntSetpoint(0.0f);
        send_ok();
        return;
    }

    /* Si es un comando es SET_EXT_RPM */
    if (parse_float_after_prefix(line, "SET_EXT_RPM:", &value))
    {
        ProcessData_SetRpmExtSetpoint(value);
        ProtocolWinder_SendSetRpm(value);
        send_ok();
        return;
    }

    /* Si es un comando es STOP_EXT */
    if (strcmp(line, "STOP_EXT") == 0)
    {
        ProcessData_SetRpmExtSetpoint(0.0f);
        ProtocolWinder_SendSetRpm(0.0f);
        send_ok();
        return;
    }

    /* Si es un comando es FAULT */
    if (strcmp(line, "FAULT") == 0)
    {
        Safety_SetFault(ALARM_EMERGENCY_STOP);
        send_ok();
        return;
    }

    /* Si es un comando es CLEAR_FAULT */
    if (strcmp(line, "CLEAR_FAULT") == 0)
    {
        Safety_ClearAll();
        ExtruderFSM_CommandReset();
        ProtocolWinder_SendReset();
        send_ok();
        return;
    }

    /* Si es un comando es PING_WINDER */
    if (strcmp(line, "PING_WINDER") == 0)
    {
        ProtocolWinder_SendPing();
        send_ok();
        return;
    }

    /* Si es un comando es WINDER_HOME */
    if (strcmp(line, "WINDER_HOME") == 0)
    {
        ProcessData_SetRpmExtSetpoint(0.0f);
        ProtocolWinder_SendHome();
        send_ok();
        return;
    }

    /* Si es un comando es WINDER_RUN */
    if (strcmp(line, "WINDER_RUN") == 0)
    {
        ProtocolWinder_SendRun();
        send_ok();
        return;
    }

    /* Si es un comando es WINDER_STOP */
    if (strcmp(line, "WINDER_STOP") == 0)
    {
        ProtocolWinder_SendStop();
        send_ok();
        return;
    }

    /* Si es un comando es WINDER_RESET */
    if (strcmp(line, "WINDER_RESET") == 0)
    {
        ProtocolWinder_SendReset();
        send_ok();
        return;
    }

    /* Si es un comando es WINDER_STATE? */
    if (strcmp(line, "WINDER_STATE?") == 0)
    {
        ProtocolWinder_SendStateRequest();
        send_ok();
        return;
    }

    /* Si es un comando es WINDER_RPM: */
    if (parse_float_after_prefix(line, "WINDER_RPM:", &value))
    {
        ProcessData_SetRpmExtSetpoint(value);
        ProtocolWinder_SendSetRpm(value);
        send_ok();
        return;
    }

    /* Si el comando no coincide con ninguno de los anteriores, envía ERR */
    send_err();
}
