/*************************************************************************
	> File Name: moto_control.c
	> Author: 
	> Mail: 
	> Created Time: Sun 11 Oct 2020 18:36:25 HKT
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "lite-utils.h"
#include "utils_timer.h"
#include "board_ops.h"
#include "qcloud_iot_demo.h"
#include "qcloud_iot_ota_esp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "driver/pwm.h"
#include "driver/gpio.h"

#include "cJSON.h"



//////////////////////////////////////////////////////////////////////
//PWM
//////////////////////////////////////////////////////////////////////
#define PWM_1_OUT_IO_NUM    14 //D5
#define PWM_2_OUT_IO_NUM    12 //D6
#define PWM_3_OUT_IO_NUM    2
#define LED_IO_NUM          13 //D7 
#define GPIO_OUTPUT_PIN_SEL (1ULL<<LED_IO_NUM)

// PWM period 10us(100Khz), same as depth
#define PWM_PERIOD    (20000)

static const char *TAG = "pwm_example";

// pwm pin number
const uint32_t pin_num[3] = {
    PWM_1_OUT_IO_NUM,
    PWM_2_OUT_IO_NUM,
    PWM_3_OUT_IO_NUM,
};

// dutys table, (duty/PERIOD)*depth
uint32_t duties[3] = {
    1250,//down_up
    1500,//left_right
    1600
};

const int left = 1200;//600
const int right = 1800;
const int down = 900;//350
const int up = 1250;

int left_right[5] = {1200,1400,1600,1800,1500};
int down_up[5] = {1250,1134,1017,900,1250};

// phase table, (phase/180)*depth
int16_t phase[3] = {
    0,
    0,
    0
};

void app_control_run(void){
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    
    gpio_set_level(LED_IO_NUM, 1);


    pwm_init(PWM_PERIOD, duties, 3, pin_num);
    pwm_set_phases(phase);
    pwm_start();
}

char get_value(const char *jsonRoot){
    // jsonRoot 是您要剖析的数据
    //首先整体判断是否为一个json格式的数据
	cJSON *pParams = cJSON_Parse(jsonRoot);
	//如果是否json格式数据
    if(pParams != NULL){
        cJSON *pValue =  cJSON_GetObjectItem(pParams, "doumiao");
        if(pValue != NULL){
            Log_d("dp: doumiao:%d",pValue->valueint);
       
            int x = left_right[4];
            int y = down_up[4];
            if(pValue->valueint > 0  && pValue->valueint <17){
                pValue->valueint--;
                x = left_right[3-pValue->valueint%4];
                y = down_up[pValue->valueint/4];
            }
            
            Log_d("move to:x=%d y=%d",x,y);
            duties[0] = y;           
            pwm_set_duty(0,duties[0]);
            duties[1] = x;           
            pwm_set_duty(1,duties[1]);
            pwm_start();
        }

        pValue =  cJSON_GetObjectItem(pParams, "onoff");
        if(pValue != NULL){
            Log_d("dp: onoff:%d",pValue->valueint);
            if(pValue->valueint == 1){
                gpio_set_level(LED_IO_NUM, 1);
            }else{
                gpio_set_level(LED_IO_NUM, 0);
            }
        }
        cJSON_Delete(pParams); 
    }    

    return 1;
}


void app_dp_report(char *str){
    Log_d("str:%s", str);
    get_value(str);
}
