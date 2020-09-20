/*
* Copyright (c) 2020 Tencent Cloud. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_smartconfig.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"

#include "qcloud_iot_export.h"
#include "qcloud_iot_demo.h"
#include "qcloud_wifi_config.h"
#include "board_ops.h"


/* normal WiFi STA mode init and connection ops */
#ifndef CONFIG_WIFI_CONFIG_ENABLED

/* WiFi router SSID  */
#define TEST_WIFI_SSID                 CONFIG_DEMO_WIFI_SSID
/* WiFi router password */
#define TEST_WIFI_PASSWORD             CONFIG_DEMO_WIFI_PASSWORD

static const int CONNECTED_BIT = BIT0;
static EventGroupHandle_t wifi_event_group;

bool wait_for_wifi_ready(int event_bits, uint32_t wait_cnt, uint32_t BlinkTime)
{
    EventBits_t uxBits;
    uint32_t cnt = 0;
    uint8_t blueValue = 0;

    while (cnt++ < wait_cnt) {
        uxBits = xEventGroupWaitBits(wifi_event_group, event_bits, true, false, BlinkTime / portTICK_RATE_MS);

        if (uxBits & CONNECTED_BIT) {
            Log_d("WiFi Connected to AP");
            return true;
        }

        blueValue = (~blueValue) & 0x01;
        set_wifi_led_state(blueValue);
    }

    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

    return false;
}


static void wifi_connection(void)
{
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = TEST_WIFI_SSID,
            .password = TEST_WIFI_PASSWORD,
        },
    };

    app_nvs_get_ssid_password(&wifi_config.sta.ssid, &wifi_config.sta.password);
    Log_i("Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

    esp_wifi_connect();
}

static esp_err_t _esp_event_handler(void* ctx, system_event_t* event)
{
    Log_i("event = %d", event->event_id);

    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
        Log_i("SYSTEM_EVENT_STA_START");
        wifi_connection();
        break;

        case SYSTEM_EVENT_STA_GOT_IP:
        Log_i("Got IPv4[%s]", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

        break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
        Log_i("SYSTEM_EVENT_STA_DISCONNECTED");
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        esp_wifi_connect();
        break;

        default:
        break;
    }

    return ESP_OK;
}

static void esp_wifi_initialise(void)
{
    tcpip_adapter_init();

    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(_esp_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

#endif //#ifnef CONFIG_DEMO_WIFI_BOARDING

void setup_sntp(void )
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    // to set more sntp server, plz modify macro SNTP_MAX_SERVERS in sntp_opts.h file
    // set sntp server after got ip address, you'd better adjust the sntp server to your area
    sntp_setservername(0, "time1.cloud.tencent.com");
    sntp_setservername(1, "cn.pool.ntp.org");
    sntp_setservername(2, "time-a.nist.gov");
    sntp_setservername(3, "cn.ntp.org.cn");

    sntp_init();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2019 - 1900) && ++retry < retry_count) {
        Log_d("Waiting for system time to be set... (%d/%d)", retry, retry_count);
        sleep(1);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    // Set timezone to China Standard Time
    setenv("TZ", "CST-8", 1);
    tzset();
}

void qcloud_demo_task(void* parm)
{
    bool wifi_connected = false;
    Log_i("qcloud_demo_task start");
    

    #if CONFIG_WIFI_CONFIG_ENABLED
    /* to use WiFi config and device binding with Wechat mini program */
    int wifi_config_state;
    //int ret = start_softAP("ESP8266-SAP", "12345678", 0);
    int ret = start_smartconfig();
    if (ret) {
        Log_e("start wifi config failed: %d", ret);
    } else {
        /* max waiting: 150 * 2000ms */
        int wait_cnt = 150;
        do {
            Log_d("waiting for wifi config result...");
            HAL_SleepMs(2000);            
            wifi_config_state = query_wifi_config_state();
        } while (wifi_config_state == WIFI_CONFIG_GOING_ON && wait_cnt--);
    }

    wifi_connected = is_wifi_config_successful();
    if (!wifi_connected) {
        Log_e("wifi config failed!");
        // setup a softAP to upload log to mini program
        start_log_softAP();
    }
    #else
    /* init wifi STA and start connection with expected BSS */
    esp_wifi_initialise();
    /* 20 * 1000ms */
    wifi_connected = wait_for_wifi_ready(CONNECTED_BIT, 20, 1000);
    #endif

    if (wifi_connected) {
        setup_sntp();
        Log_i("WiFi is ready, to do Qcloud IoT demo");
        Log_d("timestamp now:%d", HAL_Timer_current_sec());
        #ifdef CONFIG_QCLOUD_IOT_EXPLORER_ENABLED
        qcloud_iot_explorer_demo(CONFIG_DEMO_EXAMPLE_SELECT);
        #else
        qcloud_iot_hub_demo();
        #endif
    } else {
        Log_e("WiFi is not ready, please check configuration");
    }

    Log_w("qcloud_demo_task quit");
    vTaskDelete(NULL);
}

void app_control_task(void *parm){
    Log_i("app control task start");
    
    extern void app_control_run(void);
    app_control_run();

    Log_w("app control task quit");
    vTaskDelete(NULL);   
}

void app_nvs_task(void *parm){
    #define WIFI_SSID "wifissid"
    #define WIFI_PSWD "wifipasswd"
    
    if(ESP_OK == nvs_flash_init()){
        uint8_t wifi_ssid[32] = {0};
        uint8_t wifi_pswd[16] = {0};
        size_t len = 0;
        nvs_handle wifi_handle;
        while(1){
#if 0
            nvs_open("wifi", NVS_READWRITE, &wifi_handle);

            nvs_set_str(wifi_handle, "ssid", WIFI_SSID);
            nvs_set_str(wifi_handle, "pswd", WIFI_PSWD);

            nvs_commit(wifi_handle);
            nvs_close(wifi_handle);
#else
            nvs_open("wifi", NVS_READONLY, &wifi_handle);
            len = 32;
            nvs_get_str(wifi_handle, "ssid", (char *)wifi_ssid, &len);
            len = 16;
            nvs_get_str(wifi_handle, "pswd", (char *)wifi_pswd, &len);

            printf("ssid = %s\r\n", wifi_ssid);
            printf("pswd = %s\r\n", wifi_pswd);
            nvs_close(wifi_handle);
#endif
            HAL_SleepMs(1000000);            
            vTaskDelete(NULL);
        }
    }
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());

    //init log level
    IOT_Log_Set_Level(eLOG_DEBUG);
    Log_i("FW built time %s %s", __DATE__, __TIME__);

    board_init();

    nvs_flash_init();

    xTaskCreate(qcloud_demo_task, "qcloud_demo_task", 8196, NULL, 4, NULL);
    xTaskCreate(app_control_task, "app_control_task", 8196, NULL, 4, NULL);
    //xTaskCreate(app_nvs_task, "app_nvs_task", 2048, NULL, 3, NULL);
}


