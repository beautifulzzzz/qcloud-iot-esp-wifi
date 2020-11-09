/*************************************************************************
	> File Name: nvs.c
	> Author: 
	> Mail: 
	> Created Time: Mon 09 Nov 2020 08:32:07 CST
 ************************************************************************/

esp_err_t app_nvs_init(void){
    
}

esp_err_t app_nvs_get_ssid_password(uint8_t *_ssid, uint8_t *_password){
    uint8_t ssid[100];      /**< SSID of target AP*/
    uint8_t password[100];  /**< password of target AP*/
    size_t ssid_len,password_len;
    nvs_handle wifi_handle;

    nvs_open("wifi", NVS_READONLY, &wifi_handle);
    esp_err_t ret1 = nvs_get_str(wifi_handle, "ssid", (char *)wifi_ssid, 100);
    esp_err_t ret2 = nvs_get_str(wifi_handle, "pswd", (char *)wifi_pswd, 100);

    ssid_len = ssid[0];
    password_len = password_len[0];
    printf("ssid = %s\r\n", &wifi_ssid[1]);//[0]=len
    printf("pswd = %s\r\n", &wifi_pswd[1]);//[0]=len
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

    ssid_len = strlen(_ssid)+1;//len+ssid
    password_len = strlen(_ssid)+1;
    ssid[0] = ssid_len;
    password[0] = password_len;
    memcpy(&ssid[1],_ssid,strlen(_ssid));
    memcpy(&password[1],_password,strlen(_password));

    nvs_open("wifi", NVS_READWRITE, &wifi_handle);
    esp_err_t ret1 = nvs_set_str(wifi_handle, "ssid", (char *)wifi_ssid, 100);
    esp_err_t ret2 = nvs_set_str(wifi_handle, "pswd", (char *)wifi_pswd, 100);

    printf("ssid = %s\r\n", &wifi_ssid[1]);//[0]=len
    printf("pswd = %s\r\n", &wifi_pswd[1]);//[0]=len
    nvs_close(wifi_handle);   

    return (ret1 && ret2);
}


