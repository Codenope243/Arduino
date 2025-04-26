#ifndef ticTacToe
#define ticTacToe
#include "config.h"

byte symbolX[2][2] = {
  {1, 0},
  {0, 1}
};

byte symbolO[2][2] = {
  {1, 1},
  {1, 1}
};

int tttState[3][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};


// Zeichnet das Tic Tac Toe Spielfeld anhand eines festen Binärcodes
void drawTicTacToeBoard() {
  // Definition des 8x8 Musters:
  // Zeilen 2 und 5: komplett an (Raster horizontal)
  // Alle anderen Zeilen: nur Spalte 2 und 5 an (Raster vertikal)
  byte board[8] = {
    B00100100,  // Zeile 0: nur Spalten 2 und 5
    B00100100,  // Zeile 1: nur Spalten 2 und 5
    B11111111,  // Zeile 2: komplette Linie (horizontal)
    B00100100,  // Zeile 3: nur Spalten 2 und 5
    B00100100,  // Zeile 4: nur Spalten 2 und 5
    B11111111,  // Zeile 5: komplette Linie (horizontal)
    B00100100,  // Zeile 6: nur Spalten 2 und 5
    B00100100   // Zeile 7: nur Spalten 2 und 5
  };
  
  for (int r = 0; r < 8; r++) {
    lc.setRow(0, r, board[r]);
  }
}

// Funktion zum Zeichnen des Symbols "X", "O" auf der LED-Matrix
// (xOffset, yOffset) bestimmen die obere linke Ecke des Symbols
void drawSymbolX(int xOffset, int yOffset) {
  for (int r = 0; r < 2; r++) {
    for (int c = 0; c < 2; c++) {
      int posRow = yOffset + r;
      int posCol = xOffset + c;
      // Prüfe, ob die Position innerhalb der 8x8 Matrix liegt
      if (posRow >= 0 && posRow < 8 && posCol >= 0 && posCol < 8) {
        // Schalte LED an, wenn der Wert 1 ist, sonst aus:
        lc.setLed(0, posRow, posCol, symbolX[r][c]);
      }
    }
  }
}

void drawSymbolO(int xOffset, int yOffset) {
  for (int r = 0; r < 2; r++) {
    for (int c = 0; c < 2; c++) {
      int posRow = yOffset + r;
      int posCol = xOffset + c;
      // Prüfe, ob die Position innerhalb der 8x8 Matrix liegt
      if (posRow >= 0 && posRow < 8 && posCol >= 0 && posCol < 8) {
        // Schalte LED an, wenn der Wert 1 ist, sonst aus:
        lc.setLed(0, posRow, posCol, symbolO[r][c]);
      }
    }
  }
}

int cellOffsets[3] = {0, 3, 6};
void drawTTTState() {
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
      int xOffset = cellOffsets[j];
      int yOffset = cellOffsets[i];
      if(tttState[i][j] == 1) {
         drawSymbolX(xOffset, yOffset);
      } else if(tttState[i][j] == 2) {
         drawSymbolO(xOffset, yOffset);
      }
    }
  }
}

void updateDisplayWithTTTState(int movingXCell, int movingYCell, bool isX) {
  drawTicTacToeBoard();
  drawTTTState();

  int xOff = cellOffsets[movingXCell];
  int yOff = cellOffsets[movingYCell];
  if(isX) {
    drawSymbolX(xOff, yOff);
  } else {
    drawSymbolO(xOff, yOff);
  }
}



// Globale Variablen für die aktuelle bewegte Position (Zellkoordinaten: 0 bis 2)
int movingXCell = 0;
int movingYCell = 0;
bool currentIsX = true;  // Gibt an, welches Symbol aktuell bewegt wird

void joystick(){
  int xValue = analogRead(xAxis);
  int yValue = analogRead(yAxis);
  Serial.println(xValue);
  Serial.println(yValue);

  // x-Achse steuert die vertikale Bewegung; alle Richtungen umgekehrt:
  if(xValue < 500){       // war "Oben", jetzt "Unten"
    Serial.println("Unten");
    if(movingYCell < 2){
      movingYCell++;
    }
  } else if(xValue > 520){ // war "Unten", jetzt "Oben"
    Serial.println("Oben");
    if(movingYCell > 0){
      movingYCell--;
    }
  }

  // y-Achse steuert die horizontale Bewegung; auch hier umgekehrt:
  if(yValue < 470){       // war "Links", jetzt "Rechts"
    Serial.println("Rechts");
    if(movingXCell < 2){
      movingXCell++;
    }
  } else if(yValue > 550){ // war "Rechts", jetzt "Links"
    Serial.println("Links");
    if(movingXCell > 0){
      movingXCell--;
    }
  }
  
  // Angenommen, der Spieler drückt einen Button, um seinen Zug zu bestätigen:
  if(digitalRead(button) == LOW) {
    // Prüfen, ob die Zelle noch frei ist:
    if(tttState[movingYCell][movingXCell] == 0) {
      if(currentIsX) {
        tttState[movingYCell][movingXCell] = 1;  // Setzt ein "X"
      } else {
        tttState[movingYCell][movingXCell] = 2;  // Setzt ein "O"
      }
      // Wechsle den Spieler, falls gewünscht
      currentIsX = !currentIsX;
    }
    // Kurze Wartezeit für Entprellung:
    delay(300);
  }

  // Aktualisiere das Display: Raster, fixierte Symbole und das aktuell bewegte Symbol neu zeichnen
  updateDisplayWithTTTState(movingXCell, movingYCell, currentIsX);
};

