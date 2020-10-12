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
#include "cJSON.h"



//////////////////////////////////////////////////////////////////////
//PWM
//////////////////////////////////////////////////////////////////////
#define PWM_1_OUT_IO_NUM    14
#define PWM_2_OUT_IO_NUM    12
#define PWM_3_OUT_IO_NUM    2

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

void app_control_run(void){
    pwm_init(PWM_PERIOD, duties, 3, pin_num);
    pwm_set_phases(phase);
    pwm_start();
    
    int left = 1300;
    int right = 2000;
    int left_right_middle = (left+right)/2;
    char left_right_start = 0;
    int up = 1400;
    int down = 1900;
    int up_down_middle = (up+down)/2;
    char up_down_start = 0;

    int cur_left_right = 1300;
    int cur_up_down = 1400;
    while(1<2){
        /*
        if(t%3 == 0){
            duties[1] = duties[1] == 2000 ? 1300:2000;
            pwm_set_duty(1,duties[1]);
            pwm_start();
        }else if(t%5 == 0){
            duties[0] = duties[0] == 1900 ? 1400:1900;           
            pwm_set_duty(0,duties[0]);
            pwm_start();
        }
        */
        t++;
        vTaskDelay(1000 / portTICK_RATE_MS);
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
                    duties[0] = duties[0] == 1700 ? 1600:1700;           
                    pwm_set_duty(0,duties[0]);
                    pwm_start();
                    break;//up
                case 1:
                    duties[0] = duties[0] == 1900 ? 1400:1900;           
                    pwm_set_duty(0,duties[0]);
                    pwm_start();
                    break;//down
                case 2:
                    duties[1] = duties[1] == 2000 ? 1300:2000;
                    pwm_set_duty(1,duties[1]);
                    pwm_start();
                    break;//left
                case 3:
                    duties[1] = duties[1] == 2000 ? 1300:2000;
                    pwm_set_duty(1,duties[1]);
                    pwm_start();
                    break;//right
                default:break;
            }
        }
    }    
    return 1;
}


void app_dp_report(char *str){
    Log_d("str:%s", str);
    get_value(str);
}
