# smoke-esp32-mq2-fan-control
esp32 Fan controller to un-smoke 

Fan controlled by esp32 using mq2 smoke cigarette sensor.
with ability to use fan speed (for 4 wired fan , like delta fan from server)

equipment:
mq2 sensor
esp32
relay
4 wire fan

Pinout - relay pin 25 , fan pwm - pin 26, sensor pin 34

When mq sensor feel smoke - it turns on fan for 10 min.


With ota, and debug web..
