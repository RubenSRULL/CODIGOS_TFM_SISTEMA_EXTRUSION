/*
 * uart_driver.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_UART_DRIVER_H_
#define INC_UART_DRIVER_H_

// ======================== //
// Librerias
// ======================== //
#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os2.h"

// ======================== //
// Instancia de la UART de HAL
// ======================== //
typedef struct __UART_HandleTypeDef UART_HandleTypeDef;


// ============================== */
// Definiciones                   */
// ============================== */
#define UART_DRIVER_RX_DMA_SIZE       256           // Tamaño del buffer circular de recepción DMA.
#define UART_DRIVER_LINE_SIZE         128           // Tamaño máximo de una línea de recepción.
#define UART_DRIVER_TX_LINE_SIZE      132           // Tamaño máximo de una línea de transmisión.
#define UART_DRIVER_RX_QUEUE_LEN      8             // Tamaño de la cola de recepción de líneas.
#define UART_DRIVER_TX_QUEUE_LEN      8             // Tamaño de la cola de transmisión de líneas.

#define UART_DRIVER_FLAG_RX           (1U << 0)     // Flag de evento de recepción.
#define UART_DRIVER_FLAG_TX           (1U << 1)     // Flag de evento de transmisión.
#define UART_DRIVER_FLAG_ERR          (1U << 2)     // Flag de evento de error.


/* ========================================================= */
/* Estructura principal del driver                           */
/* ========================================================= */
typedef struct
{
    // Puntero a la UART de HAL.
    UART_HandleTypeDef *huart;

    // Buffer donde el DMA RX circular escribe continuamente.
    uint8_t rx_dma_buffer[UART_DRIVER_RX_DMA_SIZE];

    // Última posición leída dentro del buffer circular.
    uint16_t rx_old_pos;

    // Buffer temporal para construir una línea recibida.
    char rx_line_buffer[UART_DRIVER_LINE_SIZE];
    uint16_t rx_line_index;

    bool rx_discarding;

    // Cola de líneas recibidas.
    osMessageQueueId_t rx_queue;

    // Cola de líneas pendientes de transmitir.
    osMessageQueueId_t tx_queue;

    // Buffer real usado por el DMA TX.
    uint8_t tx_dma_buffer[UART_DRIVER_TX_LINE_SIZE];

    // Indica si ahora mismo hay una transmisión DMA en marcha.
    volatile bool tx_busy;

    // Tarea a la que se notifican eventos RX/TX/ERR.
    osThreadId_t notify_task;

    const char *name;

    uint32_t rx_overflows;
    uint32_t rx_queue_drops;
    uint32_t tx_queue_drops;
    uint32_t tx_errors;

    volatile bool restart_rx_needed;

} UartDriver_t;


/* ========================================== */
/* Métodos*/
/* ========================================== */

bool UartDriver_Init(
    UartDriver_t *drv,
    UART_HandleTypeDef *huart,
    const char *name
);

// Servicio periódico.
void UartDriver_Service(UartDriver_t *drv);

// Lee una línea recibida desde la cola RX.
bool UartDriver_ReadLine(
    UartDriver_t *drv,
    char *out_line,
    uint32_t out_size,
    uint32_t timeout_ticks
);

// Envía una línea.
bool UartDriver_SendLine(
    UartDriver_t *drv,
    const char *line,
    uint32_t timeout_ticks
);

#endif /* INC_UART_DRIVER_H_ */
