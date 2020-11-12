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
#define PWM_1_OUT_IO_NUM    14
#define PWM_2_OUT_IO_NUM    12
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
    1400,
    1300,
    1600
};

// phase table, (phase/180)*depth
int16_t phase[3] = {
    0,
    0,
    0
};

int left_right_start = 0;//-1:left 0:stop  1:right
int down_up_start = 0;//-1:down  0:stop 1:right
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


    pwm_init(PWM_PERIOD, duties, 3, pin_num);
    pwm_set_phases(phase);
    pwm_start();
    
    const int left = 1300;
    const int right = 2000;
    const int down = 1400;
    const int up = 1900;

    int cur_left_right = 1300;
    int cur_down_up = 1400;
    while(1<2){
        if(left_right_start!=0){
            cur_left_right+=(left_right_start);
            if(cur_left_right < left){
                cur_left_right = left;
                left_right_start = 0;
            }else if(cur_left_right > right){
                cur_left_right = right;
                left_right_start = 0;
            }
            duties[1] = cur_left_right;           
            pwm_set_duty(1,duties[1]);
            pwm_start();
            Log_d("mv left right state:%d [%d]",left_right_start,cur_left_right);
        }

        if(down_up_start!=0){
            cur_down_up+=(down_up_start);
            if(cur_down_up < down){
                cur_down_up = down;
                down_up_start = 0;
            }else if(cur_down_up > up){
                cur_down_up = up;
                down_up_start = 0;
             
            }
            duties[0] = cur_down_up;           
            pwm_set_duty(0,duties[0]);
            pwm_start();
            Log_d("mv down_up state:%d [%d]",down_up_start,cur_down_up);
        }

        vTaskDelay(10 / portTICK_RATE_MS);
    }
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
            switch(pValue->valueint){
                case 0:
                    down_up_start = 1;
                    break;//up
                case 1:
                    down_up_start = -1;
                    break;//down
                case 2:
                    left_right_start = -1;
                    break;//left
                case 3:
                    left_right_start = 1;
                    break;//right
                case 4:
                    down_up_start = 0;
                    left_right_start = 0;
                    break;
                default:break;
            }
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
    }    
    return 1;
}


void app_dp_report(char *str){
    Log_d("str:%s", str);
    get_value(str);
}
