#include <Arduino.h>
#include <SD.h>

const int chipSelect = 31;

void parameterspeichern(int anzahl, int modus) {

    SD.begin(chipSelect);

    File dataFile = SD.open("/daten.txt", FILE_WRITE);

    //dataFile.print(pinnummer);
    //dataFile.print(";");
    //dataFile.print(widerstand);
    //dataFile.print(";");
    //dataFile.print(kategorie);
    //dataFile.print("\n\n");

    dataFile.close();

}