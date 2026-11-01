#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PixelDust.h>

#define WIDTH       128
#define HEIGHT       64
#define N_GRAINS   2500
#define MAX_FPS      45

#define OLED_ADDR  0x3C

#define SCREEN_ROTATION 0
#define SWAP_XY  true
#define FLIP_X   true
#define FLIP_Y   true

Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1);
Adafruit_PixelDust sand(WIDTH, HEIGHT, N_GRAINS, 1, 128, false);

uint8_t mpuAddr = 0x68;
int16_t ax, ay, az;
uint32_t lastFrameUs = 0;

void showMessage(const char *msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor((WIDTH - strlen(msg) * 6) / 2, 28);
  display.print(msg);
  display.display();
}

bool mpuFound(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

void mpuWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(mpuAddr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

int16_t readWord() {
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return (int16_t)((hi << 8) | lo);
}

void mpuRead() {
  Wire.beginTransmission(mpuAddr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(mpuAddr, (uint8_t)6);
  ax = readWord();
  ay = readWord();
  az = readWord();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
    while (true) delay(1000);
  }
  display.setRotation(SCREEN_ROTATION);

  if (!mpuFound(0x68)) {
    if (mpuFound(0x69)) mpuAddr = 0x69;
    else {
      showMessage("No MPU9250");
      while (true) delay(1000);
    }
  }
  mpuWrite(0x6B, 0x00);
  delay(100);
  mpuWrite(0x1C, 0x08);

  if (!sand.begin()) {
    showMessage("Out of memory");
    while (true) delay(1000);
  }
  sand.randomize();

  showMessage("Virtual SandBox");
  delay(1000);
}

void loop() {
  uint32_t now;
  while ((now = micros()) - lastFrameUs < 1000000UL / MAX_FPS) {}
  lastFrameUs = now;

  mpuRead();
  int16_t x = SWAP_XY ? ay : ax;
  int16_t y = SWAP_XY ? ax : ay;
  if (FLIP_X) x = -x;
  if (FLIP_Y) y = -y;

  sand.iterate(x, y, az);

  display.clearDisplay();
  dimension_t gx, gy;
  for (grain_count_t i = 0; i < N_GRAINS; i++) {
    sand.getPosition(i, &gx, &gy);
    display.drawPixel(gx, gy, SSD1306_WHITE);
  }
  display.display();
}