#include <Arduino.h>
#include "abspeichern.h"
#include "MUX.h"
#include "ADC.h"

// put function declarations here:
void Kabel_End2End();
void Kabel_Stecker();

void setup() {
  MUX muxADC_L(26, 27, 14, 12, 33);
  MUX muxADC_R(26, 27, 14, 12, 25);
  MUX muxPWR_L(4, 0, 2, 15, 17);
  MUX muxPWR_R(4, 0, 2, 15, 16);
  MUX muxPWR2_L(4, 0, 2, 15, 17);// enable pins anpassen 
  MUX muxPWR2_R(4, 0, 2, 15, 16);// enable pins anpassen 
  muxADC_L.ini();
  muxADC_R.ini();
  muxPWR_L.ini();
  muxPWR_R.ini();


}

void loop() {
  // put your main code here, to run repeatedly:
}

void Kabel_End2End(){
  for(uint8_t i=0; i<32 ; i++) {   //32 mit Variable erstezen eingeabe der pins 
    if(i<16) {
      muxADC_L.enable();
      muxADC_R.disable();
      muxADC_L.setChannel(i);
    }
    else{
      muxADC_L.disable();
      muxADC_R.enable();
      muxADC_R.setChannel(i-15);
    } 
    for(uint8_t j=i; j<32 ; j++) {   //32 mit Variable erstezen eingeabe der pins 
      if(j<16) {
      muxPWR_L.enable();
      muxPWR_R.disable();
      muxPWR_L.setChannel(j);
      }
      else{
      muxPWR_L.disable();
      muxPWR_R.enable();
      muxPWR_R.setChannel(j-15);
      //ADC messen Rechnen Klassifizieren 
      //......
      } 

    }
  }


}
void Kabel_Stecker(){
  for(uint8_t i=0; i<32 ; i++) {   //32 mit Variable erstezen eingeabe der pins 
    if(i<16) {
      muxADC_L.enable();
      muxADC_R.disable();
      muxADC_L.setChannel(i);
    }
    else{
      muxADC_L.disable();
      muxADC_R.enable();
      muxADC_R.setChannel(i-15);
    } 
    for(uint8_t j=i; j<32 ; j++) {   //32 mit Variable erstezen eingeabe der pins 
      if(j<16) {
      muxPWR2_L.enable();
      muxPWR2_R.disable();
      muxPWR2_L.setChannel(j);
      }
      else{
      muxPWR2_L.disable();
      muxPWR2_R.enable();
      muxPWR2_R.setChannel(j-15);
      //ADC messen Rechnen Klassifizieren 
      //......
      } 

    }
  }



}
