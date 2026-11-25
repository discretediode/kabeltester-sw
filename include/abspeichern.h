#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include<iostream>
#include<string.h>
#include<vector>

const int chipSelect = 31;

void parameterspeichern(int anzahl, const std::vector<std::vector<int>>& in_out, const std::vector<std::vector<int>>& in_in) {

    SD.begin(chipSelect);

    int N = 1;
    char filename[13];

    // Dateinamen generieren
    sprintf(filename, "data%d.txt", N);

    // Schleife läuft, solange die Datei bereits existiert
    while (SD.exists(filename)) {
        N++;
        sprintf(filename, "data%d.txt", N);
    }

    File dataFile = SD.open(filename, FILE_WRITE);

    dataFile.print("Pin_In|Pin_Out|Pin_In|Pin_Out|Widerstand in Ohm|Kategorie\n");

    for(int r = 0; r < anzahl; r++){

        for(int c = 0; c < anzahl; c++){

            if(in_in[r][c] < 10){
                dataFile.printf("%d|/|%d|/|%d|Leitend", r, c, in_in[r][c]);
            } else if(10 < in_in[r][c] < 100){
                dataFile.printf("%d|/|%d|/|%d|Widerstand", r, c, in_in[r][c]);
            }else if(100 < in_in[r][c] < 10000){
                dataFile.printf("%d|/|%d|/|%d|Kritisch", r, c, in_in[r][c]);
            }else if(10000 < in_in[r][c]){
                dataFile.printf("%d|/|%d|/|%d|Hochohmig", r, c, in_in[r][c]);
            }else dataFile.printf("%d|/|%d|/|/|/", r, c);
            
        }

    }


    dataFile.close();

}