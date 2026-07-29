/**
 * @file    bsp.c
 * @author  Pietro Levo [pietro.levo.sc@gmail.com]- Gizem Gure [gizem.gure.sc@gmail.com]
 * @date    2025-09-05 (date of creation)
 * @updated 2026-05-13 (date of last update)
 * @version v0.2.0
 * @prefix  BSP
 *
 * @brief   Implementation of some software
 * @details This code implements button and rottaryswitch logic
 *
 * @license Licensed under "THE BEER-WARE LICENSE", Revision 69 
 *          see LICENSE file in the root directory of this software component
 */

/*---------- Includes --------------------------------------------------------*/

#include "bsp.h"


/*---------- Private define --------------------------------------------------*/
#define BTN_THRESHOLD_HIGH 0.7f
#define BTN_THRESHOLD_LOW  0.3f

#define RSW_THRESHOLD_HIGH 0.7f
#define RSW_THRESHOLD_LOW  0.3f

/*---------- Private macro ---------------------------------------------------*/


/*---------- Private variables -----------------------------------------------*/
static struct GPIO_Tuple BTN_Device_to_GPIO_Tuple_map[BTN_Device_NUM] = {
    [BTN_1] = {.GPIO_Port = BTN_1_GPIO_Port, .GPIO_Pin = BTN_1_Pin},
    [BTN_2] = {.GPIO_Port = BTN_2_GPIO_Port, .GPIO_Pin = BTN_2_Pin},
    [BTN_3] = {.GPIO_Port = BTN_3_GPIO_Port, .GPIO_Pin = BTN_3_Pin},
    [BTN_4] = {.GPIO_Port = BTN_4_GPIO_Port, .GPIO_Pin = BTN_4_Pin},
    [BTN_5] = {.GPIO_Port = BTN_5_GPIO_Port, .GPIO_Pin = BTN_5_Pin},
    [BTN_6] = {.GPIO_Port = BTN_6_GPIO_Port, .GPIO_Pin = BTN_6_Pin},
    [BTN_7] = {.GPIO_Port = BTN_7_GPIO_Port, .GPIO_Pin = BTN_7_Pin},
    [BTN_8] = {.GPIO_Port = BTN_8_GPIO_Port, .GPIO_Pin = BTN_8_Pin},
    [BTN_9] = {.GPIO_Port = BTN_9_GPIO_Port, .GPIO_Pin = BTN_9_Pin}
};


static struct GPIO_Quad RSW_Device_to_GPIO_Quad_map[RSW_Device_NUM] = {
    [RSW_Device1] = { 
        .Pins = {
            {.GPIO_Port = RSW1_C1_GPIO_Port, .GPIO_Pin = RSW1_C1_Pin},
            {.GPIO_Port = RSW1_C2_GPIO_Port, .GPIO_Pin = RSW1_C2_Pin},
            {.GPIO_Port = RSW1_C4_GPIO_Port, .GPIO_Pin = RSW1_C4_Pin},
            {.GPIO_Port = RSW1_C8_GPIO_Port, .GPIO_Pin = RSW1_C8_Pin} 
        }
    },
    [RSW_Device2] = { 
        .Pins = {
            {.GPIO_Port = RSW2_C1_GPIO_Port, .GPIO_Pin = RSW2_C1_Pin},
            {.GPIO_Port = RSW2_C2_GPIO_Port, .GPIO_Pin = RSW2_C2_Pin},
            {.GPIO_Port = RSW2_C4_GPIO_Port, .GPIO_Pin = RSW2_C4_Pin},
            {.GPIO_Port = RSW2_C8_GPIO_Port, .GPIO_Pin = RSW2_C8_Pin} 
        }
    },
    [RSW_Device3] = { 
        .Pins = {
            {.GPIO_Port = RSW3_C1_GPIO_Port, .GPIO_Pin = RSW3_C1_Pin},
            {.GPIO_Port = RSW3_C2_GPIO_Port, .GPIO_Pin = RSW3_C2_Pin},
            {.GPIO_Port = RSW3_C4_GPIO_Port, .GPIO_Pin = RSW3_C4_Pin},
            {.GPIO_Port = RSW3_C8_GPIO_Port, .GPIO_Pin = RSW3_C8_Pin} 
        }
    }
};


