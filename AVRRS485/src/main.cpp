#include <Arduino.h>

#define MASTER_ADDR 15
#define SLAVE1_ADDR 13
#define SLAVE2_ADDR 14

uint8_t ADDR;

#define LED_PIN   13
#define ADDR3_PIN 11
#define ADDR2_PIN 10
#define ADDR1_PIN 9
#define ADDR0_PIN 8
#define BUT1_PIN  7
#define BUT2_PIN  6
#define WREN_PIN  2

#define FOSC 8000000 // Clock Speed #define BAUD 9600
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void asynch9_init(long ubrr) {

  /*Set baud rate */
  UBRR0H = (unsigned char)(ubrr>>8); 
  UBRR0L = (unsigned char) ubrr;
  /* Enable receiver and transmitter */ 
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);

  /* Set frame format: 8data, 2stop bit */ 
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void send_addr(uint8_t addr) {
  // put your code here, to send an address:
}

void send_data(uint8_t data) {
    // put your code here, to send a data byte:
    /* Wait for empty transmit buffer */
    while (!(UCSRnA & (1<<UDREn))));
    /* Copy 9th bit to TXB8 */
    UCSRnB &= ~(1<<TXB8); 
    if (data & 0x0100) // 10101010 & 100000000
      UCSRnB |= (1<<TXB8);
    /* Put data into buffer, sends the data */ 
    UDRn = data;
}

uint8_t get_data(uint8_t *data) {
  // put your code here, to receive a data byte using multi processor communication mode:
}

void setup() {
  // put your setup code here, to run once:
  asynch9_init(MYUBRR);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
}