/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/
/** @file info_interactive.c
 *  @version 1.0
 *  @date Oct 2017
 *
 *  @brief get hardware peripheral information
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#include "info_interactive.h"
#include "judgement_info.h"
#include "protocol.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "comm_task.h"

UART_HandleTypeDef huart4;
/**
  * @brief  Uart send data in non blocking mode
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  * @param  p_data: Pointer to data buffer
  * @param  size: Amount of data to be sent
  */
//void write_uart_noblocking(UART_HandleTypeDef *huart, uint8_t *p_data, uint16_t size)
//{
//  HAL_UART_Transmit_DMA(huart, p_data, size);
//}

//void uart_write_completed_signal(UART_HandleTypeDef *huart)
//{
//  if (huart == &COMPUTER_HUART)
//  {
//    //osSignalSet(unpack_task_t, PC_UART_WRITE_SIGNAL);
//  }
//}

/**
  * @brief  Uart send data in blocking mode, 1000ms is the timeout value.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  * @param  p_data: Pointer to data buffer
  * @param  size: Amount of data to be sent
  */
//void write_uart_blocking(UART_HandleTypeDef *huart, uint8_t *p_data, uint16_t size)
//{
//  HAL_UART_Transmit(huart, p_data, size, 100);
//}

/**
  * @brief  Receives an amount of data in non blocking mode. 
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  p_data: Pointer to data buffer
  * @param  size: Amount of data to be received
  */
//void read_uart_noblocking(UART_HandleTypeDef *huart, uint8_t *p_data, uint16_t size)
//{
//  HAL_UART_Receive_DMA(huart, p_data, size);
//}

//void uart_read_completed_signal(UART_HandleTypeDef *huart)
//{
//  if (huart == &COMPUTER_HUART)
//  {
//    //osSignalSet(unpack_task_t, PC_UART_READ_SIGNAL);
//  }
//}

/**
  * @brief  Receives an amount of data in blocking mode. 
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  p_data: Pointer to data buffer
  * @param  size: Amount of data to be received
  */
extern TaskHandle_t judge_unpack_task_t;
//extern TaskHandle_t pc_unpack_task_t;
//void read_uart_blocking(UART_HandleTypeDef *huart, uint8_t *p_data, uint16_t size)
//{
//  HAL_UART_Receive(huart, p_data, size, 1000);
//}

//void uart_dma_full_signal(UART_HandleTypeDef *huart)
//{
//  if (huart == &JUDGE_HUART)
//  {
//    osSignalSet(judge_unpack_task_t, JUDGE_DMA_FULL_SIGNAL);
//  }
////	else if (huart == &COMPUTER_HUART)
////	{
////		osSignalSet(pc_unpack_task_t, PC_DMA_FULL_SIGNAL);
////	}
//}

void get_dma_memory_msg(DMA_Stream_TypeDef *dma_stream, uint8_t *mem_id, uint16_t *remain_cnt)
{
  *mem_id     = dma_current_memory_target(dma_stream);
  *remain_cnt = dma_current_data_counter(dma_stream);
}

/**
  * @brief  Returns the current memory target used by double buffer transfer.
  * @param  dma_stream: where y can be 1 or 2 to select the DMA and x can be 0
  *          to 7 to select the DMA Stream.
  * @retval The memory target number: 0 for Memory0 or 1 for Memory1. 
  */
uint8_t dma_current_memory_target(DMA_Stream_TypeDef *dma_stream)
{
  uint8_t tmp = 0;

  /* Get the current memory target */
  if ((dma_stream->CR & DMA_SxCR_CT) != 0)
  {
    /* Current memory buffer used is Memory 1 */
    tmp = 1;
  }
  else
  {
    /* Current memory buffer used is Memory 0 */
    tmp = 0;
  }
  return tmp;
}

/**
  * @brief  Returns the number of remaining data units in the current DMAy Streamx transfer.
  * @param  dma_stream: where y can be 1 or 2 to select the DMA and x can be 0
  *          to 7 to select the DMA Stream.
  * @retval The number of remaining data units in the current DMAy Streamx transfer.
  */
uint16_t dma_current_data_counter(DMA_Stream_TypeDef *dma_stream)
{
  /* Return the number of remaining data units for DMAy Streamx */
  return ((uint16_t)(dma_stream->NDTR));
}