/*---------- Private function prototypes -------------------------------------*/


/*---------- Exported Variables ----------------------------------------------*/


/*---------- Exported Functions ----------------------------------------------*/
GPIO_PinState BTN_Read_Pin(struct GPIO_Tuple gpio) {
    return HAL_GPIO_ReadPin(gpio.GPIO_Port, gpio.GPIO_Pin);
}

GPIO_PinState RSW_Read_Pin(struct GPIO_Quad gpio, uint8_t index) {
    return HAL_GPIO_ReadPin(gpio.Pins[index].GPIO_Port, gpio.Pins[index].GPIO_Pin);
}

/*########## BUTTONS #########################################################*/

void BTN_Devices_Init(BTN_handleTypedef *hbtn, float btn_IIR_alpha) {
    for (uint8_t i = 0; i < BTN_Device_NUM; i++) {
        hbtn[i].id = (enum BTN_Device)i;
        hbtn[i].gpio_tuple = BTN_Device_to_GPIO_Tuple_map[i];

        hbtn[i].value = 0;
        hbtn[i].prev_value = 0;
        hbtn[i].state = BTN_state_OFF;

        hbtn[i].behavior = BTN_behavior_MOMENTARY;

        IIR_Init(&hbtn[i].filter, btn_IIR_alpha);
    }

    // BTN_1 - BTN_4 momentary
    hbtn[BTN_1].behavior = BTN_behavior_MOMENTARY;
    hbtn[BTN_2].behavior = BTN_behavior_MOMENTARY;
    hbtn[BTN_3].behavior = BTN_behavior_MOMENTARY;
    hbtn[BTN_4].behavior = BTN_behavior_MOMENTARY;
 //   hbtn[BTN_9].behavior = BTN_behavior_MOMENTARY;

    // BTN_5 - BTN_8 toggle
    hbtn[BTN_5].behavior = BTN_behavior_TOGGLE;
    hbtn[BTN_6].behavior = BTN_behavior_TOGGLE;
    hbtn[BTN_7].behavior = BTN_behavior_TOGGLE;
    hbtn[BTN_8].behavior = BTN_behavior_TOGGLE;
    hbtn[BTN_9].behavior = BTN_behavior_TOGGLE;
}


void BTN_Device_Sample(BTN_handleTypedef *hbtn) {
    GPIO_PinState raw_value = BTN_Read_Pin(hbtn->gpio_tuple);

    float input = (raw_value == GPIO_PIN_RESET) ? 1.0f : 0.0f;

    float filtered = IIR_Update(&hbtn->filter, input);

    uint8_t current_pressed;

    if (filtered > BTN_THRESHOLD_HIGH) {
        current_pressed = 1;
    } else if (filtered < BTN_THRESHOLD_LOW) {
        current_pressed = 0;
    } else {
        current_pressed = hbtn->value; 
    }

    uint8_t rising_edge = (hbtn->value == 0 && current_pressed == 1);

    hbtn->value = current_pressed;
    if (hbtn->behavior == BTN_behavior_MOMENTARY) {
        hbtn->state = current_pressed ? BTN_state_ON : BTN_state_OFF;
    } 
    else if (hbtn->behavior == BTN_behavior_TOGGLE) {
        if (rising_edge) {
            hbtn->state = (hbtn->state == BTN_state_ON) ? BTN_state_OFF : BTN_state_ON;
        }
    }
}




void BTN_Device_SampleALL(BTN_handleTypedef *hbtn) {
    for (uint8_t i = 0; i < BTN_Device_NUM; i++) {
        BTN_Device_Sample(&hbtn[i]);
    }
}


enum BTN_State BTN_Device_GetState(BTN_handleTypedef *hbtn) {
    return hbtn->state;
}


/*########## ROTARY SWITCH ###################################################*/

void RSW_Devices_Init(RSW_handleTypedef *hrsw, float rsw_IIR_alpha) {
    for (uint8_t i = 0; i < RSW_Device_NUM; i++) {
        hrsw[i].id = (enum RSW_Device) i;
        hrsw[i].gpio_quad = RSW_Device_to_GPIO_Quad_map[i];
        hrsw[i].state = 0;

        for (int j = 0; j < 4; j++) {
            hrsw[i].value[j] = 0;
            IIR_Init(&hrsw[i].filters[j], rsw_IIR_alpha);
        }
    }
}


