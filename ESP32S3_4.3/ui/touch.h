/* uncomment for GT911 */
#define TOUCH_GT911
#define TOUCH_GT911_SCL 18
#define TOUCH_GT911_SDA 17
#define TOUCH_GT911_INT -1
#define TOUCH_GT911_RST 38
#define TOUCH_GT911_ROTATION ROTATION_NORMAL
#define TOUCH_MAP_X1 786
#define TOUCH_MAP_X2 335
#define TOUCH_MAP_Y1 471
#define TOUCH_MAP_Y2 215

int touch_last_x = 0, touch_last_y = 0;

#include <Wire.h>
#include <TAMC_GT911.h>
TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, 800, 480);

void touch_init()
{

  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  ts.begin();
  ts.setRotation(TOUCH_GT911_ROTATION);
}

bool touch_has_signal()
{

  return true;
}

bool touch_touched()
{
  ts.read();
  if (ts.isTouched)
  {
    touch_last_x = map(ts.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, 800 - 1);
    touch_last_y = map(ts.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, 480 - 1);

    // Serial.print("  x: ");
    // Serial.print(ts.points[0].x);
    // Serial.print("  y: ");
    // Serial.print(ts.points[0].y);
    // Serial.print(' ');
    // Serial.print("  x: ");
    // Serial.print(touch_last_x);
    // Serial.print("  y: ");
    // Serial.print(touch_last_y);
    // Serial.println(' ');

    ts.isTouched = false;
    return true;
  }
  else
  {
    return false;
  }
}

bool touch_released()
{
  return true;
}
