#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include<iostream>
#include<string.h>

const int chipSelect = 31;

void parameterspeichern(int anzahl, std::string in_out, std::string in_in) {

    SD.begin(chipSelect);

    int N = 1;
    char filename[13];

    // Dateinamen generieren (z. B. "data1.txt")
    sprintf(filename, "data%d.txt", N);

    // Schleife läuft, solange die Datei bereits existiert
    while (SD.exists(filename)) {
        N++;
        sprintf(filename, "data%d.txt", N);
    }

    File dataFile = SD.open(filename, FILE_WRITE);

    dataFile.print("Pin_IN|Pin_Out|Pin_In|Pin_Out|Widerstand|Kategorie\n");

    for(int r = 0; r < anzahl; r++){

        for(int c = 0; c < anzahl; c++){

            if(in_in[r][c] < 10){
                dataFile.print("");
            }
            
        }

    }


    dataFile.close();

}