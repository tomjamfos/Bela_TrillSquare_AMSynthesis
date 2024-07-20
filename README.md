# Bela_TrillSquare_AMSynthesis
A brief experiment using a Trill Square sensor to control an instance of amplitude modulation (see p.221 - p.224 of Curtis Roads's the computer music tutorial).

The XY locations of the touch are mapped to the frequencies of the carrier and modulator oscillator respectively. When there is no touch detected on the sensor the last recieved value is held untill a new touch gesture is made. A smoothing filter is used to reduce noise from the sensor readings.

Three LEDs are used to add depth to the interaction. 1 is switched on when a touch is detected, the other 2 display a range of light intensisty according to the location of the touch on the sensor.

![alt text](https://github.com/tomjamfos/Bela_TrillSquare_AMSynthesis/blob/main/bela_AM.jpg)
