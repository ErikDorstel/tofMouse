# tofMouse
ESP32 and VL53L5CX based Gesture Mouse
#### Features
* uses ST VL53L5CX as ToF (Time of Flight) sensor
* controls X, Y axis and left click
* 8x8 ToF sensor matrix
* optional 8x8 NeoPixel matrix
* 66.6 frames/s
* emulates a BLE Mouse
* 0 to 40 cm distance (20 cm to stop Y axis motion)
#### I2C bus
* SDA - GPIO 21
* SCL - GPIO 22
* address - 0x52
#### NeoPixel matrix
* GPIO 15
