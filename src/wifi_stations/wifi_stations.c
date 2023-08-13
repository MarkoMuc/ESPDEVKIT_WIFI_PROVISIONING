#include "wifi_stations.h"

// Description: HTTP get URI handler
//              The URI is set to /
httpd_uri_t uri_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

// Description: HTTP get URI handler
//              The URI is set to /creds
httpd_uri_t uri_post = {
    .uri      = "/creds",
    .method   = HTTP_POST,
    .handler  = post_handler,
    .user_ctx = NULL
};

void init_wifi(void* ap_event, void* sta_event, uint8_t boot_type, int* leds) {
    // Starts WIFI Initialization
    ESP_LOGI("WIFI","Starting WIFI initialization.");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    if(boot_type == 0) { // AP mode
        init_ap(ap_event, leds);
    } else if(boot_type == 1) { // STA mode
        init_sta(sta_event, leds);
    } else { // Shouldn't happen, turn all leds on if it does
        gpio_set_level(leds[0], 1);
        gpio_set_level(leds[1], 1);
        gpio_set_level(leds[2], 1);
    }
}
 
void init_ap(void* event, int* leds) {
    // AP configuration

    //TODO: add static IP
    esp_netif_create_default_wifi_ap();
    ESP_LOGI("WIFI","Starting AP mode configuration.");
    
    // Starts WIFI AP configuration
    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
        event, NULL, NULL));
    
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .channel = AP_CHANNEL,
            .password = AP_PASS,
            .max_connection = AP_MAX_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = true,
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WIFI","Finished AP configuration, SSID: %s; password :%s, channel:%d.", 
        AP_SSID, AP_PASS, AP_CHANNEL);

    gpio_set_level(leds[0],0);
    gpio_set_level(leds[1],1);
    gpio_set_level(leds[2],0);
}

void init_sta(void* event, int* leds) {
    char password[64];
    char ssid[32];
    
    ESP_LOGI("WIFI STA", "Getting Credentials.");
    
    // Gets WIFI credentials from the NVS Flash
    nvs_get_creds(password, ssid);

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Creates and assigns the event handlers
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event,
                    NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event,
                    NULL, &instance_got_ip));

    // Configures the WIFI
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
        },
    };

    int i = 0;
    while(ssid[i] != '\0') {
        wifi_config.sta.ssid[i] = (uint8_t) ssid[i];
        i++;
    }
    i = 0;

    while(password[i] != '\0') {
        wifi_config.sta.password[i] = (uint8_t) password[i];
        i++;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI("WIFI STA", "Finished configuring WIFI STA.");
    
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI("WIFI STA", "Connected to AP with SSID:%s  and password:%s",
                 ssid, password);
        gpio_set_level(leds[0], 0);
        gpio_set_level(leds[1], 0);
        gpio_set_level(leds[2], 1);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI("WIFI STA", "Failed to connect to AP with SSID:%s and password:%s",
                 ssid, password);
        save_boot_type_reset(0); // If it can't connect to the station, reset to AP mode
    } else {
        ESP_LOGE("WIFI STA", "UNEXPECTED EVENT");// Unknown event, reset to AP mode
        save_boot_type_reset(0);
    }
}
   
httpd_handle_t start_webserver()
{
    ESP_LOGI("WEB SERVER","Starting Server");
    
    // Uses default configuration 
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Handle to the server
    httpd_handle_t server = NULL;

    // Starts the server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Registers the handlers
         ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri_get));
         ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri_post));
    }
    ESP_LOGI("WEB SERVER","Finished starting Server");
    
    // If it fails, returns NULL
    return server;
}
