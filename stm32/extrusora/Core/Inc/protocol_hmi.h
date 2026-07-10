/*
 * protocol_hmi.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_PROTOCOL_HMI_H_
#define INC_PROTOCOL_HMI_H_

/* ========================================= */
/* Librerías                                 */
/* ========================================= */
#include <stdint.h>
#include <stdbool.h>
#include "uart_driver.h"

/* Inicialización del protocolo HMI */
void ProtocolHMI_Init(UartDriver_t *uart_hmi);
/* Envío de línea hacia el HMI */
void ProtocolHMI_SendRawLine(const char *line);
/* Procesamiento de línea recibida desde el HMI */
void ProtocolHMI_ProcessLine(const char *line);
/* Envío de datos de telemetría */
void ProtocolHMI_SendTelemetry(void);
/* Envío de estado */
void ProtocolHMI_SendState(void);
#endif /* INC_PROTOCOL_HMI_H_ */