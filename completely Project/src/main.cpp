#include <Arduino.h>
#include <LedControl.h>
#include <snake.h>
#include <tetris.h>
#include <ticTacToe.h>

// Icon für Snake (eine schlängelnde Linie)
byte icon_snake[8] = {
  0b00000000,
  0b01111110,
  0b01000010,
  0b01000000,
  0b01111000,
  0b00001000,
  0b00001110,
  0b00000000  
};

// Icon für Tetris (repräsentiert einen T-Block)
byte icon_tetris[8] = {
  B00000000,
  B00011000,
  B00011000,
  B00111100,
  B00011000,
  B00011000,
  B00000000,
  B00000000
};

// Icon für Tic Tac Toe (ein einfaches Gitter/Plus-Zeichen)
byte icon_tictactoe[8] = {
  0b01100101,
  0b10010010,
  0b10010101,
  0b01100000,
  0b00000110,
  0b10101001,
  0b01001001,
  0b10100110
};

// Icon für Adventure (stilisiertes Schlüssel-Symbol)
byte icon_adventure[8] = {
  B01111100,  // Schlüsselkopf
  B01000100,  // Rand
  B01000100,  // Schlüsselkopf
  B01000100,  // Beginn des Schaftes
  B01000100,  // Schaft
  B01000100,  // Schaft
  B01010100,  // erste Zahnung
  B00101000   // untere Zahnung
};

// Menüanzeige und Auswahlfunktion
void showMenuIcons() {
  int currentOption = 1;  // 1: Snake, 2: Tetris, 3: Tic Tac Toe, 4: Adventure
  bool selected = false;
  
  Serial.println("Menü (Icons) auf LED-Matrix");
  
  // Variable für Joystickwert (x-Achse)
  int xVal = 0;
  
  while (!selected) {
    // Darstellen des aktuellen Icons:
    switch (currentOption) {
      case 1:
        for (int i = 0; i < 8; i++) {
          lc.setRow(0, i, icon_snake[i]);
        }
        break;
      case 2:
        for (int i = 0; i < 8; i++) {
          lc.setRow(0, i, icon_tetris[i]);
        }
        break;
      case 3:
        for (int i = 0; i < 8; i++) {
          lc.setRow(0, i, icon_tictactoe[i]);
        }
        break;
      case 4:
        for (int i = 0; i < 8; i++) {
          lc.setRow(0, i, icon_adventure[i]);
        }
        break;
    }
    
    // Blinkeffekt, um die Auswahl hervorzuheben:
    // Hier wird das aktuelle Icon einmal kurz ausgeblendet und wieder eingeblendet.
    delay(150);
    
    // Joystick auslesen (x-Achse):
    xVal = analogRead(xAxis);
    if (xVal > 550) {  // Rechts bewegen → nächste Option
      currentOption++;
      if (currentOption > 4) {
        currentOption = 1;
      }
      delay(300);  // Entprellzeit
    } 
    else if (xVal < 470) {  // Links bewegen → vorherige Option
      currentOption--;
      if (currentOption < 1) {
        currentOption = 4;
      }
      delay(300);
    }
    
    // Button zum Bestätigen der Auswahl:
    if (digitalRead(button) == LOW) {
      selected = true;
      delay(300);  // Entprellzeit
    }
    
    delay(100);
  }
  
  lc.clearDisplay(0);
  Serial.print("Ausgewählte Option (Icon): ");
  Serial.println(currentOption);
  
  // Je nach Auswahl wird das entsprechende Spiel gestartet:
  if (currentOption == 1) {
    startSnake();  
  } 
  else if (currentOption == 2) {
    startTetris();
  }
  else if (currentOption == 3) {
    startTicTacToe();
  }
  else if (currentOption == 4) {
    //startAdventure();
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);  // Warten, bis der serielle Monitor bereit ist
  
  // LED-Matrix initialisieren
  lc.shutdown(0, false);       // Display aktivieren
  lc.setIntensity(0, 1);       // Helligkeit einstellen (0 bis 15)
  lc.clearDisplay(0);          // Display leeren
  
  // Button-Pin als INPUT_PULLUP
  pinMode(button, INPUT_PULLUP);
}

void loop() {
  showMenuIcons();
  
  // Nach Spielende kehrt das Programm ins Menü zurück:
  Serial.println("Zurück ins Menü...");
  delay(2000);
}