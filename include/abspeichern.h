#include <Arduino.h>
#include <SD.h>

const int chipSelect = 4;

void parameterspeichern(int pinnummer, float widerstand, char kategorie) {

    SD.begin(chipSelect);

    File dataFile = SD.open("/daten.txt", FILE_WRITE);

    dataFile.print(pinnummer);
    dataFile.print(";");
    dataFile.print(widerstand);
    dataFile.print(";");
    dataFile.println(kategorie);

    dataFile.close();

}