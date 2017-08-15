#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "driver/gpio.h"
#include "esp_types.h"

/* FreeRTOS Task Handle -------------------------------------------------------*/

/* FreeRTOS Semaphore Handle --------------------------------------------------*/

/* FreeRTOS event group to signal when we are connected & ready to make a request */


static const char *TAG = "example";


//#define 	LED_GPIO_NUM	GPIO_NUM_25
#define 	LED_GPIO_NUM	GPIO_NUM_4


#define CONFIG_AP_SSID	"ESP32"
#define CONFIG_AP_PASSWORD	"12345678"

uint8_t ApMac[6];

/**
    * @brief  no .    
    * @note   no.
    * @param  no.
    * @retval no.
    */
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "Connecting to AP...");
        esp_wifi_connect();
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "Connected.");
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        //ESP_LOGI(TAG, "Wifi disconnected, try to connect again...");
        esp_wifi_connect();
        break;

    default:
        break;
    }
    
    return ESP_OK;
}

/**
    * @brief  no .    
    * @note   no.
    * @param  no.
    * @retval no.
    */
void led_init( void )
{
	gpio_set_direction( LED_GPIO_NUM , GPIO_MODE_OUTPUT );
}

/**
    * @brief  no .    
    * @note   no.
    * @param  no.
    * @retval no.
    */
void led_task( void *pvParameters )
{
	int level = 0;
	
	for( ;; )
	{
        gpio_set_level( LED_GPIO_NUM , level);
        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
	nvs_flash_init();

	led_init();
	 
	tcpip_adapter_init();
	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );

	esp_wifi_get_mac( ESP_IF_WIFI_AP , ApMac );
	
	wifi_config_t ap_config = {
		.ap = {
//			.ssid = ssid,
			.password = CONFIG_AP_PASSWORD,
			.ssid_len = 0,
			.max_connection = 4,
			.authmode = WIFI_AUTH_WPA_PSK
		}
	};

	sprintf( (char *)ap_config.ap.ssid , "%s_%02X%02X" , CONFIG_AP_SSID , ApMac[4] , ApMac[5] );

	esp_err_t tmp =  esp_wifi_set_config(WIFI_IF_AP, &ap_config);
	ESP_ERROR_CHECK(esp_wifi_start());
	esp_wifi_connect();
	
	xTaskCreate( &led_task, "led task", 512, NULL, 3, NULL );

}