bool win = false;
bool Draw = false;

// Globale Variable, um die gewonnene Zeile zu speichern (bei einem horizontalen Gewinn)
int winningRow = -1;

void winCheck(){
  // Überprüfe alle Zeilen (horizontaler Gewinn)
  for(int i = 0; i < 3; i++){
    if(tttState[i][0] == tttState[i][1] && tttState[i][1] == tttState[i][2] && tttState[i][0] != 0){
      Serial.println("Gewonnen (Zeile)");
      winningRow = i;  // Speicher die gewonnene Zeile
      win = true;
      return;
    }
  }
  // Überprüfe alle Spalten
  for(int i = 0; i < 3; i++){
    if(tttState[0][i] == tttState[1][i] && tttState[1][i] == tttState[2][i] && tttState[0][i] != 0){
      Serial.println("Gewonnen (Spalte)");
      win = true;
      return;
    }
  }
  // Überprüfe Diagonalen
  if(tttState[0][0] == tttState[1][1] && tttState[1][1] == tttState[2][2] && tttState[0][0] != 0){
    Serial.println("Gewonnen (Diagonale)");
    win = true;
    return;
  }
  if(tttState[0][2] == tttState[1][1] && tttState[1][1] == tttState[2][0] && tttState[0][2] != 0){
    Serial.println("Gewonnen (Diagonale)");
    win = true;
    return;
  }
  // Überprüfe auf Unentschieden
  bool isDraw = true;
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      if(tttState[i][j] == 0){
        isDraw = false;
      }
    }
  }
  if(isDraw){
    Serial.println("Unentschieden");
    Draw = true;
  }
}

// Funktion, die die gewonnene Zeile blinken lässt
void blinkWinningRow(int row) {
  int yOff = cellOffsets[row];
  // Annahme: Jede Zelle hat eine Höhe von 2 Zeilen (siehe drawSymbolX bzw. drawSymbolO)
  for (int i = 0; i < 5; i++) {  // Blinke 5-mal
    // Lösche den Bereich der gewonnenen Zeile:
    for (int r = yOff; r < yOff + 2; r++) {
      for (int c = 0; c < 8; c++) {
        lc.setLed(0, r, c, false);
      }
    }
    delay(300);
    // Zeichne die Symbole der gewonnenen Zeile erneut:
    for (int j = 0; j < 3; j++){
      int xOff = cellOffsets[j];
      if(tttState[row][j] == 1) {
         drawSymbolX(xOff, yOff);
      } else if(tttState[row][j] == 2) {
         drawSymbolO(xOff, yOff);
      }
    }
    delay(300);
  }
}

// Funktion, die alle Spielfeldvariablen zurücksetzt
void resetTicTacToeState() {

  // Leere das Spielfeld (tttState)
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
      tttState[i][j] = 0;
    }
  }
  // Setze die Bewegungsvariablen zurück
  movingXCell = 0;
  movingYCell = 0;
  // Setze den Spieler zurück (zum Beispiel X beginnt)
  currentIsX = true;
  // Setze den Gewinn- und Unentschiedenstatus zurück
  win = false;
  Draw = false;
  // Setze winningRow zurück
  winningRow = -1;
};

void startTicTacToe() {
  Serial.println("Tic Tac Toe gestartet");
  resetTicTacToeState();
  drawTicTacToeBoard();
  drawSymbolX(0, 0);
  // Platzhalter-Schleife, damit das Board sichtbar bleibt.
  while(!win && !Draw) {
    joystick();
    winCheck();
    delay(300);
  }

  // Spielende: Gewonnen oder Unentschieden
  if(win){
    win = false;
    Serial.println("Spiel beendet - Gewonnen");
    // Falls ein horizontaler Gewinn vorliegt, blinke die Reihe:
    blinkWinningRow(winningRow);
    delay(2000);  // Kurze Pause, bevor das Spielfeld zurückgesetzt wird
    lc.clearDisplay(0);
  } else if(Draw){
    Draw = false;
    Serial.println("Spiel beendet - Unentschieden");
  }
}

#endif