void RSW_Device_Sample(RSW_handleTypedef *hrsw) {
    GPIO_PinState raw_values[4] = {
        RSW_Read_Pin(hrsw->gpio_quad, 0),
        RSW_Read_Pin(hrsw->gpio_quad, 1),
        RSW_Read_Pin(hrsw->gpio_quad, 2),
        RSW_Read_Pin(hrsw->gpio_quad, 3)
    };

    for (uint8_t i = 0; i < 4; i++) {
        float input = (raw_values[i] == GPIO_PIN_SET) ? 1.0f : 0.0f;
        float filtered = IIR_Update(&hrsw->filters[i], input);

        if (filtered > RSW_THRESHOLD_HIGH) {
            hrsw->value[i] = 1;
        } else if (filtered < RSW_THRESHOLD_LOW) {
            hrsw->value[i] = 0;
        }
    }

    hrsw->state = 0;
    hrsw->state  |= (hrsw->value[0] & 0x01) << 0;
    hrsw->state  |= (hrsw->value[1] & 0x01) << 1;
    hrsw->state  |= (hrsw->value[2] & 0x01) << 2;
    hrsw->state  |= (hrsw->value[3] & 0x01) << 3;
}


void RSW_Device_SampleALL(RSW_handleTypedef *hrsw) {
    for (uint8_t i = 0; i < RSW_Device_NUM; i++) {
            RSW_Device_Sample(&hrsw[i]);
    }
}


uint8_t RSW_Device_GetState(RSW_handleTypedef *hrsw) {
    return hrsw->state;
}

void MCB_send_msg(uint32_t id, BTN_handleTypedef *hbtn, RSW_handleTypedef *hrsw) {
    uint8_t buffer[8] = {0};

    struct mcb_steering_hmi_devices_state_t hmi;

    CAN_TxHeaderTypeDef tx_header = {.RTR = CAN_RTR_DATA, .IDE = CAN_ID_STD};

    tx_header.StdId = id;

    hmi.btn_1_is_pressed = mcb_steering_hmi_devices_state_btn_1_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_1]) == BTN_state_ON);
    hmi.btn_2_is_pressed = mcb_steering_hmi_devices_state_btn_2_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_2]) == BTN_state_ON);
    hmi.btn_3_is_pressed = mcb_steering_hmi_devices_state_btn_3_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_3]) == BTN_state_ON);
    hmi.btn_4_is_pressed = mcb_steering_hmi_devices_state_btn_4_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_4]) == BTN_state_ON);

    hmi.btn_5_is_pressed = mcb_steering_hmi_devices_state_btn_5_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_5]) == BTN_state_ON);
    hmi.btn_6_is_pressed = mcb_steering_hmi_devices_state_btn_6_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_6]) == BTN_state_ON);
    hmi.btn_7_is_pressed = mcb_steering_hmi_devices_state_btn_7_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_7]) == BTN_state_ON);
    hmi.btn_8_is_pressed = mcb_steering_hmi_devices_state_btn_8_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_8]) == BTN_state_ON);
    hmi.btn_9_is_pressed = mcb_steering_hmi_devices_state_btn_9_is_pressed_encode(BTN_Device_GetState(&hbtn[BTN_9]) == BTN_state_ON);

    hmi.rot_sw_1_state = mcb_steering_hmi_devices_state_rot_sw_1_state_encode(RSW_Device_GetState(&hrsw[RSW_Device1]));  
    hmi.rot_sw_2_state = mcb_steering_hmi_devices_state_rot_sw_2_state_encode(RSW_Device_GetState(&hrsw[RSW_Device2]));  
    hmi.rot_sw_3_state = mcb_steering_hmi_devices_state_rot_sw_3_state_encode(RSW_Device_GetState(&hrsw[RSW_Device3])); 

    tx_header.DLC = mcb_steering_hmi_devices_state_pack(buffer, &hmi, MCB_STEERING_HMI_DEVICES_STATE_LENGTH);

    // TODO: check for HAL_TIMEOUT
    CAN_send(&MCB_Handle, buffer, &tx_header);
}

/*---------- Private Functions -----------------------------------------------*/