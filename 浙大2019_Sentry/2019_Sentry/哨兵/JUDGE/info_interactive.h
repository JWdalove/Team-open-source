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
/** @file info_interactive.h
 *  @version 1.0
 *  @date Oct 2017
 *
 *  @brief get hardware peripheral information
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#ifndef __INFO_INTER_H__
#define __INFO_INTER_H__

#include "struct.h"
#include "constant.h"
#define JUDGE_HUART        huart4 //connected to judge system
/**
  * @brief  HAL DMA State structures definition
  */
typedef enum
{
    HAL_DMA_STATE_RESET             = 0x00U,  /*!< DMA not yet initialized or disabled */
    HAL_DMA_STATE_READY             = 0x01U,  /*!< DMA initialized and ready for use   */
    HAL_DMA_STATE_BUSY              = 0x02U,  /*!< DMA process is ongoing              */
    HAL_DMA_STATE_TIMEOUT           = 0x03U,  /*!< DMA timeout state                   */
    HAL_DMA_STATE_ERROR             = 0x04U,  /*!< DMA error state                     */
    HAL_DMA_STATE_ABORT             = 0x05U,  /*!< DMA Abort state                     */
} HAL_DMA_StateTypeDef;

/**
  * @brief  DMA handle Structure definition
  */
typedef struct __DMA_HandleTypeDef
{
    DMA_Stream_TypeDef         *Instance;                                                        /*!< Register base address                  */

    DMA_InitTypeDef            Init;                                                             /*!< DMA communication parameters           */



    __IO HAL_DMA_StateTypeDef  State;                                                            /*!< DMA transfer state                     */

    void                       *Parent;                                                          /*!< Parent object state                    */

    void                       (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);         /*!< DMA transfer complete callback         */

    void                       (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);     /*!< DMA Half transfer complete callback    */

    void                       (* XferM1CpltCallback)( struct __DMA_HandleTypeDef * hdma);       /*!< DMA transfer complete Memory1 callback */

    void                       (* XferM1HalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);   /*!< DMA transfer Half complete Memory1 callback */

    void                       (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);        /*!< DMA transfer error callback            */

    void                       (* XferAbortCallback)( struct __DMA_HandleTypeDef * hdma);        /*!< DMA transfer Abort callback            */

    __IO uint32_t              ErrorCode;                                                        /*!< DMA Error code                          */

    uint32_t                   StreamBaseAddress;                                                /*!< DMA Stream Base Address                */

    uint32_t                   StreamIndex;                                                      /*!< DMA Stream Index                       */

} DMA_HandleTypeDef;

/**
  * @brief  UART handle Structure definition
  */

typedef struct
{
    USART_TypeDef                 *Instance;        /*!< UART registers base address        */

    uint8_t                       *pTxBuffPtr;      /*!< Pointer to UART Tx transfer Buffer */

    uint16_t                      TxXferSize;       /*!< UART Tx Transfer size              */

    __IO uint16_t                 TxXferCount;      /*!< UART Tx Transfer Counter           */

    uint8_t                       *pRxBuffPtr;      /*!< Pointer to UART Rx transfer Buffer */

    uint16_t                      RxXferSize;       /*!< UART Rx Transfer size              */

    __IO uint16_t                 RxXferCount;      /*!< UART Rx Transfer Counter           */

//  DMA_HandleTypeDef             *hdmatx;          /*!< UART Tx DMA Handle parameters      */
//
    DMA_HandleTypeDef             *hdmarx;          /*!< UART Rx DMA Handle parameters      */



//  __IO HAL_UART_StateTypeDef    gState;           /*!< UART state information related to global Handle management
//                                                       and also related to Tx operations.
//                                                       This parameter can be a value of @ref HAL_UART_StateTypeDef */
//
//  __IO HAL_UART_StateTypeDef    RxState;          /*!< UART state information related to Rx operations.
//                                                       This parameter can be a value of @ref HAL_UART_StateTypeDef */

    __IO uint32_t                 ErrorCode;        /*!< UART Error code                    */

} UART_HandleTypeDef;






//void write_uart_noblocking(UART_HandleTypeDef *huart, uint8_t *p_data, uint16_t size);
//void write_uart_blocking(UART_HandleTypeDef *huart, uint8_t *p_data, uint16_t size);

//void read_uart_noblocking(UART_HandleTypeDef *huart, uint8_t *p_data, uint16_t size);
//void read_uart_blocking(UART_HandleTypeDef *huart, uint8_t *p_data, uint16_t size);

//void uart_write_completed_signal(UART_HandleTypeDef *huart);
//void uart_read_completed_signal(UART_HandleTypeDef *huart);
//
//void uart_idle_interrupt_signal(UART_HandleTypeDef *huart);
//void uart_dma_full_signal(UART_HandleTypeDef *huart);

void get_dma_memory_msg(DMA_Stream_TypeDef *dma_stream, uint8_t *mem_id, uint16_t *remain_cnt);


//void get_infantry_info(void);
//void get_custom_data_info(void);

//uint8_t read_gimbal_offset(int32_t *pit_offset, int32_t *yaw_offset,
//                           int32_t *pit_buff_offset, int32_t *yaw_buff_offset);

//static void get_structure_param(void);
//static void gimbal_cali_msg_hook(uint8_t cur_type, uint8_t last_type);
uint8_t dma_current_memory_target(DMA_Stream_TypeDef *dma_stream);
uint16_t dma_current_data_counter(DMA_Stream_TypeDef *dma_stream);
extern UART_HandleTypeDef huart4;
#endif
