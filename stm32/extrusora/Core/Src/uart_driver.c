/*
 * uart_driver.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

 /* ========================= */
 /* Librerías                 */
 /* ========================= */
#include "uart_driver.h"
#include "main.h"
#include <string.h>
#include <stdio.h>


/* ========================================================= */
/* Configuración interna                                     */
/* ========================================================= */

/* Número máximo de UARTs gestionadas por este driver */
#define UART_DRIVER_MAX_INSTANCES     4


/* Lista global de instancias registradas
- Sirve para que los callbacks HAL sepan qué UartDriver_t corresponde a cada UART_HandleTypeDef.*/
static UartDriver_t *g_instances[UART_DRIVER_MAX_INSTANCES];


/* Registro de instancia. Sirve para registrar una instancia de UartDriver_t. */
static bool UartDriver_RegisterInstance(UartDriver_t *drv)
{
    for (uint32_t i = 0; i < UART_DRIVER_MAX_INSTANCES; i++)
    {
        if (g_instances[i] == NULL)
        {
            g_instances[i] = drv;
            return true;
        }
    }

    return false;
}

/* Busca una instancia por su manejador UART */
static UartDriver_t *UartDriver_FindByHandle(UART_HandleTypeDef *huart)
{
    for (uint32_t i = 0; i < UART_DRIVER_MAX_INSTANCES; i++)
    {
        if ((g_instances[i] != NULL) &&
            (g_instances[i]->huart == huart))
        {
            return g_instances[i];
        }
    }

    return NULL;
}

/* Notifica a la tarea de comunicación */
/* - Activa flags en la tarea de comunicación */
static void UartDriver_NotifyTask(UartDriver_t *drv, uint32_t flags)
{
    if ((drv != NULL) && (drv->notify_task != NULL))
    {
        osThreadFlagsSet(drv->notify_task, flags);
    }
}

/* Calcula la longitud de una cadena de caracteres
    - mientras no se alcance la longitud máxima de la cadena medir*/
static uint16_t UartDriver_Strnlen(const char *s, uint16_t max_len)
{
    uint16_t len = 0;

    while ((len < max_len) && (s[len] != '\0'))
    {
        len++;
    }

    return len;
}

/* Inicia la recepción DMA */
static void UartDriver_StartRxDma(UartDriver_t *drv)
{
    if ((drv == NULL) || (drv->huart == NULL))
    {
        return;
    }

    /* Reinicio de variables internas de recepción.*/

    /* rx_old_pos: posición anterior del DMA circular.
     * rx_line_index: índice de la línea actual.
     * rx_discarding: si se está descartando una línea demasiado larga.
     * restart_rx_needed: si se necesita reiniciar la recepción DMA.
     */
    drv->rx_old_pos = 0;
    drv->rx_line_index = 0;
    drv->rx_discarding = false;
    drv->restart_rx_needed = false;

    /* Arranca recepción DMA con evento IDLE.*/
    HAL_UARTEx_ReceiveToIdle_DMA(
        drv->huart,
        drv->rx_dma_buffer,
        UART_DRIVER_RX_DMA_SIZE
    );

     /* Desactivamos flag de Half Transfer */
     if (drv->huart->hdmarx != NULL)
     {
         __HAL_DMA_DISABLE_IT(drv->huart->hdmarx, DMA_IT_HT);
     }
}

/* Añadir línea a la cola de recepción */
static void UartDriver_PushRxLine(UartDriver_t *drv)
{
    /* Añadir terminador de línea '\0' al final de la línea. */
    drv->rx_line_buffer[drv->rx_line_index] = '\0';

    /* Metemos la línea completa en la cola RX.*/
    osStatus_t status = osMessageQueuePut(
        drv->rx_queue,
        drv->rx_line_buffer,
        0,
        0
    );

    /* Si la cola está llena, descartamos la línea. */
    if (status != osOK)
    {
        drv->rx_queue_drops++;
    }

    drv->rx_line_index = 0;
}


/* Procesar un byte recibido */
static void UartDriver_ProcessRxByte(UartDriver_t *drv, uint8_t byte)
{
    /* Ignorar '\r'. */
    if (byte == '\r')
    {
        return;
    }

    /* Si Fin de línea */
    if (byte == '\n')
    {
        /* Si estamos descartando una línea */
        if (drv->rx_discarding)
        {
            drv->rx_discarding = false;
            drv->rx_line_index = 0;
            return;
        }
        /* Si hay datos en la línea actual */
        if (drv->rx_line_index > 0)
        {
            UartDriver_PushRxLine(drv);
        }

        return;
    }

    /*  Si estamos descartando una línea demasiado larga, ignoramos todo hasta el próximo '\n'. */
    if (drv->rx_discarding)
    {
        return;
    }

    /* Añadir byte normal a la línea si no se ha alcanzado el límite */
    if (drv->rx_line_index < (UART_DRIVER_LINE_SIZE - 1))
    {
        drv->rx_line_buffer[drv->rx_line_index] = (char)byte;
        drv->rx_line_index++;
    }
    /* Si se ha alcanzado el límite, descartamos el byte */
    else
    {
        drv->rx_overflows++;
        drv->rx_discarding = true;
        drv->rx_line_index = 0;
    }
}

