/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "bsp.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "mcb.h"

void CAN_build_payload(uint8_t *payload, BTN_handleTypedef *hbtn, RSW_handleTypedef *hrsw) {
    struct mcb_steering_hmi_devices_state_t hmi;

    hmi.btn_1_is_pressed = (BTN_Device_GetState(&hbtn[BTN_1]) == BTN_state_ON);
    hmi.btn_2_is_pressed = (BTN_Device_GetState(&hbtn[BTN_2]) == BTN_state_ON);
    hmi.btn_3_is_pressed = (BTN_Device_GetState(&hbtn[BTN_3]) == BTN_state_ON);
    hmi.btn_4_is_pressed = (BTN_Device_GetState(&hbtn[BTN_4]) == BTN_state_ON);

    hmi.btn_5_is_pressed = (BTN_Device_GetState(&hbtn[BTN_5]) == BTN_state_ON);
    hmi.btn_6_is_pressed = (BTN_Device_GetState(&hbtn[BTN_6]) == BTN_state_ON);
    hmi.btn_7_is_pressed = (BTN_Device_GetState(&hbtn[BTN_7]) == BTN_state_ON);
    hmi.btn_8_is_pressed = (BTN_Device_GetState(&hbtn[BTN_8]) == BTN_state_ON);
    hmi.btn_9_is_pressed = (BTN_Device_GetState(&hbtn[BTN_9]) == BTN_state_ON);

    hmi.rot_sw_1_state = RSW_Device_GetState(&hrsw[RSW_Device1]);  
    hmi.rot_sw_2_state = RSW_Device_GetState(&hrsw[RSW_Device2]);  
    hmi.rot_sw_3_state = RSW_Device_GetState(&hrsw[RSW_Device3]); 

    mcb_steering_hmi_devices_state_pack(payload, &hmi, 3u);
}


void CAN_ErrorHandler(CAN_HandleTypeDef *hcan) {
    char buf[20];
    uint32_t error = HAL_CAN_GetError(hcan);

    HAL_CAN_ResetError(hcan);
}
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  CAN_FilterTypeDef filter;
  filter.FilterActivation = ENABLE;
  filter.FilterBank = 0;
  filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter.FilterIdHigh = 0x0000;
  filter.FilterIdLow = 0x0000;
  filter.FilterMaskIdHigh = 0x0000;
  filter.FilterMaskIdLow = 0x0000;
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan1, &filter) != HAL_OK) {
    Error_Handler();
  }
  

  if (HAL_CAN_ActivateNotification(&hcan1,
    CAN_IT_TX_MAILBOX_EMPTY |
    CAN_IT_RX_FIFO0_MSG_PENDING |
    CAN_IT_RX_FIFO0_FULL |
    CAN_IT_RX_FIFO0_OVERRUN |
    CAN_IT_RX_FIFO1_MSG_PENDING |
    CAN_IT_RX_FIFO1_FULL |
    CAN_IT_RX_FIFO1_OVERRUN |
    CAN_IT_WAKEUP |
    CAN_IT_SLEEP_ACK |
    CAN_IT_ERROR_WARNING |
    CAN_IT_ERROR_PASSIVE |
    CAN_IT_BUSOFF |
    CAN_IT_LAST_ERROR_CODE |
    CAN_IT_ERROR) != HAL_OK) {
      Error_Handler();
  }
  

  if (HAL_CAN_Start(&hcan1) != HAL_OK) {
    Error_Handler();
  }

  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */



/*
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {}
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {}                                       
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan) {}                                                       
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan) {}                                                        
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {}                                         
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {}                                       
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {}
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData);
}


static HAL_StatusTypeDef CAN_wait(CAN_HandleTypeDef *hcan, uint8_t timeout) {
  uint32_t tick = HAL_GetTick();
  while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
    if (HAL_GetTick() - tick > timeout) return HAL_TIMEOUT;
  }
  return HAL_OK;
}

HAL_StatusTypeDef CAN_send(CAN_HandleTypeDef *hcan, uint8_t *buffer, CAN_TxHeaderTypeDef *header) {
  uint32_t mailbox;

  HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(hcan, header, buffer, &mailbox);

  return status;
}

void CAN_steering_Msg_send(CAN_HandleTypeDef *hcan, uint8_t *buffer, uint8_t len) {
  CAN_TxHeaderTypeDef header;
  header.StdId = 0x16E;
  header.IDE = CAN_ID_STD;
  header.RTR = CAN_RTR_DATA;
  header.DLC = len;
  header.TransmitGlobalTime = DISABLE;

  if (CAN_send(hcan, buffer, &header) != HAL_OK) {
    CAN_ErrorHandler(hcan);
  }
}

/* USER CODE END 1 */

