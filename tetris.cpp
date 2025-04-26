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

int score = 0;

// Erstelle ein globales LedControl-Objekt
#include <LedControl.h>
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1); // 1 steht für eine angeschlossene LED-Matrix

// externes Digits-Array zur Score-Anzeige (wie in Snake.h definiert)
extern byte digits[10][8];

// Definiere Spielfeldgröße (entspricht der 8×8 LED-Matrix)
const int ROWS = 8;
const int COLS = 8;
int grid[ROWS][COLS]; // 0: leer, 1: besetzt

// Es gibt jetzt 7 Tetrominoe: O, I, T, S, Z, J, L

const int NUM_PIECES = 7;

// O Piece (Quadrat) – 4x4-Array
byte pieceO[16] = {
  0,0,0,0,
  0,1,1,0,
  0,1,1,0,
  0,0,0,0
};

// I Piece (Linie, horizontal)
byte pieceI[16] = {
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,
  0,0,0,0
};

// T Piece
byte pieceT[16] = {
  0,0,0,0,
  0,1,1,1,
  0,0,1,0,
  0,0,0,0
};

// S Piece
byte pieceS[16] = {
  0,0,0,0,
  0,0,1,1,
  0,1,1,0,
  0,0,0,0
};

// Z Piece
byte pieceZ[16] = {
  0,0,0,0,
  0,1,1,0,
  0,0,1,1,
  0,0,0,0
};

// J Piece
byte pieceJ[16] = {
  0,0,0,0,
  0,1,0,0,
  0,1,1,1,
  0,0,0,0
};

// L Piece
byte pieceL[16] = {
  0,0,0,0,
  0,0,0,1,
  0,1,1,1,
  0,0,0,0
};

// Array mit den Zeigern auf die Formen
byte *pieces[NUM_PIECES] = { pieceO, pieceI, pieceT, pieceS, pieceZ, pieceJ, pieceL };

// Aktuelle fallende Figur
int currentPieceIndex;         // Index im pieces-Array
int currentRotation = 0;       // Rotation (0 bis 3)
int currentX, currentY;        // Position (obere linke Ecke des 4x4 Blocks) im Spielfeld

unsigned long lastFallTime = 0;
const unsigned long fallInterval = 500; // Fallgeschwindigkeit in ms

// Score (global aus config.h definiert oder hier)
extern int score;

// Game Over-Flag für Tetris
bool gameOverTetris = false;

// Hilfsfunktion: Gibt den Wert an Position (i, j) im 4x4 Array zurück – angepasst an die Rotation
byte getPieceCell(byte *shape, int i, int j, int rot) {
  int r, c;
  switch(rot % 4) {
    case 0: r = i;      c = j;      break;
    case 1: r = 3 - j;  c = i;      break;
    case 2: r = 3 - i;  c = 3 - j;  break;
    case 3: r = j;      c = 3 - i;  break;
    default: r = i;     c = j;      break;
  }
  return shape[r * 4 + c];
}

// Prüft, ob die aktuelle Figur bei verschobener Position/Rotation Kollisionen hat
bool checkCollision(int newX, int newY, int rotation) {
  byte *shape = pieces[currentPieceIndex];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getPieceCell(shape, i, j, rotation)) {
        int gridX = newX + j;
        int gridY = newY + i;
        // Überprüfe Spielfeldgrenzen
        if (gridX < 0 || gridX >= COLS || gridY < 0 || gridY >= ROWS) {
          return true;
        }
        // Prüfe, ob an dieser Stelle bereits ein Block liegt
        if (grid[gridY][gridX] != 0) {
          return true;
        }
      }
    }
  }
  return false;
}

// Fügt die aktuelle Figur in das Spielfeld ein
void mergePieceToGrid() {
  byte *shape = pieces[currentPieceIndex];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getPieceCell(shape, i, j, currentRotation)) {
        int gridX = currentX + j;
        int gridY = currentY + i;
        if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS) {
          grid[gridY][gridX] = 1;
        }
      }
    }
  }
}

// Überprüft alle Zeilen auf Vollständigkeit und räumt das Spielfeld auf
void clearLines() {
  for (int r = ROWS - 1; r >= 0; r--) {
    bool full = true;
    for (int c = 0; c < COLS; c++) {
      if (grid[r][c] == 0) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int row = r; row > 0; row--) {
        for (int c = 0; c < COLS; c++) {
          grid[row][c] = grid[row - 1][c];
        }
      }
      for (int c = 0; c < COLS; c++) {
        grid[0][c] = 0;
      }
      score += 10;
      r++; // dieselbe Zeile erneut prüfen
    }
  }
}

// Erzeugt eine neue fallende Figur; wenn eine sofortige Kollision auftritt, ist das Game Over
void spawnNewPiece() {
  currentPieceIndex = random(NUM_PIECES);
  currentRotation = 0;
  currentX = (COLS - 4) / 2;
  currentY = 0;
  if (checkCollision(currentX, currentY, currentRotation)) {
    gameOverTetris = true;
  }
}

