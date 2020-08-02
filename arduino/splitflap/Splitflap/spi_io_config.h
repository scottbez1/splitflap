/*
   Copyright 2020 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef IO_CONFIG_H
#define IO_CONFIG_H

#include <SPI.h>

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #define OUT_LATCH_PIN (4)
  #define _OUT_LATCH_PORT PORTD
  #define _OUT_LATCH_BIT 4

  #define IN_LATCH_PIN (5)
  #define _IN_LATCH_PORT PORTD
  #define _IN_LATCH_BIT 5

  #define OUT_LATCH(){\
    _OUT_LATCH_PORT |= (1 << _OUT_LATCH_BIT);\
    _OUT_LATCH_PORT &= ~(1 << _OUT_LATCH_BIT);\
  }
  #define IN_LATCH() {\
    _IN_LATCH_PORT &= ~(1 << _IN_LATCH_BIT);\
    _IN_LATCH_PORT |= (1 << _IN_LATCH_BIT);\
  }

  #define SPI_CLOCK 3000000

  #define BUFFER_ATTRS
#endif

#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
  #define OUT_LATCH_PIN (D1)
  #define IN_LATCH_PIN (D2)

  #define OUT_LATCH() {digitalWrite(OUT_LATCH_PIN, HIGH); digitalWrite(OUT_LATCH_PIN, LOW);}
  #define IN_LATCH() {digitalWrite(IN_LATCH_PIN, LOW); digitalWrite(IN_LATCH_PIN, HIGH);}

  #define SPI_CLOCK 3000000

  #define BUFFER_ATTRS
#endif

#ifdef ESP32
  #include "driver/spi_master.h"
  #include "driver/spi_slave.h"

  #define OUT_LATCH_PIN (12)//Any pin will work
  #define IN_LATCH_PIN (27) //Any pin will work
  #define OUTPUT_ENABLE_PIN (32) // Any pin will work

  #define SPI_CLOCK 8000000

  #define BUFFER_ATTRS WORD_ALIGNED_ATTR

  #define SPI_HOST VSPI_HOST
  #define DMA_CHANNEL 2
  #define PIN_NUM_MISO 19
  #define PIN_NUM_MOSI 23
  #define PIN_NUM_CLK  18
  #define PIN_NUM_CS   5

  spi_device_handle_t spi_tx;
  spi_device_handle_t spi_rx;

  spi_transaction_t tx_transaction;
  spi_transaction_t rx_transaction;


#endif

#if !defined(__AVR_ATmega168__) && !defined(__AVR_ATmega328P__) && !defined(ARDUINO_ESP8266_WEMOS_D1MINI) && !defined(ESP32)
#error "Unknown/unsupported board for SPI mode. ATmega328-based boards (Uno, Duemilanove, Diecimila), ESP8266 and ESP32 are currently supported"
#endif


#define MOTOR_BUFFER_LENGTH (NUM_MODULES / 2 + (NUM_MODULES % 2 != 0))
#define SENSOR_BUFFER_LENGTH (NUM_MODULES / 4 + (NUM_MODULES % 4 != 0))

BUFFER_ATTRS uint8_t motor_buffer[MOTOR_BUFFER_LENGTH];
BUFFER_ATTRS uint8_t sensor_buffer[SENSOR_BUFFER_LENGTH];

#ifdef __AVR__
// Define placement new so we can initialize SplitflapModules at runtime into a static buffer.
// (see https://arduino.stackexchange.com/a/1499)
void* operator new(size_t size, void* ptr) {
  return ptr;
}
#endif

#ifdef ESP32
void out_latch(spi_transaction_t *trans) {
    digitalWrite(OUT_LATCH_PIN, HIGH);
    digitalWrite(OUT_LATCH_PIN, LOW);
}

void in_latch(spi_transaction_t *trans) {
    digitalWrite(IN_LATCH_PIN, LOW);
    digitalWrite(IN_LATCH_PIN, HIGH);
}
#endif

// Static buffer for SplitflapModules (initialized at runtime)
static char moduleBuffer[NUM_MODULES][sizeof(SplitflapModule)];

SplitflapModule* modules[NUM_MODULES];

inline void initialize_modules() {
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    // Create SplitflapModules in a statically allocated buffer using placement new
    modules[i] = new (moduleBuffer[i]) SplitflapModule(motor_buffer[(NUM_MODULES - i - 1)/2], i % 2 == 0 ? 0 : 4, sensor_buffer[i/4], 1 << (i % 4));
  }
  
  memset(motor_buffer, 0, MOTOR_BUFFER_LENGTH);
  memset(sensor_buffer, 0, SENSOR_BUFFER_LENGTH);

  // Initialize SPI
  pinMode(IN_LATCH_PIN, OUTPUT);
  digitalWrite(IN_LATCH_PIN, HIGH);
  pinMode(OUT_LATCH_PIN, OUTPUT);
  digitalWrite(OUT_LATCH_PIN, LOW);

#ifdef ESP32

  esp_err_t ret;

  //Initialize the SPI bus
  spi_bus_config_t tx_bus_config = {
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = PIN_NUM_MISO,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 1000,
  };
  ret=spi_bus_initialize(SPI_HOST, &tx_bus_config, DMA_CHANNEL);
  ESP_ERROR_CHECK(ret);

  spi_device_interface_config_t tx_device_config = {
      .command_bits=0,
      .address_bits=0,
      .dummy_bits=0,
      .mode=0,
      .duty_cycle_pos=0,
      .cs_ena_pretrans=0,
      .cs_ena_posttrans=0,
      .clock_speed_hz=SPI_CLOCK,
      .input_delay_ns=0,
      .spics_io_num=PIN_NUM_CS,
      .flags = 0,
      // .flags = SPI_DEVICE_HALFDUPLEX,
      .queue_size=1,
      .pre_cb=NULL,
      .post_cb=&out_latch,
  };
  ret=spi_bus_add_device(SPI_HOST, &tx_device_config, &spi_tx);
  ESP_ERROR_CHECK(ret);

  spi_device_interface_config_t rx_device_config = {
      .command_bits=0,
      .address_bits=0,
      .dummy_bits=0,
      .mode=2,
      .duty_cycle_pos=0,
      .cs_ena_pretrans=0,
      .cs_ena_posttrans=0,
      .clock_speed_hz=SPI_CLOCK,
      .input_delay_ns=0,
      .spics_io_num=-1,// PIN_NUM_CS,
      .flags = 0,
      // .flags = SPI_DEVICE_HALFDUPLEX,
      .queue_size=1,
      .pre_cb=&in_latch,
      .post_cb=NULL,
  };
  ret=spi_bus_add_device(SPI_HOST, &rx_device_config, &spi_rx);
  ESP_ERROR_CHECK(ret);

  memset(&tx_transaction, 0, sizeof(tx_transaction));
  tx_transaction.length = MOTOR_BUFFER_LENGTH*8;
  tx_transaction.tx_buffer = &motor_buffer;
  tx_transaction.rx_buffer = NULL;

  memset(&rx_transaction, 0, sizeof(rx_transaction));
  rx_transaction.length = SENSOR_BUFFER_LENGTH*8;
  rx_transaction.rxlength = SENSOR_BUFFER_LENGTH*8;
  rx_transaction.tx_buffer = &motor_buffer;
  rx_transaction.rx_buffer = &sensor_buffer;

#else
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
#endif
}

inline void motor_sensor_io() {
#ifdef ESP32
    esp_err_t ret;
    
    // Send data
    ret=spi_device_polling_transmit(spi_tx, &tx_transaction);
    assert(ret==ESP_OK);

    // Receive data
    ret=spi_device_polling_transmit(spi_rx, &rx_transaction);
    assert(ret==ESP_OK);
#else
  IN_LATCH();
  delayMicroseconds(1);

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
  // Due to timing differences in the SPI implementation on the ESP8266, the
  // first bit is "lost" when using SPI to shift in data from a 74HC165
  // register. To correct for that, we manually read the first bit before
  // using SPI to read byte-by-byte and shift the data accordingly.
  bool extra_bit = digitalRead(D6);
#endif

  for (uint8_t i = 0; i < MOTOR_BUFFER_LENGTH; i++) {
    int val = SPI.transfer(motor_buffer[MOTOR_BUFFER_LENGTH - 1 - i]);
    if (i < SENSOR_BUFFER_LENGTH) {
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
      sensor_buffer[i] = (extra_bit << 7) | (val >> 1);
      extra_bit = val & B00000001;
#else
      sensor_buffer[i] = val;
#endif
    }
  }

  OUT_LATCH();
#endif
}

#endif
