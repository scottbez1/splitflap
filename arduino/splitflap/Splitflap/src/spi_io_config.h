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

  #define LATCH_PIN (25)

  // Optional - uncomment if connecting the output enable pin of the 74HC595 shift registers
  // to the ESP32. You can otherwise hard-wire the output enable pins to always be enabled.
  // #define OUTPUT_ENABLE_PIN (27)

  #define PIN_NUM_MISO 39
  #define PIN_NUM_MOSI 32
  #define PIN_NUM_CLK  33

  // Note: You may need to slow this down to 3MHz if you're using a classic driver board;
  // the MIC5842 only officially supports up to 3.3MHz
  #define SPI_CLOCK 4000000

  #define BUFFER_ATTRS WORD_ALIGNED_ATTR

  // Note: must use HSPI to avoid conflict with ST7789 driver which uses VSPI
  #define SPI_HOST HSPI_HOST
  #define DMA_CHANNEL 1


  spi_device_handle_t spi_tx;
  spi_device_handle_t spi_rx;

  spi_transaction_t tx_transaction;
  spi_transaction_t rx_transaction;


#endif

#if !defined(__AVR_ATmega168__) && !defined(__AVR_ATmega328P__) && !defined(ARDUINO_ESP8266_WEMOS_D1MINI) && !defined(ESP32)
#error "Unknown/unsupported board for SPI mode. ATmega328-based boards (Uno, Duemilanove, Diecimila), ESP8266 and ESP32 are currently supported"
#endif

#ifdef CHAINLINK
#define MOTOR_BUFFER_LENGTH (NUM_MODULES * 2 / 3 + (NUM_MODULES % 3 != 0) * 2)
#define SENSOR_BUFFER_LENGTH (NUM_MODULES / 6 + (NUM_MODULES % 6 != 0))
#else
#define MOTOR_BUFFER_LENGTH (NUM_MODULES / 2 + (NUM_MODULES % 2 != 0))
#define SENSOR_BUFFER_LENGTH (NUM_MODULES / 4 + (NUM_MODULES % 4 != 0))
#endif


BUFFER_ATTRS uint8_t motor_buffer[MOTOR_BUFFER_LENGTH];
BUFFER_ATTRS uint8_t sensor_buffer[SENSOR_BUFFER_LENGTH];

#ifdef __AVR__
// Define placement new so we can initialize SplitflapModules at runtime into a static buffer.
// (see https://arduino.stackexchange.com/a/1499)
void* operator new(__attribute__((unused)) size_t size, void* ptr) {
  return ptr;
}
#endif

#ifdef ESP32
void reset_latch(spi_transaction_t *trans) {
    digitalWrite(LATCH_PIN, LOW);
}

void latch_registers(spi_transaction_t *trans) {
    digitalWrite(LATCH_PIN, HIGH);
}
#endif

// Static buffer for SplitflapModules (initialized at runtime)
static char moduleBuffer[NUM_MODULES][sizeof(SplitflapModule)];

SplitflapModule* modules[NUM_MODULES];

#ifdef CHAINLINK
static const uint8_t MOTOR_OFFSET[] = {0, 0, 1, 2, 3, 3};
#endif

inline void initialize_modules() {
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    // Create SplitflapModules in a statically allocated buffer using placement new
#ifdef CHAINLINK
    modules[i] = new (moduleBuffer[i]) SplitflapModule(motor_buffer[MOTOR_BUFFER_LENGTH - 1 - i/6*4 - MOTOR_OFFSET[i%6]], i % 2 == 0 ? 0 : 4, sensor_buffer[i/6], 1 << (i % 6));
#else
    modules[i] = new (moduleBuffer[i]) SplitflapModule(motor_buffer[MOTOR_BUFFER_LENGTH - 1 - i/2], i % 2 == 0 ? 0 : 4, sensor_buffer[i/4], 1 << (i % 4));
#endif
  }
  
  memset(motor_buffer, 0, MOTOR_BUFFER_LENGTH);
  memset(sensor_buffer, 0, SENSOR_BUFFER_LENGTH);

  // Initialize SPI
#ifdef IN_LATCH_PIN
  pinMode(IN_LATCH_PIN, OUTPUT);
  digitalWrite(IN_LATCH_PIN, HIGH);
#endif

#ifdef OUT_LATCH_PIN
  pinMode(OUT_LATCH_PIN, OUTPUT);
  digitalWrite(OUT_LATCH_PIN, LOW);
#endif

