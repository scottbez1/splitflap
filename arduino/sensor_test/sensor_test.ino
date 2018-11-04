#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#define NUM_MODULES (12)

#define OUT_LATCH_PIN (4)
#define OUT_LATCH_PORT PORTD
#define OUT_LATCH_BIT 4

#define IN_LATCH_PIN (5)
#define IN_LATCH_PORT PORTD
#define IN_LATCH_BIT 5

#define SENSOR_BUFFER_LENGTH (NUM_MODULES / 4 + (NUM_MODULES % 4 != 0))
uint8_t sensor_buffer[SENSOR_BUFFER_LENGTH];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_MODULES, 6, NEO_GRB + NEO_KHZ800);
uint32_t color_green = strip.Color(0, 30, 0);
uint32_t color_purple = strip.Color(5, 0, 5);

void setup() {
  Serial.begin(38400);
  strip.begin();

  // Initialize SPI
  pinMode(IN_LATCH_PIN, OUTPUT);
  pinMode(OUT_LATCH_PIN, OUTPUT);
  digitalWrite(IN_LATCH_PIN, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(3000000, MSBFIRST, SPI_MODE0));
}

void loop() {
  IN_LATCH_PORT &= ~(1 << IN_LATCH_BIT);
  IN_LATCH_PORT |= (1 << IN_LATCH_BIT);
  for (uint8_t i = 0; i < SENSOR_BUFFER_LENGTH; i++) {
    sensor_buffer[i] = SPI.transfer(0);
  }
  OUT_LATCH_PORT |= (1 << OUT_LATCH_BIT);
  OUT_LATCH_PORT &= ~(1 << OUT_LATCH_BIT);


  for (uint8_t i = 0; i< SENSOR_BUFFER_LENGTH; i++) {
    for (uint8_t b = 0; b < 4; b++) {
      bool sensor_val = !(sensor_buffer[i] & (1 << b));
      strip.setPixelColor(4*i + b, sensor_val ? color_green : color_purple);
      Serial.write(sensor_val ? 'X' : '.');
    }
  }
  Serial.write('\n');
  strip.show();

  delay(10);
}
