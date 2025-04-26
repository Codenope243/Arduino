#ifndef CONFIG_H
#define CONFIG_H

// Pins für die LED-Matrix
const int DIN_PIN = 13;
const int CS_PIN = 12;
const int CLK_PIN = 11;

// JoyStick Pins
const int xAxis = A0;
const int yAxis = A1;
const int button = 10;

int score = 0;

// Erstelle ein globales LedControl-Objekt
#include <LedControl.h>
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1); // 1 steht für eine angeschlossene LED-Matrix

#endif