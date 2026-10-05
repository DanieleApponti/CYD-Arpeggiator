#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>           
#include <XPT2046_Touchscreen.h> 

#define AUDIO_PIN       26      
#define XPT2046_CS      33      
#define XPT2046_IRQ     36      

TFT_eSPI tft = TFT_eSPI();
SPIClass mySPI = SPIClass(VSPI);
XPT2046_Touchscreen touch(XPT2046_CS, XPT2046_IRQ);

// --- LOGICA ARPEGGIATORE ---
int bpm = 120;
unsigned long lastNoteTime = 0;
int noteDuration = 125;
int notaAttuale = 0;

// Direzione arpeggio: 0 = UP, 1 = DOWN
int direzione = 0; 

// Struttura del Giro Armonico (Frequenze delle note)
int giroArmonico[4][4] = {
  {261, 329, 392, 523}, // 0: Do Maggiore (Do4, Mi4, Sol4, Do5)
  {220, 261, 329, 440}, // 1: La Minore (La3, Do4, Mi4, La4)
  {293, 349, 440, 587}, // 2: Re Minore (Re4, Fa4, La4, Re5)
  {196, 246, 293, 392}  // 3: Sol Maggiore (Sol3, Si3, Re4, Sol4)
};
const char* nomiAccordi[] = {"DoM", "Lam", "Rem", "SolM"};
int accordoAttuale = 0;

// --- GESTIONE TOUCH (CLICK / DOPPIO CLICK) ---
unsigned long lastTouchTime = 0;
bool touchRilasciato = true;
const int debounceDelay = 250; // Tempo massimo tra due tocchi per il doppio click

// Funzione di supporto per aggiornare lo schermo senza rallentare le note
void aggiornaDisplay() {
  tft.fillRect(20, 100, 280, 80, TFT_BLACK); // Pulisce solo l'area dei dati
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("CHORD: ", 20, 100);
  tft.drawString(nomiAccordi[accordoAttuale], 140, 100);

  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.drawString("DIR: ", 20, 140);
  if (direzione == 0) tft.drawString("UP", 140, 140);
  else tft.drawString("DOWN", 140, 140);
}

void setup() {
  Serial.begin(115200);
  pinMode(AUDIO_PIN, OUTPUT);

  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);

  mySPI.begin(25, 39, 32, 33); 
  touch.begin(mySPI);
  touch.setRotation(1);

  // Grafica Statica (Disegnata una sola volta all'avvio)
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString("CYD ARPEGGIATOR", 25, 10);
  
  // Area del "Pulsantone" touch virtuale
  tft.drawRect(10, 50, 300, 40, TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("TOCCA QUI (1x Accordi / 2x Dir)", 20, 62);

  aggiornaDisplay();

  noteDuration = (60000 / bpm) / 4;
  lastNoteTime = millis();
}

void loop() {
  // 1. CONTROL TOUCH (Simulazione click/doppio click dell'encoder)
  if (touch.touched() && touchRilasciato) {
    TS_Point p = touch.getPoint();
    int touchX = map(p.y, 240, 3800, 0, 320);
    int touchY = map(p.x, 3800, 240, 0, 240);

    // Controlla se tocchiamo l'area del pulsantone
    if (touchX > 10 && touchX < 310 && touchY > 50 && touchY < 90) {
      unsigned long touchTime = millis();
      
      if (touchTime - lastTouchTime < debounceDelay) {
        // --- DOPPIO CLICK: Cambia Direzione ---
        direzione = (direzione == 0) ? 1 : 0;
        aggiornaDisplay();
        lastTouchTime = 0; // Resetta per evitare falsi tripli click
      } else {
        // --- CLICK SINGOLO: Avanza nel Giro Armonico ---
        accordoAttuale++;
        if (accordoAttuale >= 4) accordoAttuale = 0;
        aggiornaDisplay();
        lastTouchTime = touchTime;
      }
      touchRilasciato = false; // Blocca finché non stacchi il dito
    }
  }

  if (!touch.touched()) {
    touchRilasciato = true; // Il dito è stato sollevato, pronto per il prossimo tocco
  }

  // 2. LOGICA DI ARPEGGIO FLUIDA
  if (millis() - lastNoteTime >= noteDuration) {
    lastNoteTime = millis();

    // Calcola l'indice della nota in base alla direzione (UP o DOWN)
    int indiceNota = notaAttuale;
    if (direzione == 1) {
      indiceNota = 3 - notaAttuale; // Inverte l'ordine (3, 2, 1, 0)
    }

    int frequenzaNota = giroArmonico[accordoAttuale][indiceNota];
    
    // Suona la nota sullo speaker del CYD
    tone(AUDIO_PIN, frequenzaNota, noteDuration - 25); 

    notaAttuale++;
    if (notaAttuale >= 4) {
      notaAttuale = 0;
    }
  }
}