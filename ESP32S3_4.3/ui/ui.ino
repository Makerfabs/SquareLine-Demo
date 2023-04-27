#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_SHT31.h>
#include "ui.h"

// NTP
const char *ntpServer = "120.25.108.11";

// Global value
int value_1 = 50;
int net_flag = 0;
int date_flag = 0;
int weather_flag = 0;
String ssid = "";
String pwd = "";

float temperature;
float humidity;
float tvoc;
float eCO2;

/*Change to your screen resolution*/
static const uint16_t screenWidth = 800;
static const uint16_t screenHeight = 480;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_SGP30 sgp;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */
);

// Uncomment for ST7262 IPS LCD 800x480
Arduino_RPi_DPI_RGBPanel *gfx = new Arduino_RPi_DPI_RGBPanel(
    bus,
    800 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
    480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 12000000 /* prefer_speed */, true /* auto_flush */);

#include "touch.h"

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if (touch_has_signal())
    {
        if (touch_touched())
        {
            data->state = LV_INDEV_STATE_PR;

            /*Set the coordinates*/
            data->point.x = touch_last_x;
            data->point.y = touch_last_y;
        }
        else if (touch_released())
        {
            data->state = LV_INDEV_STATE_REL;
        }
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println(LVGL_Arduino);
    Serial.println("I am LVGL_Arduino");

    // Init Display
    touch_init();

    sensor_init();

    gfx->begin();
    gfx->fillScreen(BLACK);

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();

    Serial.println("Setup done");

    xTaskCreatePinnedToCore(Task_TFT, "Task_TFT", 20480, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(Task_my, "Task_my", 40960, NULL, 3, NULL, 1);
}

void Task_TFT(void *pvParameters)
{
    while (1)
    {
        lv_timer_handler();
        vTaskDelay(10);
    }
}

long task_runtime_1 = 0;
void Task_my(void *pvParameters)
{
    while (1)
    {
        if (net_flag != 0)
            if ((millis() - task_runtime_1) > 1000)
            {
                my_func_1();

                if (net_flag == 2)
                {
                    display_time();
                }

                task_runtime_1 = millis();
            }

        if (net_flag == 1)
            my_func_2();

        vTaskDelay(100);
    }
}

void loop()
{
}

void my_func_1()
{
    // if (value_1 < 100)
    //     value_1 += 1;
    // if (value_1 >= 100)
    //     value_1 = 0;
    // lv_bar_set_value(ui_Bar1, value_1, LV_ANIM_OFF);

    // char temp[10];
    // sprintf(temp, "%d aaa", value_1);
    // lv_label_set_text(ui_Label4, temp);
    sensor_read();

    lv_bar_set_value(ui_Bar1, (int)temperature, LV_ANIM_OFF);
    lv_bar_set_value(ui_Bar2, (int)humidity, LV_ANIM_OFF);
    lv_bar_set_value(ui_Bar3, (int)tvoc, LV_ANIM_OFF);
    lv_bar_set_value(ui_Bar4, (int)eCO2, LV_ANIM_OFF);

    char temp[20];
    sprintf(temp, "Temp:\t%d C", (int)temperature);
    lv_label_set_text(ui_Label11, temp);
    sprintf(temp, "Humi:\t%d %%", (int)humidity);
    lv_label_set_text(ui_Label12, temp);
    sprintf(temp, "tvoc:\t%d ppb", (int)tvoc);
    lv_label_set_text(ui_Label13, temp);
    sprintf(temp, "eCO2:\t%d ppm", (int)eCO2);
    lv_label_set_text(ui_Label14, temp);
}

void my_func_2()
{

    WiFi.begin(ssid.c_str(), pwd.c_str());
    // WiFi.begin(SSID, PWD);

    int connect_count = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500);
        Serial.print(".");
        connect_count++;
        // if (connect_count > 20)
        // {
        //     net_flag = 0;
        //     return;
        // }
    }

    Serial.println("Wifi connect");
    configTime((const long)(8 * 3600), 0, ntpServer);

    // lv_label_set_text(ui_Label1, weather_request().c_str());
    lv_label_set_text(ui_Label1, "Shenzhen Thunderstorm");

    net_flag = 2;
}

void display_time()
{
    struct tm timeinfo;
    char temp[30];

    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
    }
    else
    {
        sprintf(temp, "%02d:%02d:%02d",
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        Serial.println(temp);
        lv_label_set_text(ui_Label15, temp);

        if (date_flag == 0)
        {
            date_flag = 1;
            int year = timeinfo.tm_year + 1900;
            int month = timeinfo.tm_mon + 1;
            int day = timeinfo.tm_mday;
            lv_calendar_set_today_date(ui_Calendar2, year, month, day);
            lv_calendar_set_showed_date(ui_Calendar2, year, month);
        }
    }
}

void sensor_init()
{
    if (!sht31.begin(0x44))
    {
        Serial.println("SHT31 not found.");
    }
    delay(100);

    if (!sgp.begin())
    {
        Serial.println("SGP30 not found.");
    }
}

void sensor_read()
{
    humidity = sht31.readHumidity();
    temperature = sht31.readTemperature();

    if (!isnan(temperature))
    { // check if 'is not a number'
        Serial.print("Temp *C = ");
        Serial.println(temperature);
    }
    else
    {
        Serial.println("Failed to read temperature");
    }

    if (!isnan(humidity))
    { // check if 'is not a number'
        Serial.print("Hum. % = ");
        Serial.println(humidity);
    }
    else
    {
        Serial.println("Failed to read humidity");
    }

    sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

    if (!sgp.IAQmeasure())
    {
        Serial.println("Measurement failed");
        return;
    }
    Serial.print("TVOC ");
    Serial.print(tvoc = sgp.TVOC);
    Serial.print(" ppb\t");
    Serial.print("eCO2 ");
    Serial.print(eCO2 = sgp.eCO2);
    Serial.println(" ppm");

    if (!sgp.IAQmeasureRaw())
    {
        Serial.println("Raw Measurement failed");
        return;
    }
}

uint32_t getAbsoluteHumidity(float temperature, float humidity)
{
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                // [mg/m^3]
    return absoluteHumidityScaled;
}

String weather_request()
{
    HTTPClient http;

    String ret_str = "Shenzhen ";

    Serial.print("[HTTP] begin...\n");

    String url = "http://api.weatherapi.com/v1/current.json?key=271578bfbe12438085782536232404&q=Shenzhen&aqi=no";
    http.begin(url);

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
            Serial.println(payload);

            // JSON
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            JsonObject obj = doc.as<JsonObject>();

            String cond_txt = doc["current"]["condition"]["text"];
            ret_str = ret_str + cond_txt;
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        ret_str = "Can't get weather";
    }

    http.end();
    return ret_str;
}