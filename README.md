# CYD-Arpeggiator
Arpeggiator CYD con AI Chrome - out spk -

# 🎹 CYD Arpeggiator (All-in-One Touch Synthesizer)
Un arpeggiatore e sintetizzatore musicale autonomo basato sul **CYD (Cheap Yellow Display) ESP32-2432R028**. 
Questo progetto trasforma una schedina economica con display touch in uno strumento musicale portatile "all-in-one". Rispetto alle versioni precedenti basate su Arduino Nano o ESP32 con moduli esterni, questa versione elimina completamente i potenziometri fisici, l'encoder e il chip audio esterno (VS1053), gestendo tutta la grafica, il touch e la sintesi sonora direttamente a bordo.
## 🚀 Caratteristiche Principali
* **Sintesi Sonora Nativa:** L'audio viene generato direttamente dall'ESP32 tramite onde quadre e l'amplificatore integrato del CYD, riprodotto dalla cassa acustica posteriore.
* **Interfaccia Touch Semplificata:** Schermo diviso simmetricamente in due macro-aree di tocco per un utilizzo "live" immediato e senza falsi positivi (causati dalla natura dei pannelli resistivi).
* **3 Giri Armonici Automatici:** 
  1. *Classico Pop (DoM - Lam - Rem - SolM)* — Atmosfera felice e nostalgica.
  2. *Epico Drammatico (Lam - FaM - DoM - SolM)* — Colonne sonore moderne ed emozionanti.
  3. *Rock Misterioso (Mim - DoM - SolM - ReM)* — Sonorità avvolgenti e leggermente dark.
* **Controllo Dinamico di Direzione:** Possibilità di invertire l'arpeggio al volo tra modalità **UP** (Salita) e **DOWN** (Discesa).
* **Massima Fluidità:** Aggiornamento dello schermo ottimizzato per non sottrarre cicli di clock al processore, garantendo un arpeggio stabile e privo di scatti (jitter).
## 🛠️ Hardware Richiesto
* **Display:** CYD (Cheap Yellow Display) ESP32-2432R028 (Display da 2.8" con Touch Screen Resistivo).
* **Audio:** Altoparlante passivo da 4/8 Ohm (collegato al connettore integrato sul retro del CYD).
* **Alimentazione:** Cavo Micro-USB o Type-C (a seconda della revisione della scheda).
## 📐 Logica dei Controlli Touch
Per ovviare ai limiti dei rimbalzi fisici dei display resistivi economici, l'interfaccia risponde dividendo il display in due metà verticali indipendenti dall'altezza del tocco:
* **Toccando il LATO SINISTRO (Zona ACCORDI / GIRO):** Avanza all'accordo successivo. Quando il giro armonico da 4 accordi si conclude, lo strumento passa automaticamente al *Genere/Giro Armonico successivo*, aggiornando i dati grafici sul display.
* **Toccando il LATO DESTRO (Zona DIREZIONE):** Inverte istantaneamente la direzione delle note dell'arpeggio (da Salita a Discesa e viceversa).
## 💻 Librerie Richieste (Arduino IDE)
Per compilare lo sketch, assicurati di aver installato le seguenti librerie tramite il Gestore Librerie di Arduino:
1. **`TFT_eSPI`** (per la gestione della grafica sul display)
2. **`XPT2046_Touchscreen`** (per la lettura dei dati grezzi del pannello touch)
3. **`SPI`** e **`Wire`** (librerie native di sistema)
> ⚠️ **Nota di configurazione:** Ricordati di configurare correttamente il file `User_Setup.h` all'interno della cartella della libreria `TFT_eSPI` inserendo i pin specifici del tuo modello di CYD, altrimenti lo schermo rimarrà bianco.
## 📝 Autore & Ringraziamenti
* Sviluppato da **[DanieApponti.GitHub]**
* Ottimizzato in collaborazione con l'assistente IA di Google.
