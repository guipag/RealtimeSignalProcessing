/*
 * TP2 — Objet « gain » custom  (VERSION ÉTUDIANT)
 * Chaîne : in -> gain (custom) -> out.
 *
 * Paramètres pilotés en MIDI depuis le PC (pupitre : ressources_generales/logiciels/pupitre_midi.html) :
 *   CC 7  = gain (0..4)     CC 80 = bypass     CC 81 = mute
 *
 * >>> Outils -> USB Type -> "Serial + MIDI + Audio"
 * >>> Ouvrir le pupitre dans Chrome, sortie "Teensy MIDI/Audio", preset "TP2 - Gain".
 *
 * Complétez les zones  // TODO
 */
#include <Audio.h>
#include <Wire.h>

// on doit inclure notre nouvel effet pour que le compilateur sache où aller chercher le code
#include "effect_gain.h"
// cette partie permet de faire le lien en MIDI entre la carte et le pupitre MIDI accessible dans ressources_generales/logiciels/
// Ne pas modifier ce code (vous pouvez toutefois aller regarder comment il fonctionne...)
#include "midi_map.h"

AudioInputUSB        in;
AudioEffectGain      gain;
AudioOutputI2S       out;
AudioControlSGTL5000 sgtl;

// TODO 1 : in[0] -> gain[0]  puis  gain[0] -> out[0]

AudioConnection patchR(in, 1, out, 1);       // voie droite en direct (A/B)

elapsedMillis tPrint;
float curGain = 1.0f;

// --- Réception MIDI : appelée automatiquement par usbMIDI.read() ---
void onControlChange(byte canal, byte cc, byte val) {
  switch (cc) {
    case 7:
      curGain = midiLin(val, 0.0f, 4.0f);
      gain.setGain(curGain);
      break;
    case 80:
      gain.setBypass(midiBool(val));
      break;
    case 81:
      gain.mute(midiBool(val));
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

  gain.setGain(curGain);
  usbMIDI.setHandleControlChange(onControlChange);
}

void loop() {
  usbMIDI.read();  

  if (tPrint >= 1000) {
    tPrint = 0;
    Serial.print("gain = ");     Serial.print(curGain, 2);
    Serial.print("  CPU % = ");  Serial.print(AudioProcessorUsage(), 2);
    Serial.print("  MemMax = "); Serial.println(AudioMemoryUsageMax());
  }
}
