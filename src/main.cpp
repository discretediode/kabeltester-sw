#include <Arduino.h>
#include <list>
#include "abspeichern.h"

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(3, 3);
  int anzahl;
  int modus;
  std::list<int> pinnummer;
  std::list<int> widerstand;
  std::list<char> einordnung;
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}