// Hilfsfunktion zur Transformation der Koordinaten
// Transformiert (row, col) in "gedrehte" Koordinaten (newRow, newCol) (90° im Uhrzeigersinn)
void transformCoordinates(int row, int col, int &newRow, int &newCol) {
  newRow = col;
  newCol = (ROWS - 1) - row;
}

// Zeichnet das Spielfeld und die fallende Figur auf der LED-Matrix, rotiert um 90° im Uhrzeigersinn
void drawTetris() {
  lc.clearDisplay(0);
  
  int newRow, newCol;
  // Zeichne feststehende Blöcke im Spielfeld:
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      if (grid[r][c] != 0) {
        transformCoordinates(r, c, newRow, newCol);
        lc.setLed(0, newRow, newCol, true);
      }
    }
  }
  
  // Zeichne die fallende Figur:
  byte *shape = pieces[currentPieceIndex];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getPieceCell(shape, i, j, currentRotation)) {
        int gridX = currentX + j;
        int gridY = currentY + i;
        if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS) {
          transformCoordinates(gridY, gridX, newRow, newCol);
          lc.setLed(0, newRow, newCol, true);
        }
      }
    }
  }
}

// Erfundene Steuerung: invertierte Richtungen (Rechts/Links sowie Oben/Unten vertauscht)
void handleInput() {
  static unsigned long lastMoveTime = 0;
  static unsigned long lastRotateTime = 0;
  const unsigned long moveDelay = 150;   // minimaler Abstand zwischen Bewegungen (ms)
  const unsigned long rotateDelay = 300; // minimaler Abstand zwischen Rotationen (ms)
  
  unsigned long currentTime = millis();
  int xValue = analogRead(xAxis);
  int yValue = analogRead(yAxis);
  int btnState = digitalRead(button);
  
  // Seitliche Bewegung: X-Achse invertiert
  if (currentTime - lastMoveTime > moveDelay) {
    // Statt x > 600 (rechts) bewegen wir nach links:
    if (xValue > 600) {
      if (!checkCollision(currentX - 1, currentY, currentRotation)) {
        currentX--;
        lastMoveTime = currentTime;
      }
    }
    // Statt x < 400 (links) bewegen wir nach rechts:
    else if (xValue < 400) {
      if (!checkCollision(currentX + 1, currentY, currentRotation)) {
        currentX++;
        lastMoveTime = currentTime;
      }
    }
  }
  
  // Vertikale Bewegung: Y-Achse invertiert
  if (currentTime - lastMoveTime > moveDelay) {
    // Statt y < 400 (schneller fallen) bewegen wir nach oben
    if (yValue < 400) {
      if (!checkCollision(currentX, currentY - 1, currentRotation)) {
        currentY--;
        lastMoveTime = currentTime;
      }
    }
    // Statt y > 600 (weniger fallen) bewegen wir nach unten:
    else if (yValue > 600) {
      if (!checkCollision(currentX, currentY + 1, currentRotation)) {
        currentY++;
        lastMoveTime = currentTime;
      }
    }
  }
  
  // Rotation via Button (unverändert)
  if (btnState == LOW && (currentTime - lastRotateTime > rotateDelay)) {
    int newRotation = (currentRotation + 1) % 4;
    if (!checkCollision(currentX, currentY, newRotation)) {
      currentRotation = newRotation;
    }
    lastRotateTime = currentTime;
  }
}

// Zeigt den Score (wie beim Snake-Spiel) auf der LED-Matrix an
void displayScoreTetris() {
  lc.clearDisplay(0);
  if (score < 10) {
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, digits[score][i]);
    }
  } else {
    int tens = score / 10;
    int ones = score % 10;
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, digits[tens][i]);
    }
    delay(2000); // Zeige die Zehnerziffer 2 Sek.
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, digits[ones][i]);
    }
  }
  // Warten auf Buttondruck, um zurückzusetzen
  while (digitalRead(button) == HIGH) {
    delay(100);
  }
}

// Haupt-Update-Funktion des Spiels
void updateTetris() {
  handleInput();
  // Automatisches Herabfallen
  if (millis() - lastFallTime > fallInterval) {
    if (!checkCollision(currentX, currentY + 1, currentRotation)) {
      currentY++;
    } else {
      mergePieceToGrid();
      clearLines();
      spawnNewPiece();
    }
    lastFallTime = millis();
  }
  drawTetris();
}

// Startet das Tetris-Spiel – kann in loop() aus main.cpp aufgerufen werden
void startTetris() {
  // Spielfeld leeren
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      grid[r][c] = 0;
    }
  }
  score = 0;
  gameOverTetris = false;
  spawnNewPiece();
  lastFallTime = millis();
  
  // Haupt-Spielschleife
  while (!gameOverTetris) {
    updateTetris();
    delay(50);
  }
  
  // Game Over: Score anzeigen
  displayScoreTetris();
}

#endif