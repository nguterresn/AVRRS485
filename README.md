# AVR RS485 ![vshield](https://img.shields.io/github/v/release/nguterresn/SELE-AVRRS485) ![sizeshield](https://img.shields.io/github/repo-size/nguterresn/SELE-AVRRS485)

## Contents

- [System Concept](#System-Concept)
- [USART Configuration](#USART-Configuration)
- [Frame Format](#Frame-Format)
- [Transmission](#Transmission)
	- [Address Frame](#Address-Frame)
	- [Data Frame](#Data-Frame)
- [Reception](#Reception)
- [Setup](#Setup)
- [System Diagram](#System-Diagram)
	- [Multi-processor Communication Mode](#Multi-processor-Communication-Mode)
	- [LED Commands](#LED-Commands)
- [PCB](#PCB)
	- [Schematic](#Schematic)
	- [PCB Design](#PCB-Design)

### System Concept

<img width="874" alt="Captura de ecrã 2019-11-13, às 16 04 25" src="https://user-images.githubusercontent.com/38976366/68781265-6b33ae00-062f-11ea-969e-323784b92777.png">

### USART Configuration

### Initialization

Since the [PlatformIO](https://platformio.org) serial monitor is set to a baudrate of 9600, the USART baudrate should be 9600 in order to monitor data:

```
#define FOSC 16000000 
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
```
###### Note: Microcontroller frequency is set to 16 MHz.

To be able to use USART, the receiver (RX) and the transmitter (TX) should be enable:
```
UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<UCSZ02);
```

The frame format should have a **9th bit** to distinguish between address frames and data frames (+1 stop bits):
```
UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
```

### Frame Format

Frame | start | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | stopbit 
--- | --- | --- | --- |--- |--- |--- |--- |--- |--- |--- |--- 
Bit | LOW | X | X | X | X | X | X | X | X | X | HIGH 

### Transmission

#### Address Frame

To send an Address Frame:

```
send_addr(SLAVE1_ADDR);
```

Waits until the channel is empty, saves 1 into TXB8 (9 bit) and sends address:

```
...
while (!(UCSR0A & (1<<UDRE0)));

UCSR0B |= (1<<TXB80);

UDR0 = addr;
```

#### Data Frame

To send a Data Frame:

```
send_data(ON);
...
send_data(OFF);
```

Waits until the channel is empty, saves 0 into TXB8 (9 bit) and sends data:

```
...
while (!(UCSR0A & (1<<UDRE0)));

UCSR0B &= ~(1<<TXB80);

UDR0 = data;
```

### Reception

To receive, call:

```
get_data(&ADDRESS or &DATATYPE):
```

Waits for until receives something and checks if one of these error occur:

- Frame Error
- Data OverRun

```
...
while (!(UCSR0A & (1<<RXC0)));

if (UCSR0A & ((1<<FE0)|(1<<DOR0)))
	return ERROR;
```

###### Note: Frame Error happens when, i.e., the next character of stop bit is 0. 
###### Note: Data OverRun occurs when the receive buffer is full.
###### Note: USART Parity Error (UPE0) is not enable since there is no parity bit in the initialize frame.

After checking errors appearance, ninth bit is filtered from UCSR0B register and it is returned. The parameter buffer is updated with new received data from *UDR0*:

```
...
ninthbit = (ninthbit >> 1) & 0x01; 

*buffer = UDR0;

return ninthbit;
```

### Setup

LED and RS485 Enable Pin is setup as an Output. 

```
pinMode(LED_PIN, OUTPUT);
pinMode(WREN_PIN, OUTPUT);
```

To read the digital values of the DIP Switch, pull-up resistors are enable and node address is calculated:

```
pinMode(ADDR0_PIN, INPUT_PULLUP);
pinMode(ADDR1_PIN, INPUT_PULLUP);
pinMode(ADDR2_PIN, INPUT_PULLUP);
pinMode(ADDR3_PIN, INPUT_PULLUP);

ADDR = digitalRead(ADDR0_PIN) | digitalRead(ADDR1_PIN)<<1 | digitalRead(ADDR2_PIN)<<2 | digitalRead(ADDR3_PIN)<<3;
```

In case of master address, the pull-up resistors connected to the buttons are enable and the RS485 enable pin (WREN_PIN) is set to 1, in order to be able to send data to the slaves:

```
if (ADDR == MASTER_ADDR) {

	pinMode(BUT1_PIN, INPUT_PULLUP);
	pinMode(BUT2_PIN, INPUT_PULLUP);
	
	digitalWrite(WREN_PIN, HIGH);
	
} 
```

On the other hand, the Multi-processor Communication Mode is disable, the LED is turned off and the RS485 enable pin (WREN_PIN) is set to 0, in order to be able to receive data from the master:

```
/* SLAVE */
else
{
	/* Multi-processor Communication Mode */
	UCSR0A = (1<<MPCM0);
	
	digitalWrite(LED_PIN, LOW);
	
	digitalWrite(WREN_PIN, LOW);
}
```

### System Diagram
#### Master-Slave - Slave Side

![blockdiagramMS-2](https://user-images.githubusercontent.com/38976366/69921649-c49f3800-148b-11ea-84ee-ffaaa21135db.png)

#### Multi-processor Communication Mode

This method allows to distinguish between address and datatype frames. When a node is considered addressed, the multi-processor communication mode is enable, it only receives datatype commands and discards address frames, until multi-processor communication mode is disable:

```
...
/* If address received from master matches the local address set by the DIP switches. */

/* ADDRESSED */
if (ADDR_R == ADDR) {
	
	/* Multi-processor Communication Mode ENABLE */
	UCSR0A &= 0xfe; 
	
	/* Received frames are all of datatype format, addressed ones are discarded */
	
	/* Multi-processor Communication Mode DISABLE */
	UCSR0A |= (1<<MPCM0); 

} 
```

#### LED Commands

The LED is turned on and turned off by received ON and OFF (from the master), respectively, with value 255 and 127.
This received value is shifted 7 bits to have a value between 0 and 1, in order to command the LED:

```
...
digitalWrite(LED_PIN, data>>7 & 0x01);
...
```


### PCB
This circuit was designed to be easily tested and programmed. In addition to the mandatory requirements, a FT232 interface, a AVR ISP and a 5V voltage regulator was added as optional requirements. 
- The FT232 interface will allow testing Rx and Tx communications using a [FTDI](https://en.wikipedia.org/wiki/FTDI) chip connected to a USB port, using a computer to track results. 
- The [ISP](https://www.elementzonline.com/image/cache/catalog/data/products/Programmers%20and%20Debuggers/USBasp/isp_header-500x500.jpg) interface will allow uC to be programmed.
- 5V Voltage regulator (7805) allows acceptance of higher input voltages (up to 12V) without compromising the system behavior. 

#### Schematic

<img width="1002" alt="sch" src="https://user-images.githubusercontent.com/38976366/69010298-350b6c80-0956-11ea-9a10-7910c63e9a8f.png">

#### PCB Design

 - Trace's width: 0.3mm. Able to handle 1A through the circuit.


![tracewidth](https://github.com/nguterresn/SELE-AVRRS485/blob/master/SELE_ACS/Img/tracewidth.png)

##### V0.1.1

<img width="900" alt="designpcb" src="https://user-images.githubusercontent.com/38976366/69010304-40f72e80-0956-11ea-9c7d-80db3697fb7a.png">

##### V0.1.2
- Crystal Capacitors (22pF) were changed to ceramic to match crystal performance requirements.
<img width="900" alt="PCB" src="https://user-images.githubusercontent.com/38976366/69197397-1dd0a880-0b29-11ea-8d56-e212b53b5c6b.png">
