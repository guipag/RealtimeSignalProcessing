/*
 * midi_map.h — conversion des valeurs MIDI (0..127) en valeurs physiques
 *
 * Utilisé par tous les TP pilotés en MIDI. Les plages doivent correspondre
 * EXACTEMENT à celles du pupitre (ressources_generales/logiciels/pupitre_midi.html) pour que la valeur
 * affichée à l'écran soit celle réellement appliquée par la Teensy.
 *
 * ---------------------------------------------------------------------------
 * TABLE DES CONTROL CHANGE (CC) UTILISÉS
 * ---------------------------------------------------------------------------
 *  TP2 — Gain
 *    CC 7  gain           lin   0 .. 4          CC 80 bypass    CC 81 mute
 *  TP3 — Biquad
 *    CC 74 cutoff         log   40 .. 20000 Hz  CC 81 mode (0=passe-bas, 1=passe-haut)
 *    CC 71 Q              lin   0.5 .. 10
 *  (Jour 2 — TP4 à TP6 : pas de MIDI, réglages dans setup())
 *  A2 — Quantification / sigma-delta (TP avancé)
 *    CC 21 niveau source  lin   -60 .. 0 dBFS   CC 81 source (0=sinus de test, 1=USB)
 *    CC 24 fréq. sinus    log   200 .. 10000 Hz CC 82 dither TPDF
 *    CC 25 bits B         pas   {2, 3, 4, 6, 8, 12, 16}
 *    CC 26 mise en forme  pas   {0, 1, 2} (ordre)
 *    CC 27 CAN sigma-delta pas  {arrêt, ordre 1, ordre 2}
 *    CC 28 OSR            pas   {16, 32, 64}    CC 83 sortie bitstream sur la broche 2
 *    CC 29 bande mesure   log   1000 .. 20000 Hz
 * ---------------------------------------------------------------------------
 */
#ifndef MIDI_MAP_H
#define MIDI_MAP_H

#include <Arduino.h>
#include <math.h>

// Interpolation LINÉAIRE : v = 0 -> lo, v = 127 -> hi
static inline float midiLin(uint8_t v, float lo, float hi) {
  return lo + (hi - lo) * ((float)v / 127.0f);
}

// Interpolation LOGARITHMIQUE : v = 0 -> lo, v = 127 -> hi
// (utilisée pour les fréquences et les temps : l'oreille est logarithmique)
// Prérequis : lo > 0 et hi > 0.
static inline float midiLog(uint8_t v, float lo, float hi) {
  return lo * powf(hi / lo, (float)v / 127.0f);
}

// Sélecteur à n positions : 0..127 découpé en n zones égales -> 0..n-1
// (même découpage que le type 'steps' du pupitre)
static inline int midiStep(uint8_t v, int n) {
  int i = ((int)v * n) / 128;
  return i < n - 1 ? i : n - 1;
}

// Interrupteur : convention MIDI usuelle, >= 64 signifie « activé »
static inline bool midiBool(uint8_t v) {
  return v >= 64;
}

#endif // MIDI_MAP_H
