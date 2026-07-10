/*
 * protocol_winder.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_PROTOCOL_WINDER_H_
#define INC_PROTOCOL_WINDER_H_

// ======================= //
// Librerias
// ======================= //
#include <stdint.h>
#include <stdbool.h>
#include "uart_driver.h"

// ======================= //
// Prototipos de funciones
// ======================= //
void ProtocolWinder_Init(UartDriver_t *uart_winder);
void ProtocolWinder_ProcessLine(const char *line);
void ProtocolWinder_SendPing(void);
void ProtocolWinder_SendStateRequest(void);
void ProtocolWinder_SendHome(void);
void ProtocolWinder_SendRun(void);
void ProtocolWinder_SendStop(void);
void ProtocolWinder_SendReset(void);
void ProtocolWinder_SendSetRpm(float rpm);
void ProtocolWinder_PollState(void);

#endif /* INC_PROTOCOL_WINDER_H_ */
