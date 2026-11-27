#include <Arduino.h>
#include "MUX.h"
#include "ADC.h"

//MUX enable Pins
#define ADC_L1_E 32
#define ADC_L2_E 33
#define ADC_R1_E 10
#define ADC_R2_E 11

#define PWR_R1_E 1
#define PWR_R2_E 3
#define PWR_L1_E 17
#define PWR_L2_E 16

//MUX select Pins
#define ADC_S0 26
#define ADC_S1 27
#define ADC_S2 14
#define ADC_S3 12

#define PWR_S0 4
#define PWR_S1 0
#define PWR_S2 2
#define PWR_S3 15

//ADC
#define ADC_RDY 13
#define I2C_SCL 22
#define I2C_SDA 21



/*
//MUX ADC L
#define ADC_L1_E 26
#define ADC_L_S1 27
#define ADC_L_S2 14
#define ADC_L_S3 12
#define ADC_L_E  32

// MUX ADC R
#define ADC_R_S0 26
#define ADC_R_S1 27
#define ADC_R_S2 14
#define ADC_R_S3 12
#define ADC_R_E  25

// MUX PWR L
#define PWR_L_S0 04 // vielleich 4, wegen Oktalzahl, besser 4 in dezimal schreiben
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
*/



// Bekannter Widerstand und Versorgungsspannung für Spannungsteiler definieren
#define R_KNOWN 10000.0  // 10k Ohm
#define V_SUPPLY 5.0     // Versorgungsspannung in Volt


// put function declarations here:
void Kabel_ini();
void Kabel_End2End(uint8_t PinAnzahl);
void Kabel_SteckerL(uint8_t PinAnzahl);
void Kabel_SteckerR(uint8_t PinAnzahl);
float Spannungsteiler(float U_mess);
const char* Klassifizierung(float R_Kabel, uint8_t PinAnzahl);
void matrix_scanner(uint8_t PinAnzahl, uint8_t choice);
void test(int i);


//MUX Pins definieren
MUX muxADC_L1(ADC_S0, ADC_S1, ADC_S2, ADC_S3, ADC_L1_E);
MUX muxADC_L2(ADC_S0, ADC_S1, ADC_S2, ADC_S3, ADC_L2_E);
MUX muxPWR_L1(PWR_S0, PWR_S1, PWR_S2, PWR_S3, PWR_L1_E);
MUX muxPWR_L2(PWR_S0, PWR_S1, PWR_S2, PWR_S3, PWR_L2_E);
MUX muxPWR_R1(PWR_S0, PWR_S1, PWR_S2, PWR_S3, PWR_R1_E);
MUX muxPWR_R2(PWR_S0, PWR_S1, PWR_S2, PWR_S3, PWR_R2_E);
MUX muxADC_R1(ADC_S0, ADC_S1, ADC_S2, ADC_S3, ADC_R1_E);
MUX muxADC_R2(ADC_S0, ADC_S1, ADC_S2, ADC_S3, ADC_R2_E);

// ADC definieren
ADC adc(0x48);


// MUX auf ausgangsstellung setzen
void Kabel_ini(){
  muxADC_L1.ini();
  muxADC_L2.ini();
  muxPWR_L1.ini();
  muxPWR_L2.ini();
  muxPWR_R1.ini();
  muxPWR_R2.ini();
  muxADC_R1.ini();
  muxADC_R2.ini();
  adc.begin(I2C_SDA, I2C_SCL, ADC_RDY);
}


void Kabel_End2End(uint8_t PinAnzahl){
  for(uint8_t i=0; i<PinAnzahl ; i++) {   //32 mit Variable erstezen eingeabe der pins 
    if(i<16) {
      muxADC_L1.enable();
      muxADC_L2.disable();
      muxADC_L1.setChannel(i);
    }
    else{
      muxADC_L1.disable();
      muxADC_L2.enable();
      muxADC_L2.setChannel(i-16);
    } 
    for(uint8_t j=0; j<PinAnzahl ; j++) {   //32 mit Variable erstezen eingeabe der pins 
      if(j<16) {
      muxPWR_L1.enable();
      muxPWR_L2.disable();
      muxPWR_L1.setChannel(j);
      }
      else if(j>=16) {
      muxPWR_L1.disable();
      muxPWR_L2.enable();
      muxPWR_L2.setChannel(j-16);
      } 
      //ADC messen Rechnen Klassifizieren 
      float voltE2E  = adc.readVolt(CH_A0); //wo wird der Kanal definiert???
      float U_mess  = adc.readVolt(CH_A0);
      //......

    }
  }


}
void Kabel_SteckerL(uint8_t PinAnzahl){
  for(uint8_t i=0; i<PinAnzahl ; i++) {   //32 mit Variable erstezen eingeabe der pins 
    if(i<16) {
      muxADC_L1.enable();
      muxADC_L2.disable();
      muxADC_L1.setChannel(i);
    }
    else{
      muxADC_L1.disable();
      muxADC_L2.enable();
      muxADC_L2.setChannel(i-16);
    } 
    for(uint8_t j=i; j<PinAnzahl ; j++) {   //32 mit Variable erstezen eingeabe der pins 
      if(j<16) {
      muxPWR_R1.enable();
      muxPWR_R2.disable();
      muxPWR_R1.setChannel(j);
      }
      else{
      muxPWR_R1.disable();
      muxPWR_R2.enable();
      muxPWR_R2.setChannel(j-16);
      //ADC messen Rechnen Klassifizieren 
      float voltSTL  = adc.readVolt(CH_A0);
      float U_mess  = adc.readVolt(CH_A0);
      //......
      } 
    }
  }
}

