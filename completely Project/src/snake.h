#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>
#include <LedControl.h>

// Pins für die LED-Matrix
const int DIN_PIN = 13;
const int CS_PIN = 12;
const int CLK_PIN = 11;

// JoyStick Pins
const int xAxis = A0;
const int yAxis = A1;
const int button = 10;

// Initialisieren des LedControl-Objekts
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1); // 1 steht für eine angeschlossene LED-Matrix

// Bitmuster für die Ziffern 0 bis 9
byte digits[10][8] = {
  {B00000000, B00111100, B01000010, B01000010, B01000010, B01000010, B00111100, B00000000}, // 0
  {B00000000, B00000000, B00000000, B01000100, B01111110, B01000000, B00000000, B00000000}, // 1
  {B00000000, B00000100, B10000010, B11000010, B10100010, B10010010, B10001100, B00000000}, // 2
  {B00000000, B01000100, B10000010, B10010010, B10010010, B10010010, B01101100, B00000000}, // 3
  {B00000000, B00100000, B00110000, B00101000, B00100100, B11111110, B00100000, B00000000}, // 4
  {B00000000, B10011110, B10010010, B10010010, B10010010, B10010010, B11110010, B00000000}, // 5
  {B00000000, B00111110, B01001001, B01001001, B01001001, B01001001, B00110001, B00000000}, // 6
  {B00000000, B00000010, B00000010, B11100010, B00010010, B00001010, B00000110, B00000000}, // 7
  {B00000000, B01101100, B10010010, B10010010, B10010010, B10010010, B01101100, B00000000}, // 8
  {B00000000, B10001100, B10010010, B10010010, B10010010, B10010010, B01111100, B00000000},  // 9
};

const int maxLength = 64; // Maximale Länge der Schlange (8x8 Matrix)
int snakeX[maxLength];
int snakeY[maxLength];
int snakeLength = 1;

int xPlayerPosition = 4;
int yPlayerPosition = 4;
int previousY = yPlayerPosition;
int previousX = xPlayerPosition;
bool cannPlayerTakeBarry = true;

unsigned long previousMillis = 0;
const long interval = 1500; // Intervall für die Barry-Funktion
int barryState = 0;
int barryX, barryY;

// Globale Variablen für Positionen
int BarryPosition;
int PlayerPosition;

bool GameOver = false;

// Variable für den Score
int score = 0;

void setupSnake() {
  pinMode(button, INPUT_PULLUP);
  // Initialisieren der LED-Matrix
  lc.shutdown(0, false);       // Wake up displays
  lc.setIntensity(0, 1);       // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);          // Clear display register
  lc.setLed(0, xPlayerPosition, yPlayerPosition, true);    // Set initial player position

  // Initialisiere die Schlange
  snakeX[0] = xPlayerPosition;
  snakeY[0] = yPlayerPosition;
}

// Funktion für die Auswertung des Joysticks
void joyStick() {
  // Y-Achse auswerten
  int yValue = analogRead(yAxis);
  int xValue = analogRead(xAxis);
  previousY = yPlayerPosition;
  previousX = xPlayerPosition;

  // Bewegung nur auf einer Achse zur gleichen Zeit erlauben
  if (abs(yValue - 512) > abs(xValue - 512)) {
    // Y-Achse Bewegung
    if (yValue > 550) {
      yPlayerPosition--;
    } else if (yValue < 470) {
      yPlayerPosition++;
    }
  } else {
    // X-Achse Bewegung
    if (xValue > 550) {
      xPlayerPosition--;
    } else if (xValue < 470) {
      xPlayerPosition++;
    }
  }

  // Clamping: Begrenze die Positionen auf den gültigen Bereich (0 bis 7)
  if (xPlayerPosition < 0) {
    xPlayerPosition = 0;
  } else if (xPlayerPosition > 7) {
    xPlayerPosition = 7;
  }

  if (yPlayerPosition < 0) {
    yPlayerPosition = 0;
  } else if (yPlayerPosition > 7) {
    yPlayerPosition = 7;
  }
}

// Funktion für die Bewegung von Barry
void Barry() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (barryState == 0) {
      barryX = random(0, 8);
      barryY = random(0, 8);
      lc.setLed(0, barryX, barryY, true);
      cannPlayerTakeBarry = true;
      barryState = 1;
    } else {
      lc.setLed(0, barryX, barryY, false);
      barryX = random(0, 8);
      barryY = random(0, 8);
      lc.setLed(0, barryX, barryY, true);
      barryState = 0;
    }
  }
}

void Player() {
  BarryPosition = barryX + barryY * 8;
  PlayerPosition = xPlayerPosition + yPlayerPosition * 8;

  if (BarryPosition == PlayerPosition && cannPlayerTakeBarry) {
    cannPlayerTakeBarry = false;
    score++;
    snakeLength++;
    Serial.println(score);
  }

  // Nur aktualisieren, wenn sich der Kopf bewegt hat:
  if (snakeX[0] != xPlayerPosition || snakeY[0] != yPlayerPosition) {
    // Aktualisiere die Position der Schlange
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }
    snakeX[0] = xPlayerPosition;
    snakeY[0] = yPlayerPosition;
  }

  // Zeichne die Schlange auf der LED-Matrix
  lc.clearDisplay(0);
  for (int i = 0; i < snakeLength; i++) {
    lc.setLed(0, snakeX[i], snakeY[i], true);
  }
  // Zeichne die Beere erneut
  lc.setLed(0, barryX, barryY, true);

  // Überprüfe NUR, ob die Schlange in sich selbst läuft
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      GameOver = true;
      Serial.println("Game Over - Schlange hat sich selbst berührt");
    }
  }
}

void startSnake() {
  setupSnake(); // Initialisiere das Spiel nur einmal
  while (!GameOver) {
    joyStick();
    Player();
    Barry();
    delay(100);
  }
  // Nach Game Over:
  lc.clearDisplay(0);
  
  // Wenn der Score einstellig ist, zeige ihn direkt an:
  if (score < 10) {
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, digits[score][i]);
    }
  } 
  // Bei einem zweistelligen Score: erst den Zehner, dann den Einer anzeigen.
  else {
    int tens = score / 10;
    int ones = score % 10;
    
    // Zeige zuerst den Zehner:
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, digits[tens][i]);
    }
    delay(2000); // Zeige den Zehner etwa 2 Sekunden lang
    
    // Anschließend den Einer:
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, digits[ones][i]);
    }
  }
  
  // Button-Druck zum Reset:
  while (GameOver) {
    if (digitalRead(button) == LOW) {
      lc.clearDisplay(0);
      score = 0;
      snakeLength = 1;
      GameOver = false;
      Serial.println("Game reset");
    }
    Serial.println(score);
    delay(100);
  }
}

#endif
