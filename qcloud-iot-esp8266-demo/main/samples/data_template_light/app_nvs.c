/*************************************************************************
	> File Name: app_nvs.c
	> Author: 
	> Mail: 
	> Created Time: Mon 09 Nov 2020 08:32:07 CST
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
#include "nvs.h"

esp_err_t app_nvs_get_ssid_password(uint8_t *ssid, uint8_t *password){
    nvs_handle wifi_handle;
    size_t len_ssid = 100,len_password = 100;

    nvs_open("wifi", NVS_READONLY, &wifi_handle);

    esp_err_t ret1 = nvs_get_str(wifi_handle, "ssid", (char *)ssid, &len_ssid);
    esp_err_t ret2 = nvs_get_str(wifi_handle, "pswd", (char *)password, &len_password);

    nvs_close(wifi_handle);   

    if((ret1 == ESP_OK) && (ret2 == ESP_OK)){
        Log_i("get_ssid = %s\r\n", ssid);
        Log_i("get_pswd = %s\r\n", password);
        return ESP_OK;
    }

    return ESP_FAIL;
}


esp_err_t app_nvs_set_ssid_password(uint8_t *ssid, uint8_t *password){
    nvs_handle wifi_handle;

    nvs_open("wifi", NVS_READWRITE, &wifi_handle);
    esp_err_t ret1 = nvs_set_str(wifi_handle, "ssid", (char *)ssid);
    esp_err_t ret2 = nvs_set_str(wifi_handle, "pswd", (char *)password);

    Log_i("save_ssid = %s\r\n", ssid);
    Log_i("save_pswd = %s\r\n", password);
    nvs_close(wifi_handle);   

    return (ret1 && ret2);
}