void Kabel_SteckerR(uint8_t PinAnzahl){
  for(uint8_t i=0; i<PinAnzahl ; i++) {   //32 mit Variable erstezen eingeabe der pins 
    if(i<16) {
      muxADC_R1.enable();
      muxADC_R2.disable();
      muxADC_R1.setChannel(i);
    }
    else{
      muxADC_R1.disable();
      muxADC_R2.enable();
      muxADC_R2.setChannel(i-16);
    } 
    for(uint8_t j=i; j<PinAnzahl ; j++) {   //32 mit Variable erstezen eingeabe der pins 
      if(j<16) {
      muxPWR_R1.enable();
      muxPWR_R2.disable();
      muxPWR_R1.setChannel(j);
      }
      else{
      muxPWR_R1.disable();
      muxPWR_R2.enable();
      muxPWR_R2.setChannel(j-16);
      //ADC messen Rechnen Klassifizieren 
      float voltStR  = adc.readVolt(CH_A1);
      float U_mess  = adc.readVolt(CH_A1);
      //......
      } 
    }
  }
}

float Spannungsteiler(float U_mess) 
{
  float I_bekannt = U_mess / R_KNOWN; // Strom durch den bekannten Widerstand berechnen
  float R_Kabel = (V_SUPPLY - U_mess) / I_bekannt; // Widerstand des Kabels berechnen
  return R_Kabel;
}

const char* Klassifizierung(float R_Kabel)
{
   if (R_Kabel <= 1.0 )
    {
      return "Leitend";
    }
    else if (R_Kabel > 1.0 && R_Kabel <= 1000.0)
    {
      return "Schluss";
    }
    else if (R_Kabel > 1000.0 && R_Kabel <= 100000.0)
    {
      return "Hochohmig";
    }
    else if (R_Kabel > 100000.0)
    {
      return "Isolierend";
    }
    else
    {
      return "unbekannt";
    }
}

void matrix_scanner(uint8_t PinAnzahl, uint8_t choice)
{
  switch (choice) {
    case 1:
      // Implementierung für die erste Option
      Kabel_End2End(PinAnzahl);
      break;
    case 2:
      // Implementierung für die zweite Option
      Kabel_SteckerL(PinAnzahl);
      break;
    case 3:
      // Implementierung für die dritte Option
      Kabel_SteckerR(PinAnzahl);
      break;
    default:
      // Standardaktion oder Fehlerbehandlung
      Serial.println("Ungültige Wahl. Bitte wählen Sie 1, 2 oder 3.");
      break;
  }
}


  /*for (uint8_t tx_pin = 0; tx_pin < PinAnzahl; tx_pin++) 
  {
    // Funktion für Versorgung des tx_pin implementieren
    if(tx_pin<16)
    {
      muxADC2_L.enable();
      muxADC2_R.disable();
      muxADC2_L.setChannel(tx_pin);
    }
    else
    {
      muxADC2_L.disable();
      muxADC2_R.enable();
      muxADC2_R.setChannel(tx_pin-16);
    }
    for (uint8_t rx_pin = 0; rx_pin < PinAnzahl; rx_pin++) 
    {
      // Funktion für Messung des rx_pin implementieren
      if(rx_pin<16)
      {
        muxPWR_L.enable();
        muxPWR_R.disable();
        muxPWR_L.setChannel(rx_pin);
      }
      else
      {
        muxPWR_L.disable();
        muxPWR_R.enable();
        muxPWR_R.setChannel(rx_pin-16);
      }
    }
  }*/



void test(int i)
{
   muxADC_L1.enable();
   muxADC_L2.disable();
   muxADC_L1.setChannel(i);

}

