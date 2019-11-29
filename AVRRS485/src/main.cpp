#include <Arduino.h>

#define MASTER_ADDR 15
#define SLAVE1_ADDR 13
#define SLAVE2_ADDR 14
#define ON 255  // b11111111
#define OFF 127 // b01111111
#define ERROR -1
#define ADDRESS 1
#define DATATYPE 0

uint8_t ADDR, ADDR_R, state;
uint8_t data;
uint8_t button1 = 0, button2 = 0;

#define LED_PIN   13
#define ADDR3_PIN 11
#define ADDR2_PIN 10
#define ADDR1_PIN 9
#define ADDR0_PIN 8
#define BUT1_PIN  7
#define BUT2_PIN  6
#define WREN_PIN  2

#define FOSC 16000000 // Clock Speed #define BAUD 9600
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void send_data(uint8_t data);

void asynch9_init(long ubrr) {

    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8); 
    UBRR0L = (unsigned char) ubrr;

    /* Enable receiver and transmitter */ 
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    /* Set frame format: 9data, 2stop bit. No parity bit, AS USART */ 
    UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01)|(1<<UCSZ02);
}

void send_addr(uint8_t addr) {
  // put your code here, to send an address:
    while (!(UCSR0A & (1<<UDRE0)));
    /* Copy 9th bit to TXB8 */
    UCSR0B |= (1<<TXB80); // 1 means address
    /* Put data into buffer, sends the data */ 
    UDR0 = addr;
}

void send_data(uint8_t data) {
    
    // put your code here, to send a data byte:
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1<<UDRE0)));
    /* Copy 9th bit to TXB8 */
    UCSR0B &= ~(1<<TXB80); // 0 means data
    /* Put data into buffer, sends the data */ 
    UDR0 = data;
}

int get_data(uint8_t *buffer) {
    
    // put your code here, to receive a data byte using multi processor communication mode:
	unsigned char status, ninthbit;

    /* Wait for data to be received */ 
    while (!(UCSR0A & (1<<RXC0)));

    /* Get status and 9th bit, then data */
    /* from buffer */ 
    status = UCSR0A; 
    ninthbit = UCSR0B; 

    /* If error, return -1 */
    if (status & ((1<<FE0)|(1<<DOR0)))
        return ERROR;// <- is signed

    /* Filter the 9th bit, then return */ 
    /* delimita o registo ao nono bit */
    ninthbit = (ninthbit >> 1) & 0x01; 

	*buffer = UDR0;

    return ninthbit;

}

void setup() {
  // put your setup code here, to run once:

  	asynch9_init(MYUBRR);
  	pinMode(LED_PIN, OUTPUT);
  	pinMode(WREN_PIN, OUTPUT);

  	pinMode(ADDR0_PIN, INPUT_PULLUP);
  	pinMode(ADDR1_PIN, INPUT_PULLUP);
  	pinMode(ADDR2_PIN, INPUT_PULLUP);
	pinMode(ADDR3_PIN, INPUT_PULLUP);

	digitalWrite(WREN_PIN, LOW);

  	ADDR = digitalRead(ADDR0_PIN) | digitalRead(ADDR1_PIN)<<1 | digitalRead(ADDR2_PIN)<<2 | digitalRead(ADDR3_PIN)<<3;

	if (ADDR == MASTER_ADDR) {
		pinMode(BUT1_PIN, INPUT_PULLUP);
		pinMode(BUT2_PIN, INPUT_PULLUP);
	} 
	else
	{
		/* Multi-processor Communication Mode */
		UCSR0A = (1<<MPCM0);
		digitalWrite(LED_PIN, LOW);
	}

}

void loop() {

	if (ADDR == MASTER_ADDR) {

		digitalWrite(WREN_PIN, HIGH);
			/* Pull up ? */
		while (!(digitalRead(BUT1_PIN))) {

			if (!button1) {
				send_addr(SLAVE1_ADDR);
				send_data(ON);
				button1 = 1;
			}
		}

		while (!(digitalRead(BUT2_PIN))) {

			if (!button2) {
				send_addr(SLAVE2_ADDR);
				send_data(ON);
				button2 = 1;
				}
			}

			if (button1 || button2)
				send_data(OFF);

			button1 = 0;
			button2 = 0;

	} else {

		get_data(&ADDR_R);

		/* ADDRESSED */
		if (ADDR_R == ADDR) {

			UCSR0A &= 0xfe; 

			/* Improve this - automate */

			get_data(&data);

			digitalWrite(LED_PIN, data>>7 & 0x01);

			/*get_data(&data);

			digitalWrite(LED_PIN, data>>7 & 0x01);*/

			/* til here */

			UCSR0A |= (1<<MPCM0); 

		} 
			
	}
}