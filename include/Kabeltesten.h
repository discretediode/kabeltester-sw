#include <Arduino.h>
#include "abspeichern.h"
#include "MUX.h"
#include "ADC.h"
//MUX ADC L
#define ADC_L_S0 26
#define ADC_L_S1 27
#define ADC_L_S2 14
#define ADC_L_S3 12
#define ADC_L_E  33
// MUX ADC R
#define ADC_R_S0 26
#define ADC_R_S1 27
#define ADC_R_S2 14
#define ADC_R_S3 12
#define ADC_R_E  25

// MUX PWR L
#define PWR_L_S0 04
#define PWR_L_S1 00
#define PWR_L_S2 02
#define PWR_L_S3 15
#define PWR_L_E  17
// MUX PWR R
#define PWR_R_S0 04
#define PWR_R_S1 00
#define PWR_R_S2 02
#define PWR_R_S3 15
#define PWR_R_E  16 

// MUX PWR2 L
#define PWR2_L_S0 04 
#define PWR2_L_S1 00
#define PWR2_L_S2 02 
#define PWR2_L_S3 15
#define PWR2_L_E  255  // enable pins anpassen
// MUX PWR2 R
#define PWR2_R_S0 04
#define PWR2_R_S1 00
#define PWR2_R_S2 02
#define PWR2_R_S3 15
#define PWR2_R_E  255  // enable pins anpassen

//MUX ADC2 L
#define ADC2_L_S0 26
#define ADC2_L_S1 27
#define ADC2_L_S2 14
#define ADC2_L_S3 12
#define ADC2_L_E  255  // enable pins anpassen
// MUX ADC2 R
#define ADC2_R_S0 26 
#define ADC2_R_S1 27
#define ADC2_R_S2 14
#define ADC2_R_S3 12
#define ADC2_R_E  255  // enable pins anpassen




// put function declarations here:
void MUX_ini();
void Kabel_End2End(uint8_t PinAnzahl);
void Kabel_SteckerL(uint8_t PinAnzahl);
void Kabel_SteckerR(uint8_t PinAnzahl);

//MUX Pins definieren
MUX muxADC_L(ADC_L_S0, ADC_L_S1, ADC_L_S2, ADC_L_S3, ADC_L_E);
MUX muxADC_R(ADC_R_S0, ADC_R_S1, ADC_R_S2, ADC_R_S3, ADC_R_E);
MUX muxPWR_L(PWR_L_S0, PWR_L_S1, PWR_L_S2, PWR_L_S3, PWR_L_E);
MUX muxPWR_R(PWR_R_S0, PWR_R_S1, PWR_R_S2, PWR_R_S3, PWR_R_E);
MUX muxPWR2_L(PWR2_L_S0, PWR2_L_S1, PWR2_L_S2, PWR2_L_S3, PWR2_L_E);
MUX muxPWR2_R(PWR2_R_S0, PWR2_R_S1, PWR2_R_S2, PWR2_R_S3, PWR2_R_E);
MUX muxADC2_L(ADC2_L_S0, ADC2_L_S1, ADC2_L_S2, ADC2_L_S3, ADC2_L_E);
MUX muxADC2_R(ADC2_R_S0, ADC2_R_S1, ADC2_R_S2, ADC2_R_S3, ADC2_R_E);


// MUX auf ausgangsstellung setzen
void MUX_ini(){
  muxADC_L.ini();
  muxADC_R.ini();
  muxPWR_L.ini();
  muxPWR_R.ini();
  muxPWR2_L.ini();
  muxPWR2_R.ini();
  muxADC2_L.ini();
  muxADC2_R.ini();
}


void Kabel_End2End(uint8_t PinAnzahl){
  for(uint8_t i=0; i<PinAnzahl ; i++) {   //32 mit Variable erstezen eingeabe der pins 
    if(i<16) {
      muxADC_L.enable();
      muxADC_R.disable();
      muxADC_L.setChannel(i);
    }
    else{
      muxADC_L.disable();
      muxADC_R.enable();
      muxADC_R.setChannel(i-16);
    } 
    for(uint8_t j=i; j<PinAnzahl ; j++) {   //32 mit Variable erstezen eingeabe der pins 
      if(j<16) {
      muxPWR_L.enable();
      muxPWR_R.disable();
      muxPWR_L.setChannel(j);
      }
      else{
      muxPWR_L.disable();
      muxPWR_R.enable();
      muxPWR_R.setChannel(j-16);
      //ADC messen Rechnen Klassifizieren 
      //......
      } 

    }
  }


}
void Kabel_SteckerL(uint8_t PinAnzahl){
  for(uint8_t i=0; i<PinAnzahl ; i++) {   //32 mit Variable erstezen eingeabe der pins 
    if(i<16) {
      muxADC_L.enable();
      muxADC_R.disable();
      muxADC_L.setChannel(i);
    }
    else{
      muxADC_L.disable();
      muxADC_R.enable();
      muxADC_R.setChannel(i-16);
    } 
    for(uint8_t j=i; j<PinAnzahl ; j++) {   //32 mit Variable erstezen eingeabe der pins 
      if(j<16) {
      muxPWR2_L.enable();
      muxPWR2_R.disable();
      muxPWR2_L.setChannel(j);
      }
      else{
      muxPWR2_L.disable();
      muxPWR2_R.enable();
      muxPWR2_R.setChannel(j-16);
      //ADC messen Rechnen Klassifizieren 
      //......
      } 
    }
  }
}

void Kabel_SteckerR(uint8_t PinAnzahl){
  for(uint8_t i=0; i<PinAnzahl ; i++) {   //32 mit Variable erstezen eingeabe der pins 
    if(i<16) {
      muxADC2_L.enable();
      muxADC2_R.disable();
      muxADC2_L.setChannel(i);
    }
    else{
      muxADC2_L.disable();
      muxADC2_R.enable();
      muxADC2_R.setChannel(i-16);
    } 
    for(uint8_t j=i; j<PinAnzahl ; j++) {   //32 mit Variable erstezen eingeabe der pins 
      if(j<16) {
      muxPWR_L.enable();
      muxPWR_R.disable();
      muxPWR_L.setChannel(j);
      }
      else{
      muxPWR_L.disable();
      muxPWR_R.enable();
      muxPWR_R.setChannel(j-16);
      //ADC messen Rechnen Klassifizieren 
      //......
      } 
    }
  }
}

