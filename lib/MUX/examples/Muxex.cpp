
#include <Arduino.h>
#include "MUX.h"

//Multiplexer eingänge definieren
MUX mux(2, 3, 4, 5, 10);


void setup() {

    //Multiplexer einstellen 
    mux.ini();

}

void loop() {

    for (uint8_t i=0; i<16; i++){
        //Multiplexer aktiviern 
        mux.enable();
        //Channel wählen
        mux.setChannel(i);

    }
    //Multiplexer ausschalten 
    mux.disable();
}