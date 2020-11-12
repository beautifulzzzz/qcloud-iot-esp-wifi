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

#if 0
esp_err_t app_nvs_init(void){
   return ESP_OK;
}

esp_err_t app_nvs_get_ssid_password(uint8_t *_ssid, uint8_t *_password){
    uint8_t ssid[100];      /**< SSID of target AP*/
    uint8_t password[100];  /**< password of target AP*/
    size_t ssid_len,password_len;
    nvs_handle wifi_handle;

    nvs_open("wifi", NVS_READONLY, &wifi_handle);
    esp_err_t ret1 = nvs_get_str(wifi_handle, "ssid", (char *)ssid, 100);
    esp_err_t ret2 = nvs_get_str(wifi_handle, "pswd", (char *)password, 100);

    ssid_len = ssid[0];
    password_len = password_len[0];
    printf("ssid = %s\r\n", &ssid[1]);//[0]=len
    printf("pswd = %s\r\n", &password[1]);//[0]=len
    nvs_close(wifi_handle);   

    if((ret1 == ESP_OK) && (ret2 == ESP_OK)){
        memcpy(_ssid,&ssid[1],ssid_len);        
        memcpy(_password,&password[1],password_len);
        _ssid[ssid_len] = 0;
        _password[password_len] = 0;
    }

    return (ret1 && ret2);
}

esp_err_t app_nvs_get_ssid_password(uint8_t *_ssid, uint8_t *_password){
    uint8_t ssid[100];      /**< SSID of target AP*/
    uint8_t password[100];  /**< password of target AP*/
    size_t ssid_len,password_len;
    nvs_handle wifi_handle;

    ssid_len = strlen((char *)_ssid)+1;//len+ssid
    password_len = strlen((char *)_password)+1;
    ssid[0] = ssid_len;
    password[0] = password_len;
    memcpy(&ssid[1],_ssid,strlen((char *)_ssid));
    memcpy(&password[1],_password,strlen((char *)_password));

    nvs_open("wifi", NVS_READWRITE, &wifi_handle);
    esp_err_t ret1 = nvs_set_str(wifi_handle, "ssid", (char *)ssid);
    esp_err_t ret2 = nvs_set_str(wifi_handle, "pswd", (char *)password);

    printf("ssid = %s\r\n", &ssid[1]);//[0]=len
    printf("pswd = %s\r\n", &password[1]);//[0]=len
    nvs_close(wifi_handle);   

    return (ret1 && ret2);
}
#endif

