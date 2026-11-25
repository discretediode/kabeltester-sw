#include <Arduino.h>
#include<string>
#include "abspeichern.h"
#include <vector>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(3, 3);
  int anzahl;
  int modus;
  std::vector<std::vector<int>> in_out(16, std::vector<int>(16));
  std::vector<std::vector<int>> in_in;
  
  for (int i = 7; i >= 1; --i) {
    in_in.push_back(std::vector<int>(i)); // Jede Zeile bekommt genau 'i' Plätze
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}