/* Arrancar servicio de recepción */
static void UartDriver_ServiceRx(UartDriver_t *drv)
{
    if ((drv == NULL) ||
        (drv->huart == NULL) ||
        (drv->huart->hdmarx == NULL))
    {
        return;
    }

    /*
     * DMA RX circular:
     *
     * El DMA escribe continuamente en rx_dma_buffer.
     *
     * NDTR indica cuántos bytes quedan por rellenar.
     *
     * Por tanto:
     *
     *   pos = tamaño_buffer - NDTR
     *
     * pos es la posición actual de escritura del DMA.
     */
    uint16_t pos = UART_DRIVER_RX_DMA_SIZE -
                   (uint16_t)__HAL_DMA_GET_COUNTER(drv->huart->hdmarx);

    /*
     * No hay datos nuevos.
     */
    if (pos == drv->rx_old_pos)
    {
        return;
    }

    /*
     * Caso normal:
     * El DMA ha avanzado sin dar la vuelta al buffer.
     */
    if (pos > drv->rx_old_pos)
    {
        for (uint16_t i = drv->rx_old_pos; i < pos; i++)
        {
            UartDriver_ProcessRxByte(drv, drv->rx_dma_buffer[i]);
        }
    }
    else
    {
        /*
         * Caso circular:
         * El DMA llegó al final del buffer y volvió al inicio.
         */

        for (uint16_t i = drv->rx_old_pos; i < UART_DRIVER_RX_DMA_SIZE; i++)
        {
            UartDriver_ProcessRxByte(drv, drv->rx_dma_buffer[i]);
        }

        for (uint16_t i = 0; i < pos; i++)
        {
            UartDriver_ProcessRxByte(drv, drv->rx_dma_buffer[i]);
        }
    }

    /*
     * Guardamos la nueva posición ya procesada.
     */
    drv->rx_old_pos = pos;
}

/* Arrancar servicio de transmisión */
static void UartDriver_ServiceTx(UartDriver_t *drv)
{
    if ((drv == NULL) || (drv->huart == NULL))
    {
        return;
    }

    /* Si ya hay una transmisión DMA en marcha, no lanzamos otra */
    if (drv->tx_busy)
    {
        return;
    }

    char next_line[UART_DRIVER_TX_LINE_SIZE];

    /* Sacamos una línea pendiente de la cola TX */
    osStatus_t status = osMessageQueueGet(
        drv->tx_queue,
        next_line,
        NULL,
        0
    );

    /* Si no hay líneas pendientes, salimos */
    if (status != osOK)
    {
        return;
    }

    /* Calculamos la longitud de la línea */
    uint16_t len = UartDriver_Strnlen(
        next_line,
        UART_DRIVER_TX_LINE_SIZE
    );

    /* Si la línea está vacía, salimos */
    if (len == 0)
    {
        return;
    }

    /* Copiar la línea al buffer usado por DMA.*/
    memcpy(drv->tx_dma_buffer, next_line, len);
    /* Marcar que hay una transmisión en curso */
    drv->tx_busy = true;

    /* Transmitir por DMA */
    HAL_StatusTypeDef hal_status = HAL_UART_Transmit_DMA(
        drv->huart,
        drv->tx_dma_buffer,
        len
    );

    /* Si no se pudo lanzar la transmisión, liberamos TX. */
    if (hal_status != HAL_OK)
    {
        drv->tx_busy = false;
        drv->tx_errors++;
    }
}


/* ========================================================= */
/* Funciones públicas                                        */
/* ========================================================= */

/* Inicializar el driver UART */
bool UartDriver_Init(
    UartDriver_t *drv,
    UART_HandleTypeDef *huart,
    const char *name
)
{
    if ((drv == NULL) || (huart == NULL))
    {
        return false;
    }

    /* Limpiamos toda la estructura */
    memset(drv, 0, sizeof(UartDriver_t));

    drv->huart = huart;
    drv->name = name;

    /* Guardamos la tarea actual */
    drv->notify_task = osThreadGetId();

    drv->tx_busy = false;
    drv->restart_rx_needed = false;

    /* Cola RX: almacena líneas ya completas recibidas por UART */
    drv->rx_queue = osMessageQueueNew(
        UART_DRIVER_RX_QUEUE_LEN,
        UART_DRIVER_LINE_SIZE,
        NULL
    );

    /* Si no se pudo crear la cola RX, devolvemos error */
    if (drv->rx_queue == NULL)
    {
        return false;
    }

    /* Cola TX: almacena líneas pendientes de transmitir. */
    drv->tx_queue = osMessageQueueNew(
        UART_DRIVER_TX_QUEUE_LEN,
        UART_DRIVER_TX_LINE_SIZE,
        NULL
    );

    /* Si no se pudo crear la cola TX, devolvemos error */
    if (drv->tx_queue == NULL)
    {
        return false;
    }

    /* Registramos la instancia para que los callbacks puedan encontrarla */
    if (!UartDriver_RegisterInstance(drv))
    {
        return false;
    }

    /* Arrancamos RX DMA circular */
    UartDriver_StartRxDma(drv);

    return true;
}

