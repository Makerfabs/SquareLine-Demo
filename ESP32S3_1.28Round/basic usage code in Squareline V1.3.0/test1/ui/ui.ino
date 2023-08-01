#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <CST816S.h>

#define TFT_BLK 45
#define TOUCH_INT 40
#define TOUCH_SDA 38
#define TOUCH_SCL 39
#define TOUCH_RST 16


CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT); // sda, scl, rst, irq
/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 0
/* USBSerial debugging */
void my_print(const char * buf)
{
    USBSerial.printf(buf);
    USBSerial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX = 0, touchY = 0;

    if (!touch.available())
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touch.data.x;
        data->point.y = touch.data.y;

        USBSerial.print("Data x ");
        USBSerial.println(touchX);

        USBSerial.print("Data y ");
        USBSerial.println(touchY);
    }
}

void setup()
{
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, 1);

    touch.begin();
    USBSerial.begin( 115200 ); /* prepare for possible USBSerial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    USBSerial.println( LVGL_Arduino );
    USBSerial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 0 ); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );


    ui_init();

    USBSerial.println( "Setup done" );
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}
