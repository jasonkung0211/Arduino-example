
//Hall-effect sensor handbrake
//CPO 2021-10-18 REV3 to simulate load cell response or work with soft stop washers
//#define DEBUG 1  //Uncomment to observe output in the debug terminal
#ifdef DEBUG
  #include <SoftwareSerial.h>
#endif
#include <Joystick.h>
#define joyX A3  // Pin receiving HallSensor output

//**********MUST BE ADJUSTED FOR YOUR USE CASE***************
float oil_ValidRange = 0;  //move the brake response to the last X% of the range to account for "soft stop" in oil-filled master cylinder or resistance washers
                           //0.1 look at last 10% of the range
                           //0.4 look at last 40% of the range, etc.
                           //Set to 0 for linear range
//***********************************************************

int xAxis_ = 0;
int xMin = 9999;
int xMax = 0;
int xPrev = -1;
int oilMin = 0;
int calcJoyVal = 0;
int calcJoyValPrev = 0;
int xSpan = 0;
Joystick_ Joystick(0x12, JOYSTICK_TYPE_JOYSTICK, 0, 0,true,false,false,false,false,false,false,false,false,false,false);
const bool initAutoSendState = true;

void setup() {
  Joystick.setXAxisRange(-127, 127);
  Joystick.begin();
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
}

void loop() {
  xAxis_ = analogRead(joyX);  //Current raw brake value
  if (xAxis_ != xPrev) {  //Only run calculations if the raw sensor input has changed
    xPrev = xAxis_;
    if (xAxis_ > xMax) {
      xMax=xAxis_;  //Maximum value seen so far - reset known xMax value (100% braking)
    }
    else if (xAxis_ < xMin) {
      xMin = xAxis_;  //Minumum value seen so far - reset known xMin value (0% braking) 
    }

    if (oil_ValidRange > 0) {  //Oil-filled only: look only at last portion of range to account for master cylinder resistance 
       oilMin = xMax - (oil_ValidRange * xMax);
       if (xAxis_ >= oilMin) { //in braking range
        calcJoyVal = map(xAxis_,oilMin,xMax,-128,128);  
      }
      else {
        calcJoyVal = -128;
      }
      
    //Non-oil-filled only: linear output
    }
    else {  
      calcJoyVal = map(xAxis_,xMin,xMax,-128,128);  //Convert the arbitrary sensor span to joystick values of -127 to 127 
    }
    
    calcJoyVal = constrain(calcJoyVal,-128,128);
    if (calcJoyVal != calcJoyValPrev) { //only send usb joystic value out if it has changed from previous
      Joystick.setXAxis(calcJoyVal);
      calcJoyValPrev = calcJoyVal;
    }
    
    #ifdef DEBUG
      Serial.print("  oil_ValidRange=");
      Serial.print(oil_ValidRange);
      Serial.print("  oilMin=");
      Serial.print(oilMin);
      Serial.print("  xAxis_=");
      Serial.print(xAxis_);
      Serial.print("  xPrev_=");
      Serial.print(xPrev);
      Serial.print("  xMin=");
      Serial.print(xMin);
      Serial.print("  xMax=");
      Serial.print(xMax);
      Serial.print("  calcJoyVal=");
      Serial.println(calcJoyVal);
    #endif
    
  }
  delay(50); //50ms (20 updates per second)
}