/* Servicio del driver UART 
    - Ejecución periódica del driver*/
void UartDriver_Service(UartDriver_t *drv)
{
    if (drv == NULL)
    {
        return;
    }

    /* Si se necesita reiniciar RX */
    if (drv->restart_rx_needed)
    {
        /* Abortamos la recepción actual y reiniciamos RX DMA */
        HAL_UART_AbortReceive(drv->huart);
        UartDriver_StartRxDma(drv);
    }

    /* Lanzar servicio RX */
    UartDriver_ServiceRx(drv);

    /* Lanzar servicio TX */
    UartDriver_ServiceTx(drv);
}

/* Leer una línea de la cola RX */
bool UartDriver_ReadLine(
    UartDriver_t *drv,
    char *out_line,
    uint32_t out_size,
    uint32_t timeout_ticks
)
{
    if ((drv == NULL) || (out_line == NULL) || (out_size == 0))
    {
        return false;
    }

    char local_line[UART_DRIVER_LINE_SIZE];

    osStatus_t status = osMessageQueueGet(
        drv->rx_queue,
        local_line,
        NULL,
        timeout_ticks
    );

    if (status != osOK)
    {
        return false;
    }

    /* Copiamos la línea leída a la salida, asegurándonos de no desbordar el buffer de salida */
    strncpy(out_line, local_line, out_size - 1);
    out_line[out_size - 1] = '\0';

    return true;
}

/* Enviar una línea por UART a través de cola */
bool UartDriver_SendLine(
    UartDriver_t *drv,
    const char *line,
    uint32_t timeout_ticks
)
{
    if ((drv == NULL) || (line == NULL))
    {
        return false;
    }

    char tx_line[UART_DRIVER_TX_LINE_SIZE];

    /* Añadir terminador de línea */
    int len = snprintf(
        tx_line,
        sizeof(tx_line),
        "%s\r\n",
        line
    );

    /* Si la línea es demasiado larga, devolvemos error */
    if ((len <= 0) || (len >= (int)sizeof(tx_line)))
    {
        return false;
    }

    /* Metemos la línea en la cola TX */
    osStatus_t status = osMessageQueuePut(
        drv->tx_queue,
        tx_line,
        0,
        timeout_ticks
    );

    /* Si la cola está llena, devolvemos error */
    if (status != osOK)
    {
        drv->tx_queue_drops++;
        return false;
    }

    /* Despertamos la CommTask para que llame a UartDriver_Service() y arranque la transmisión DMA.*/
    UartDriver_NotifyTask(drv, UART_DRIVER_FLAG_TX);

    return true;
}


/* ========================================================= */
/* Callbacks HAL                                             */
/* ========================================================= */
/*
 * CubeMX genera las interrupciones:
 *
 *   USARTx_IRQHandler()
 *   DMAx_IRQHandler()
 *
 * Esas interrupciones llaman al HAL.
 * Después el HAL llama a estos callbacks.
 */


/* Callback de recepción con ReceiveToIdle DMA */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    (void)Size;
    /* Buscar el controlador UART correspondiente */
    UartDriver_t *drv = UartDriver_FindByHandle(huart);

    if (drv == NULL)
    {
        return;
    }

    /* Notificar a la CommTask */
    UartDriver_NotifyTask(drv, UART_DRIVER_FLAG_RX);
}


/* Callback de transmisión completada */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Buscar el controlador UART correspondiente */
    UartDriver_t *drv = UartDriver_FindByHandle(huart);

    if (drv == NULL)
    {
        return;
    }

    /* Liberamos TX para que UartDriver_ServiceTx() pueda enviar la siguiente línea de la cola. */
    drv->tx_busy = false;
    /* Notificar a la CommTask */
    UartDriver_NotifyTask(drv, UART_DRIVER_FLAG_TX);
}


/* Callback de error UART.*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    /* Buscar el controlador UART correspondiente */
    UartDriver_t *drv = UartDriver_FindByHandle(huart);

    if (drv == NULL)
    {
        return;
    }

    /* Liberamos TX y aumentamos el contador de errores */
    drv->tx_busy = false;
    drv->tx_errors++;

    /* Marcar reinicio de recepción necesario para reiniciar el DMA circular */
    drv->restart_rx_needed = true;
    /* Notificar a la CommTask */
    UartDriver_NotifyTask(drv, UART_DRIVER_FLAG_ERR);
}
