/*
 * TP3 — Filtre biquad  (VERSION ÉTUDIANT)
 * Chaîne : in -> lpf (biquad DF2T custom) -> out.
 *
 * Paramètres pilotés en MIDI depuis le PC (pupitre : ressources_generales/logiciels/pupitre_midi.html) :
 *   CC 74 = cutoff (40..20000 Hz, log)   CC 71 = Q (0.5..10)
 *   CC 81 = mode  (< 64 : passe-bas, >= 64 : passe-haut)
 *
 * >>> Outils -> USB Type -> "Serial + MIDI + Audio"
 * >>> Pupitre : preset "TP3 - Biquad".
 *
 * Complétez les zones  // TODO
 */
#include <Audio.h>
#include <Wire.h>
#include "effect_biquad.h"
#include "midi_map.h"

AudioInputUSB            in;
AudioFilterBiquadManual  lpf;
AudioOutputI2S           out;
AudioControlSGTL5000     sgtl;

// TODO 1 : in[0] -> lpf[0]  puis  lpf[0] -> out[0]


elapsedMillis tPrint;
float fc       = 1000.0f;
float qFactor  = 0.707f;
bool  highpass = false;

void applyFilter() {
  if (highpass) lpf.setHighpass(fc, qFactor);
  else          lpf.setLowpass(fc, qFactor);
}

// --- Réception MIDI ---
void onControlChange(byte canal, byte cc, byte val) {
  switch (cc) {
    case 74:
      fc = midiLog(val, 40.0f, 20000.0f) ; 
      applyFilter();
      break;
    case 71:
      qFactor = midiLin(val, 0.5f, 10.0f) ; 
      applyFilter();
      break;
    case 81:
      highpass = midiBool(val) ; 
      applyFilter();
      break;
  }
}

void setup() {
  Serial.begin(115200);
  AudioMemory(12);

  sgtl.enable();
  sgtl.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl.volume(0.5);
  sgtl.lineInLevel(5);

  applyFilter();
  usbMIDI.setHandleControlChange(onControlChange);
}

void loop() {
  usbMIDI.read();

  if (tPrint >= 500) {
    tPrint = 0;
    Serial.print(highpass ? "HP" : "LP");
    Serial.print("  fc = ");     Serial.print(fc, 0);
    Serial.print(" Hz   Q = ");  Serial.print(qFactor, 2);
    Serial.print("   CPU % = "); Serial.println(AudioProcessorUsage(), 2);
  }
}
