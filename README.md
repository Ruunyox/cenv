# C-ENV
### Temperature and Humidty Monitoring Tool

---

<p align="center">
<img src="cenv2.png"/>
</p>

C-ENV is a c-based application that communicates with an arduino over 
an 8N1 serial connection to read temperature and humidty data over 
time. It has been designed to work with slower sensors, such as the DHTxx 
series, that have query frequencies of ~ 0.5 Hz.

The program uses a simple ncurses interface to provide monitoring of 
variables from the terminal. The source code can be easily modified to 
produce readings from other sensors, such as light sensors, proximity 
sensors, pressure sensors, etc.

# Dependencies

- <ncurses.h>
- <DHT.h>
- <DHT_U.h> 

