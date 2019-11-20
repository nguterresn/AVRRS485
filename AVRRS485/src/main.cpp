#include <Arduino.h>

#define MASTER_ADDR 15
#define SLAVE1_ADDR 13
#define SLAVE2_ADDR 14

uint8_t ADDR;
//

#define LED_PIN   13
#define ADDR3_PIN 11
#define ADDR2_PIN 10
#define ADDR1_PIN 9
#define ADDR0_PIN 8
#define BUT1_PIN  7
#define BUT2_PIN  6
#define WREN_PIN  2

void asynch9_init(long BAUD) {
  // put your code here, to setup asynchronous serial communication using 9 bits:
}

void send_addr(uint8_t addr) {
  // put your code here, to send an address:
}

void send_data(uint8_t data) {
  // put your code here, to send a data byte:
}

uint8_t get_data(uint8_t *data) {
  // put your code here, to receive a data byte using multi processor communication mode:
}

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}