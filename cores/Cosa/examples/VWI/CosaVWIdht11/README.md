CosaVWIdht11
=============
Demonstration of the DHT11 and Virtual Wire Interface (VWI) driver
Transmits a simple message with humidity and temperature readings from 
DHT11. The messages may be monitored with the CosaVWIreceiver or 
the CosaVWIdebug sketch.

Circuit
-------
Connect RF433/315 Transmitter Data to Arduino/ATtiny84/85 D9/D3/D3,
and DHT11 data with pullup (approx. 5 Kohm) to D7/D10/D2. 
The LED (ATtiny84/85 D4/D5) is on when transmitting.
