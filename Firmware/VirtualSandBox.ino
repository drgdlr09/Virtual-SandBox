#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_PixelDust.h>

#define WIDTH       128
#define HEIGHT       64
#define MAX_FPS      45

#define PIN_BUTTON   D0
#define OLED_ADDR  0x3C
#define ACCEL_ADDR 0x18

#define SCREEN_ROTATION 2
#define SWAP_XY  false
#define FLIP_X   true
#define FLIP_Y   false

Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1);
Adafruit_LIS3DH accel;
Adafruit_PixelDust *sand = nullptr;

const uint16_t GRAIN_LEVELS[] = {250, 500, 1000, 2000, 3000};
const uint8_t NUM_LEVELS = sizeof(GRAIN_LEVELS) / sizeof(GRAIN_LEVELS[0]);
uint8_t level = 2;

uint32_t lastFrameUs = 0;
uint32_t lastPressMs = 0;
bool wasPressed = false;

void showMessage(const char *msg) 
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor((WIDTH - strlen(msg) * 6) / 2, 28);
  display.print(msg);
  display.display();
}

void makeSand() 
{
  delete sand;
  sand = new Adafruit_PixelDust(WIDTH, HEIGHT, GRAIN_LEVELS[level], 1, 128, false);
  if (!sand->begin()) {
    showMessage("Out of memory");
    while (true) delay(1000);
  }
  sand->randomize();
}

void nextLevel() 
{
  level = (level + 1) % NUM_LEVELS;
  makeSand();
  char msg[20];
  snprintf(msg, sizeof(msg), "%u grains", GRAIN_LEVELS[level]);
  showMessage(msg);
  delay(500);
}

void setup() 
{
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  Wire.begin();
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) 
  {
    Serial.println("OLED not found");
    while (true) delay(1000);
  }
  display.setRotation(SCREEN_ROTATION);

  if (!accel.begin(ACCEL_ADDR)) 
  {
    showMessage("No accelerometer");
    while (true) delay(1000);
  }
  accel.setRange(LIS3DH_RANGE_4_G);
  accel.setDataRate(LIS3DH_DATARATE_100_HZ);

  randomSeed(esp_random());
  makeSand();
  showMessage("Virtual SandBox");
  delay(1000);
}

void loop() 
{
  uint32_t now;
  while ((now = micros()) - lastFrameUs < 1000000UL / MAX_FPS) {}
  lastFrameUs = now;

  bool pressed = digitalRead(PIN_BUTTON) == LOW;
  if (pressed && !wasPressed && millis() - lastPressMs > 50) 
  {
    lastPressMs = millis();
    nextLevel();
  }
  wasPressed = pressed;

  accel.read();
  int16_t x = SWAP_XY ? accel.y : accel.x;
  int16_t y = SWAP_XY ? accel.x : accel.y;
  if (FLIP_X) x = -x;
  if (FLIP_Y) y = -y;
  int16_t z = accel.z;

  sand->iterate(x, y, z);

  display.clearDisplay();
  dimension_t gx, gy;
  for (grain_count_t i = 0; i < GRAIN_LEVELS[level]; i++) 
  {
    sand->getPosition(i, &gx, &gy);
    display.drawPixel(gx, gy, SSD1306_WHITE);
  }
  display.display();
}