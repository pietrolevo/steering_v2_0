/**
 * @file    steering.h
 * @author  Pietro Levo [pietro.levo.sc@gmail.com]
 * @date    2025-09-03 (date of creation)
 * @updated 2025-09-04 (date of last update)
 * @version v0.1.0
 * @prefix  STE
 *
 * @brief   Implementation of some software
 * @details This code implements bla bla
 *
 * @license Licensed under "THE BEER-WARE LICENSE", Revision 69 
 *          see LICENSE file in the root directory of this software component
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _STEERING_H_
#define _STEERING_H_

/* ---------- Includes -------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "bsp.h"
#include "can.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* ---------- Exported types -------------------------------------------------*/
typedef struct {
    BTN_handleTypedef hbtn[BTN_Device_NUM];
    RSW_handleTypedef hrsw[RSW_Device_NUM];
} Steering_Board;

/* ---------- Exported constants ---------------------------------------------*/


/* ---------- Exported variables ---------------------------------------------*/


/* ---------- Exported macros ------------------------------------------------*/


/* ---------- Exported functions ---------------------------------------------*/
void Steering_Init(void);
void Steering_Run(void);

/* ---------- Private types --------------------------------------------------*/


/* ---------- Private variables ----------------------------------------------*/


/* ---------- Private constants ----------------------------------------------*/


/* ---------- Private Macros -------------------------------------------------*/


#endif