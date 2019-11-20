# SELE

#### System Concept

<img width="874" alt="Captura de ecrã 2019-11-13, às 16 04 25" src="https://user-images.githubusercontent.com/38976366/68781265-6b33ae00-062f-11ea-969e-323784b92777.png">

### PCB
This circuit was designed to be easily tested and programmed. In addition to the mandatory requirements, a FT232 interface, a AVR ISP and a 5V volateg regulator was added as optional requirements. 
- The FT232 interface will allow to test Rx and Tx communications using a FTDI chip connected to a USB port, using a computer to track results. [FTDI](https://en.wikipedia.org/wiki/FTDI)
- The ISP interface will allow uC to be programmed.
- 5V Voltage regulator allows acceptance of higher input voltages (up to 12V) without compromising the system behavior. 

#### Schematic

<img width="1002" alt="sch" src="https://user-images.githubusercontent.com/38976366/69010298-350b6c80-0956-11ea-9a10-7910c63e9a8f.png">

#### PCB Design

##### V0.1.1

<img width="900" alt="designpcb" src="https://user-images.githubusercontent.com/38976366/69010304-40f72e80-0956-11ea-9c7d-80db3697fb7a.png">

##### V0.1.2
- Crystal Capacitors (22pF) were changed to ceramic to match crystal performance requirements.
<img width="900" alt="PCB" src="https://user-images.githubusercontent.com/38976366/69197397-1dd0a880-0b29-11ea-8d56-e212b53b5c6b.png">
