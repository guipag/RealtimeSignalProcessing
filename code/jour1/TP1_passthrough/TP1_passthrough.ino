/*
 * TP1 — Passthrough & prise en main  (VERSION ÉTUDIANT)
 * DSP temps réel sur Teensy 4.x + Audio Shield (SGTL5000)
 *
 * Objectif : router l'entrée ligne vers la sortie casque (stéréo),
 * régler le codec, et instrumenter CPU + mémoire.
 *
 * Complétez les zones marquées  // TODO
 */

#include <Audio.h>
#include <Wire.h>

// ----------------------------------------------------------------------------
// 1) Objets du graphe de flux
// ----------------------------------------------------------------------------
AudioInputI2S        in;       // entrée I2S (ADC du SGTL5000)
// TODO 1 : déclarer la sortie I2S       ->  AudioOutputI2S        out;
// TODO 2 : déclarer le contrôleur codec ->  AudioControlSGTL5000  sgtl;

// ----------------------------------------------------------------------------
// 2) Connexions (patch cords)
//    AudioConnection(source, canalSource, destination, canalDest)
//    canal 0 = gauche (L), canal 1 = droite (R)
// ----------------------------------------------------------------------------
// TODO 3 : connecter in[0] -> out[0]  (voie gauche)
// TODO 4 : connecter in[1] -> out[1]  (voie droite)

// Chronomètre d'affichage
elapsedMillis tPrint;

// Cette fonction est appelée UNE SEULE FOIS au démarrage de la carte (ou après un reset)
// On y place toutes les fonctions/méthodes d'initialisation
void setup() {
  Serial.begin(115200);

  // ------------------------------------------------------------------------
  // 3) Réserver la mémoire audio (pool de blocs de 128 échantillons)
  // ------------------------------------------------------------------------
  // TODO 5 : AudioMemory(12);

  // ------------------------------------------------------------------------
  // 4) Configurer le codec SGTL5000
  // ------------------------------------------------------------------------
  // TODO 6 : sgtl.enable();
  // TODO 7 : sgtl.inputSelect(AUDIO_INPUT_LINEIN);   // ou AUDIO_INPUT_MIC
  // TODO 8 : sgtl.volume(0.5);                        // sortie casque 0..1
  // TODO 9 : sgtl.lineInLevel(5);                     // gain entrée ligne 0..15
}

// Cette fonction est exécutée en boucle, aussi vite que le processeur le permet. 
// Attention : l'audio est géré par interruption, cette fonction ne doit donc contenir aucun traitement audio
// Elle est utilisée pour gérer les interfaces (affichage de texte, écran, liaison avec le PC, la lecture de valeurs analogiques pour des paramètres, etc.)
void loop() {
  if (tPrint >= 1000) {
    tPrint = 0;
    Serial.print("CPU % = ");     Serial.print(AudioProcessorUsage(), 2);
    Serial.print("  CPUmax = ");  Serial.print(AudioProcessorUsageMax(), 2);
    Serial.print("  Mem = ");     Serial.print(AudioMemoryUsage());
    Serial.print("  MemMax = ");  Serial.println(AudioMemoryUsageMax());
  }
}

