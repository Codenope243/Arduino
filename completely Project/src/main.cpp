#include <Arduino.h>
#include <snake.h>

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

// Icon für Atari Adventure (stilisiertes Schlüssel-Symbol)
// Dieses Icon soll an den Schlüssel erinnern, den man in Atari Adventure finden kann.
byte icon_atari_adventure[8] = {
  B01111100,  // große, runde Schlüsselkopfform (Pixel in den Spalten 1 bis 5)
  B01000100,  // Rand des Schlüsselkopfs
  B01000100,  // Schlüsselkopf
  B01000100,  // Beginn des Schaftes
  B01000100,  // Schaft
  B01000100,  // Schaft
  B01010100,  // erste "Zahnung" (Teeth) – Pixel in den Spalten 1, 3 und 5
  B00101000   // untere Zahnung, etwas schmaler (Pixel in den Spalten 2 und 4)
};

// Hilfsfunktion zum Blinken des Icons (langsames Ausblenden)
void blinkIcon(byte icon[8], int times, int delayTime) {
  for (int j = 0; j < times; j++) {
    lc.clearDisplay(0);
    delay(delayTime * 2);   // längere Pause für langsameres Blinken
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, icon[i]);
    }
    delay(delayTime * 2);
  }
}

// Zeigt das Menü auf der LED-Matrix an und lässt den Benutzer mittels Joystick (x-Achse)
// und Button eine Auswahl treffen.
void showMenuIcons() {
  int currentOption = 1;  // 1: Snake, 2: Tetris, 3: Adventure
  bool selected = false;
  
  Serial.println("Menü (Icons) auf LED-Matrix");
  
  // Variablen zum Auslesen des Joysticks:
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
          lc.setRow(0, i, icon_atari_adventure[i]);
        }
        break;
    }
    
    // Langsamer Blink-Effekt, um die Auswahl hervorzuheben:
    blinkIcon((currentOption == 1 ? icon_snake : 
               currentOption == 2 ? icon_tetris : icon_atari_adventure), 1, 150);
    
    // Joystick auslesen (x-Achse):
    xVal = analogRead(xAxis);
    if (xVal > 550) {  // Rechts bewegen → nächste Option
      currentOption++;
      if (currentOption > 3) {
        currentOption = 1;
      }
      delay(300);  // kurzer Delay als Entprellung
    } 
    else if (xVal < 470) {  // Links bewegen → vorherige Option
      currentOption--;
      if (currentOption < 1) {
        currentOption = 3;
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
  
  // Je nach Auswahl wird ein Spiel gestartet:
  if (currentOption == 1) {
    startSnake();  // Funktion aus snake.h
  } 
  else if (currentOption == 2) {
    //startTetris(); // Hier könnte der Tetris-Code aufgerufen werden
  } 
  else if (currentOption == 3) {
    //startAdventure(); // Hier könnte der Adventure-Code aufgerufen werden
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000); // Warten, bis der serielle Monitor bereit ist
  
  // Initialisiere die LED-Matrix (Anpassung je nach Hardware notwendig):
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