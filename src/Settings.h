#define BOARD_FIRMWARE_VERSION        "1.0.1"
#define BOARD_HARDWARE_VERSION        "1.0.0"

#define BOARD_NAME                    "TTGO T-Call"        // Name of your product. Should match App Export request info.
#define BOARD_VENDOR                  "Company Name"        // Name of your company. Should match App Export request info.
#define BOARD_TEMPLATE_ID             "TMPL0000"            // ID of the Tile Template. Can be found in Tile Template Settings

#define PRODUCT_WIFI_SSID             "Grow Control"         // Name of the device, to be displayed during configuration. Should match export request info.
#define BOARD_CONFIG_AP_URL           "our-product.cc"      // Config page will be available in a browser at 'http://our-product.cc/'

/*
 * Board configuration (see examples below).
 */



  // Custom board configuration
   #define BOARD_BUTTON_PIN            0                     // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true                  // true if button is "active-low"

  #define BOARD_LED_PIN               13                    // Set LED pin - if you have a single-color LED attached
  //#define BOARD_LED_PIN_R           27                    // Set R,G,B pins - if your LED is PWM RGB 
  //#define BOARD_LED_PIN_G           26
  //#define BOARD_LED_PIN_B           25
  //#define BOARD_LED_PIN_WS2812      33                    // Set if your LED is WS2812 RGB
  #define BOARD_LED_INVERSE           false                 // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        64         // 0..255 brightness control




/*
 * Advanced options
 */

#define BUTTON_HOLD_TIME_INDICATION   3000
#define BUTTON_HOLD_TIME_ACTION       10000

#define BOARD_PWM_MAX                 1023

#define LEDC_CHANNEL_1     1
#define LEDC_CHANNEL_2     2
#define LEDC_CHANNEL_3     3
#define LEDC_TIMER_BITS    10
#define LEDC_BASE_FREQ     12000

#define WIFI_NET_CONNECT_TIMEOUT      30000
#define WIFI_CLOUD_CONNECT_TIMEOUT    15000
#define WIFI_AP_CONFIG_PORT           80
#define WIFI_AP_IP                    IPAddress(192, 168, 4, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)

#define USE_TICKER

#if defined(APP_DEBUG)
  #define DEBUG_PRINT(...) BLYNK_LOG1(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)
#endif
//===========================T-Call=======================================
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22
#define I2C_SDA_2            18
#define I2C_SCL_2            19
#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00
#define ADC_BAT  // TCALL 35


#define BLYNK_PRINT Serial    
#define BLYNK_HEARTBEAT 30
#define TINY_GSM_MODEM_SIM800
//====================================Light pins===================
#define GROW_LIGTH 12
#define BLOOM_LIGTH 15
//==================================Fan cjntrol=====================
//#define GROW_FAN_PIN 34
#define BLOOM_FAN_PIN 25
#define BlOOM_FAN_SPEED_PIN 33

//#define GROW_HEATER_PIN 27
#define BLOOM_HEATER_PIN 14
//=======================================Pump============================
#define BLOOM_PUMP 32
//#define GROW_PUMP 35
//========================================DHT=============================
//#define DHTPIN 18
//#define DHTTYPE DHT11
//DHT dht(DHTPIN, DHTTYPE);
//========================================ds18b20=========================
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9
#define NUMSENSORS 4
