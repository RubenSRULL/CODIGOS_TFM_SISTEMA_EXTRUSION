/*
 * protocol_extruder.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_PROTOCOL_EXTRUDER_H_
#define INC_PROTOCOL_EXTRUDER_H_

// =========================== //
// Librerias
// =========================== //
#include <stdint.h>
#include <stdbool.h>
#include "uart_driver.h"

// =========================== //
// Prototipos de funciones
// =========================== //
void ProtocolExtruder_Init(UartDriver_t *uart_extruder);
void ProtocolExtruder_ProcessLine(const char *line);
void ProtocolExtruder_SendStatus(void);

#endif /* INC_PROTOCOL_EXTRUDER_H_ */
