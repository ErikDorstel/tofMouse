#include "FastLED.h"
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>
#include <BleMouse.h>

#define numLeds   64
#define brightLeds 5
CRGB leds[numLeds];

SparkFun_VL53L5CX VL53L5CX;
VL53L5CX_ResultsData tofData;
struct mouseStruct { int x; int y; int z; int x1; int y1; float total; };
struct bufferStruct { mouseStruct mouse[10]; int index=0; } buffer;

BleMouse bleMouse;
uint32_t bleTimer;

void setup() {
  Serial.begin(115200);
  LEDS.addLeds<NEOPIXEL,15>(leds,numLeds);
  FastLED.setBrightness(brightLeds);
  FastLED.setDither(DISABLE_DITHER);
  leds[27].setHSV(160,255,255); leds[28].setHSV(160,255,255);
  leds[35].setHSV(160,255,255); leds[36].setHSV(160,255,255); FastLED.show();
  bleTimer=millis();
  bleMouse.deviceName="ToF Gesture Mouse";
  bleMouse.deviceManufacturer="Erik Dorstel";
  Wire.begin();
  Wire.setClock(400000);
  VL53L5CX.begin();
  VL53L5CX.setResolution(8*8);
  VL53L5CX.setRangingFrequency(15);
  Serial.print("VL53L5CX Resolution - "); Serial.println(VL53L5CX.getResolution());
  Serial.print("VL53L5CX Ranging Frequency - "); Serial.println(VL53L5CX.getRangingFrequency());
  VL53L5CX.startRanging(); }

void putBuffer(mouseStruct value) {
  buffer.index++; if (buffer.index>9) { buffer.index=0; }
  buffer.mouse[buffer.index]=value; }

mouseStruct getBuffer(int index) {
  if (buffer.index+index<0) { return buffer.mouse[buffer.index+index+10]; }
  else { return buffer.mouse[buffer.index+index]; } }

void loop() {
  if (millis()>=bleTimer) { bleTimer=millis()+10000; if (!bleMouse.isConnected()) { bleMouse.begin(); Serial.println("BLE Mouse connection attempt."); } }
  if (VL53L5CX.isDataReady()) {
    if (VL53L5CX.getRangingData(&tofData)) {
      float total=0; float right=0; float left=0; float front=0; float rear=0; float height=0;
      int rows[8]={0,0,0,0,0,0,0,0}; int cols[8]={0,0,0,0,0,0,0,0};
      for (int pixel=0;pixel<numLeds;pixel++) {
        float value=tofData.distance_mm[pixel];
        int hue=map(value,0,400,0,255); int vol=255;
        if (value>400) { vol=0; }
        leds[pixel].setHSV(hue,255,vol);

        if (value<=400) {
          if (pixel%8<=3) { right++; } else { left++; }
          if (pixel<=31) { rear++; } else { front++; }
          cols[pixel%8]++;
          rows[pixel/8]++;
          total++; height+=value; } }

      if (total>0) {

        int colmin; for (int col=0;col<=7;col++) { if (cols[col]>0) { colmin=col; break; } }
        int colmax; for (int col=7;col>=0;col--) { if (cols[col]>0) { colmax=col; break; } }
        int rowmin; for (int row=0;row<=7;row++) { if (rows[row]>0) { rowmin=row; break; } }
        int rowmax; for (int row=7;row>=0;row--) { if (rows[row]>0) { rowmax=row; break; } }

        leds[colmin].setHSV(0,0,255); leds[colmax].setHSV(0,0,255);
        leds[rowmin*8].setHSV(0,0,255); leds[rowmax*8].setHSV(0,0,255);

        int x=(float(colmin+colmax)/2-3.5)*-10;
        int y=(4-rowmin)*-5;
        int z=(height/total-200)/-2;

        //Serial.println("Left: " + String(left) + " Right: " + String(right) + " Front: " + String(front) + " Rear: " + String(rear) + " Height: " + String(height/total));
        left=ceil(left*40/total);
        right=ceil(right*40/total);
        rear=ceil(rear*40/total);
        front=ceil(front*40/total);
        int x1=right-left;
        int y1=front-rear;

        //Serial.println("X: " + String(x1) + " Y: " + String(y) + " Z: " + String(z) + " Total: " + String(total));
        if (bleMouse.isConnected()) { bleMouse.move(x1,z); }
        putBuffer({x,y,z,x1,y1,total});
        if (getBuffer(0).y<-10 && getBuffer(0).y-getBuffer(-3).y<-10 && getBuffer(-3).y-getBuffer(-6).y>10) {
          leds[0].setHSV(0,255,255); if (bleMouse.isConnected()) { bleMouse.click(MOUSE_LEFT); } } }

      FastLED.show(); } } }