#ifdef LATCH_PIN
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, LOW);
#endif

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
      .mode=3,
      .duty_cycle_pos=0,
      .cs_ena_pretrans=0,
      .cs_ena_posttrans=0,
      .clock_speed_hz=SPI_CLOCK,
      .input_delay_ns=0,
      .spics_io_num=-1,
      .flags = 0,
      .queue_size=1,
      .pre_cb=NULL,
      .post_cb=NULL,
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
      .input_delay_ns=30,
      .spics_io_num=-1,
      .flags = SPI_DEVICE_HALFDUPLEX,
      .queue_size=1,
      .pre_cb=&latch_registers,
      .post_cb=&reset_latch,
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
  rx_transaction.tx_buffer = NULL;
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
    int val = SPI.transfer(motor_buffer[i]);
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

#ifdef CHAINLINK
void chainlink_set_led(uint8_t moduleIndex, bool on) {
  uint8_t groupPosition = moduleIndex % 6;
  uint8_t byteIndex = MOTOR_BUFFER_LENGTH - 1 - moduleIndex/6*4 - (groupPosition < 3 ? 1 : 2);
  uint8_t bitMask = (groupPosition < 3) ? (1 << (4 + groupPosition)) : (1 << (groupPosition - 3));
  if (on) {
    motor_buffer[byteIndex] |= bitMask;
  } else {
    motor_buffer[byteIndex] &= ~bitMask;
  }
}

static uint8_t chainlink_loopbackMotorByte(uint8_t loopbackIndex) {
  return MOTOR_BUFFER_LENGTH - 1 - (loopbackIndex / 2) * 4 - (((loopbackIndex % 2) == 0) ? 1 : 2);
}
static uint8_t chainlink_loopbackMotorBitMask(uint8_t loopbackIndex) {
  return (loopbackIndex % 2) == 0 ? (1 << 7) : (1 << 3);
}
static uint8_t chainlink_loopbackSensorByte(uint8_t loopbackIndex) {
  return loopbackIndex / 2;
}
static uint8_t chainlink_loopbackSensorBitMask(uint8_t loopbackIndex) {
  return (loopbackIndex % 2) == 0 ? 1 << 6 : 1 << 7;
}

bool chainlink_test_startup_loopback(bool results[NUM_LOOPBACKS]) {
    bool success = true;

    // Turn off all motors, leds, and loopbacks; make sure all loopback inputs read 0
    memset(motor_buffer, 0, MOTOR_BUFFER_LENGTH);
    motor_sensor_io();
    motor_sensor_io();

    for (uint8_t i = 0; i < NUM_LOOPBACKS; i++) {
      results[i] = ((sensor_buffer[chainlink_loopbackSensorByte(i)] & chainlink_loopbackSensorBitMask(i))) == 0;
      success &= results[i];
    }
    return success;
}

void chainlink_set_loopback(uint8_t loop_out_index) {
    // Turn on loopback output
    motor_buffer[chainlink_loopbackMotorByte(loop_out_index)] |= chainlink_loopbackMotorBitMask(loop_out_index);
}

/**
 * Validate that the loopback from loop_out_index can be read successfully. There must be AT LEAST 2 motor_sensor_io() invocations
 * between setting the loopback and validating it - one for turning on the shift register output and another to read in the shift
 * register input.
 */
bool chainlink_validate_loopback(uint8_t loop_out_index, bool results[NUM_LOOPBACKS]) {
    bool success = true;
    for (uint8_t loop_in_index = 0; loop_in_index < NUM_LOOPBACKS; loop_in_index++) {
      uint8_t expected_bit_mask = (loop_out_index == loop_in_index) ? chainlink_loopbackSensorBitMask(loop_in_index) : 0;
      uint8_t actual_bit_mask = sensor_buffer[chainlink_loopbackSensorByte(loop_in_index)] & chainlink_loopbackSensorBitMask(loop_in_index);

      bool ok = actual_bit_mask == expected_bit_mask;
      success &= ok;
      if (results != nullptr) {
        results[loop_in_index] = ok;
      }
    }

    // Turn off loopback output
    motor_buffer[chainlink_loopbackMotorByte(loop_out_index)] &= ~chainlink_loopbackMotorBitMask(loop_out_index);
    return success;
}

bool chainlink_test_all_loopbacks(bool loopback_result[NUM_LOOPBACKS][NUM_LOOPBACKS], bool loopback_off_result[NUM_LOOPBACKS]) {
    bool loopback_success = true;

    // Turn one loopback bit on at a time and make sure only that loopback bit is set
    for (uint8_t loop_out_index = 0; loop_out_index < NUM_LOOPBACKS; loop_out_index++) {
      chainlink_set_loopback(loop_out_index);
      motor_sensor_io();
      motor_sensor_io();
      loopback_success &= chainlink_validate_loopback(loop_out_index, loopback_result[loop_out_index]);
    }

    loopback_success &= chainlink_test_startup_loopback(loopback_off_result);

    return loopback_success;
}

#endif

#endif
