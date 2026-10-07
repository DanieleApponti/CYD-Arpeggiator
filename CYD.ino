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
int direzione = 0; // 0 = UP, 1 = DOWN

// --- STRUTTURA DEI GIRI ARMONICI (4 note per ogni accordo) ---
const int NUM_ACCORDI = 4;
const int NUM_GIRI = 3;

int tuttiIGiri[NUM_GIRI][NUM_ACCORDI][4] = {
  // GIRO 0: Il Classico Pop (DoM -> Lam -> Rem -> SolM)
  {
    {261, 329, 392, 523}, // DoM
    {220, 261, 329, 440}, // Lam
    {293, 349, 440, 587}, // Rem
    {196, 246, 293, 392}  // SolM
  },
  // GIRO 1: L'Epico Pop-Rock (Lam -> FaM -> DoM -> SolM)
  {
    {220, 261, 329, 440}, // Lam
    {175, 220, 261, 349}, // FaM  (Fa3, La3, Do4, Fa4)
    {261, 329, 392, 523}, // DoM
    {196, 246, 293, 392}  // SolM
  },
  // GIRO 2: Il Rock/Misterioso (Mim -> DoM -> SolM -> ReM)
  {
    {165, 196, 246, 330}, // Mim  (Mi3, Sol3, Si3, Mi4)
    {261, 329, 392, 523}, // DoM
    {196, 246, 293, 392}, // SolM
    {293, 370, 440, 587}  // ReM  (Re4, Fa#4, La4, Re5)
  }
};

// Nomi dei giri e degli accordi per lo schermo
const char* nomeDelGiro[] = {"CLASSICO POP", "EPICO DRAMMATICO", "ROCK MISTERIOSO"};
const char* nomiAccordi[NUM_GIRI][NUM_ACCORDI] = {
  {"DoM", "Lam", "Rem", "SolM"},
  {"Lam", "FaM", "DoM", "SolM"},
  {"Mim", "DoM", "SolM", "ReM"}
};

int giroAttuale = 0;      
int accordoAttuale = 0;   

// --- GESTIONE TOUCH WITH TIMEOUT ---
bool touchRilasciato = true;
unsigned long lastTouchTime = 0; // Serve solo per dare un micro-ritardo anti-rimbalzo
// Funzione grafica fluida per aggiornare i dati a schermo
void aggiornaDisplay() {
  tft.fillRect(10, 115, 300, 115, TFT_BLACK); // Pulisce l'area sotto il pulsante
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("GIRO: ", 20, 120);
  tft.drawString(nomeDelGiro[giroAttuale], 90, 120);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString("CHORD: ", 20, 155);
  tft.drawString(nomiAccordi[giroAttuale][accordoAttuale], 150, 155);

  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("DIREZIONE: ", 20, 205);
  if (direzione == 0) tft.drawString("UP (Salita)", 150, 205);
  else tft.drawString("DOWN (Discesa)", 150, 205);
}

void setup() {
  Serial.begin(115200);
  pinMode(AUDIO_PIN, OUTPUT);

  mySPI.begin(25, 39, 32, 33); 
  touch.begin(mySPI);
  touch.setRotation(1); 

  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);

  // --- NUOVA INTERFACCIA GRAFICA A DUE PULSANTI ---
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString("CYD ARPEGGIATOR", 25, 10);
  
  // Pulsante Sinistro: ACCORDI (Largo 145 pixel)
  tft.drawRect(10, 45, 145, 60, TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("ACCORDI", 40, 55); 
  tft.setTextSize(1);
  tft.drawString("(Tocca per cambiare)", 25, 82); 

  // Pulsante Destro: DIREZIONE (Largo 145 pixel)
  tft.drawRect(165, 45, 145, 60, TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("DIR.", 215, 55); 
  tft.setTextSize(1);
  tft.drawString("(Inverte l'arpeggio)", 180, 82); 

  aggiornaDisplay();

  noteDuration = (60000 / bpm) / 4;
  lastNoteTime = millis();
}

void loop() {
  // 1. FILTRO TOUCH ADATTIVO (IGNORA LA Y E CORREGGE L'ASSE X)
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    
    // Controlliamo solo la pressione (z > 500), il rilascio e l'anti-rimbalzo temporale
    if (p.z > 500 && touchRilasciato && (millis() - lastTouchTime > 150)) { 
      lastTouchTime = millis(); 

      // Il chip XPT2046 sul CYD spesso legge l'asse orizzontale sul canale p.x o p.y 
      // a seconda di come è saldato. Proviamo a mappare p.x per l'asse orizzontale puro da 0 a 320.
      int touchX = map(p.x, 240, 3800, 0, 320); 

      // --- CASO 1: SE TOCCI SUL LATO SINISTRO DELLO SCHERMO (touchX < 160) = ACCORDI ---
      if (touchX < 160) {
        accordoAttuale++;
        if (accordoAttuale >= NUM_ACCORDI) {
          accordoAttuale = 0;
          giroAttuale++;
          if (giroAttuale >= NUM_GIRI) {
            giroAttuale = 0;
          }
        }
        aggiornaDisplay();
      }
      
      // --- CASO 2: SE TOCCI SUL LATO DESTRO DELLO SCHERMO (touchX >= 160) = DIREZIONE ---
      else {
        direzione = (direzione == 0) ? 1 : 0;
        aggiornaDisplay();
      }
      
      touchRilasciato = false; // Blocca fino al rilascio del dito
    }
  }

  if (!touch.touched()) {
    touchRilasciato = true; // Dito sollevato
  }

  // 2. GENERATORE DI NOTE AUDIO FLUIDO
  if (millis() - lastNoteTime >= noteDuration) {
    lastNoteTime = millis();
    int indiceNota = notaAttuale;
    if (direzione == 1) {
      indiceNota = 3 - notaAttuale; 
    }
    int frequenzaNota = tuttiIGiri[giroAttuale][accordoAttuale][indiceNota];
    tone(AUDIO_PIN, frequenzaNota, noteDuration - 25); 
    notaAttuale++;
    if (notaAttuale >= 4) {
      notaAttuale = 0;
    }
  }
}