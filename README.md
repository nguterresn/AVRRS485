# AVR RS485 ![vshield](https://img.shields.io/github/v/release/nguterresn/SELE-AVRRS485) ![sizeshield](https://img.shields.io/github/repo-size/nguterresn/SELE-AVRRS485)

### System Concept

<img width="874" alt="Captura de ecrã 2019-11-13, às 16 04 25" src="https://user-images.githubusercontent.com/38976366/68781265-6b33ae00-062f-11ea-969e-323784b92777.png">

### USART Configuration

### Initialization

Since the PlatoformIO serial monitor is set to a baudrate of 9600, the USART baudrate should be 9600 in order to monitor data:

```
/* uc freq = 8Mhz */
#define FOSC 8000000 
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
```

To be able to use USART, the receiver (RX) and the transmitter (TX) should be enable:
```
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
```

The frame format should have a **9 bit** to distinguish between address frames and data frames (+2 stop bits):
```
UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01)|(1<<UCSZ02);
```

### Frame Format

# | start | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | stopbit | stopbit
--- | --- | --- | --- |--- |--- |--- |--- |--- |--- |--- |--- |---
bit | LOW | X | X | X | X | X | X | X | X | X | HIGH | HIGH 

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
send_data(data);
```

Waits until the channel is empty, saves 0 into TXB8 (9 bit) and sends data:

```
...
while (!(UCSR0A & (1<<UDRE0)));

UCSR0B &= ~(1<<TXB80);

UDR0 = data;
```

### Reception

Address is defined as:
```
#define ADDRESS 1
```

Data is defined as:
```
#define DATATYPE 0
```

To receive data, call:

- ADDRESS to return an address;
- DATATYPE to return a data frame;

```
get_data(ADDRESS or DATATYPE):
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

###### Note: USART Parity Error (UPE0) is not enable since there is no parity bit in the initialize frame.

After checking errors appearance, ninth bit is filtered from UCSR0B register and data is returned:

```
...
ninthbit = UCSR0B; 
ninthbit = (ninthbit >> 1) & 0x01; 

if (ninthbit && type) {
	return receivedata & 0x0e;
} else if (!(ninthbit) && !(type)) {
	return receivedata;
} else return ERROR;
```

Since Master has the 15th address, all the slave are within 0 and 14:

```
...
return receivedata & 0x0e;
...
```

### Setup

LED and RS485 Enable Pin is setup as an Output. Enable Pin is set to 1 to enable transmission and reception:

```
pinMode(LED_PIN, OUTPUT);
pinMode(WREN_PIN, OUTPUT);
digitalWrite(WREN_PIN, HIGH);
```

To read the digital values of the DIP Switch, pull-up resistors are enable and node address is calculated:

```
pinMode(ADDR0_PIN, INPUT_PULLUP);
pinMode(ADDR1_PIN, INPUT_PULLUP);
pinMode(ADDR2_PIN, INPUT_PULLUP);
pinMode(ADDR3_PIN, INPUT_PULLUP);

ADDR = digitalRead(ADDR0_PIN) | digitalRead(ADDR1_PIN)<<1 | digitalRead(ADDR2_PIN)<<2 | digitalRead(ADDR3_PIN)<<3;
```

### System Diagram
#### Master-Slave - Slave Side

![blockdiagramMS](https://user-images.githubusercontent.com/38976366/69570464-c95d7b00-0fb7-11ea-93ed-bc8e6e174572.png)


On the Slave side, *state* = 1 means an **Adressed State**, meaning the slave is receiving orders from the master. So, *state* = 0 means an **Unaddressed State**, meaning the slave is not receiving data from the master.

In order to leave this **Unaddressed State**, the Slave must receive an address that matches its one:

```
ADDR = digitalRead(ADDR0_PIN) | digitalRead(ADDR1_PIN)<<1 | digitalRead(ADDR2_PIN)<<2 | digitalRead(ADDR3_PIN)<<3;
(...)
ADDR_R = get_data(ADDRESS);

if(ADDR_R == ADDR) ...
```

By the time the slave is  on the **Addressed State**, it receives data until it receives a different address. Changes to *state* = 0, to an **Unaddressed State**

```
...
command = get_data(DATATYPE);

  while (command != ERROR) {
				
		digitalWrite((command>>7) & 0x01, LED_PIN);

		command = get_data(DATATYPE);
	}

state = 0;
...
```


### PCB
This circuit was designed to be easily tested and programmed. In addition to the mandatory requirements, a FT232 interface, a AVR ISP and a 5V voltage regulator was added as optional requirements. 
- The FT232 interface will allow testing Rx and Tx communications using a FTDI chip connected to a USB port, using a computer to track results. [FTDI](https://en.wikipedia.org/wiki/FTDI)
- The ISP interface will allow uC to be programmed.
- 5V Voltage regulator allows acceptance of higher input voltages (up to 12V) without compromising the system behavior. 

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
