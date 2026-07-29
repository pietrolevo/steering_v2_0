/**
 * @file    steering.c
 * @author  Pietro Levo [pietro.levo.sc@gmail.com]
 * @date    2025-09-03 (date of creation)
 * @updated 2026-05-13 (date of last update)
 * @version v0.1.1
 * @prefix  STE
 *
 * @brief   Implementation of some software
 * @details This code implements the function to activate the steering wheel
 *          and run the components
 *
 * @license Licensed under "THE BEER-WARE LICENSE", Revision 69 
 *          see LICENSE file in the root directory of this software component
 */

/*---------- Includes --------------------------------------------------------*/

#include "steering.h"

/*---------- Private define --------------------------------------------------*/
#define BTN_SAMPLE_TIME 10
#define RSW_SAMPLE_TIME 10
#define CAN_CYCLE_TIME 200

#define BTN_IIR_ALPHA 0.3f
#define RSW_IIR_ALPHA 0.66f
/*---------- Private macro ---------------------------------------------------*/


/*---------- Private variables -----------------------------------------------*/
static Steering_Board steering_v2_0;

/*---------- Private function prototypes -------------------------------------*/
void Steering_Sample(Steering_Board *steering);

/*---------- Exported Variables ----------------------------------------------*/


/*---------- Exported Functions ----------------------------------------------*/
void Steering_Init() {
    BTN_Devices_Init(steering_v2_0.hbtn, BTN_IIR_ALPHA);
    RSW_Devices_Init(steering_v2_0.hrsw, RSW_IIR_ALPHA);
}


void Steering_Run() {
    static uint32_t sfwTimSample = 0;
    static uint32_t sfwTimCan = 0;
    uint8_t payload[3];
    uint32_t now = HAL_GetTick();

    if (now >= sfwTimSample) {
        sfwTimSample = now + BTN_SAMPLE_TIME;
        Steering_Sample(&steering_v2_0);
    }

    if (now >= sfwTimCan) {
        sfwTimCan = now + CAN_CYCLE_TIME;
        MCB_send_msg(MCB_STEERING_HMI_DEVICES_STATE_FRAME_ID, steering_v2_0.hbtn, steering_v2_0.hrsw);
    }
}

/*---------- Private Functions -----------------------------------------------*/
void Steering_Sample(Steering_Board *steering) {
    BTN_Device_SampleALL(steering->hbtn);
    RSW_Device_SampleALL(steering->hrsw);
}
