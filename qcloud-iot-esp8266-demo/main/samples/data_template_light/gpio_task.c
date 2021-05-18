/*************************************************************************
  > File Name: gpio_task.c
  > Author: 
  > Mail: 
  > Created Time: Sun 21 Mar 2021 13:54:59 HKT
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
#include "freertos/event_groups.h"


#include "esp_system.h"
#include "driver/pwm.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "nvs.h"
#include "esp_task_wdt.h"


#define GPIO_KEY            16

#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_KEY))

void button_task_run(void *arg){
    int time = 0;
    int reboot = 0;
    uint32_t cnt = 0,cnt2 = 0;
    uint8_t led_r_on = 0;
    while(1){
        if(reboot != 0){
            reboot++;
            if(reboot > 20){
                reboot = 0;
                esp_restart();
            }
        }else{
            if(gpio_get_level(GPIO_KEY) == 0){      
                time += 20;
            }else{
                if(time > 0){
                    if(time < 1500){
                        Log_i("short press");
                    }else if(time > 2500){
                        uint8_t reset = 3;
                        extern esp_err_t app_nvs_set_reset(uint8_t *reset);
                        app_nvs_set_reset(&reset);
                        Log_i("long press");
                        
                        reboot = 1;
                    }
                }
                time = 0;
            }
        }
        vTaskDelay(20 / portTICK_RATE_MS);
    }
}

void gpio_task_init(void){
    gpio_config_t io_conf;

    //bit mask of the pins, use GPIO9 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
}

