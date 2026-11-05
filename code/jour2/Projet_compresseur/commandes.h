/*
 * commandes.h — réglages depuis le moniteur série  (FOURNI COMPLET, rien à modifier)
 *
 * Taper une commande puis Entrée, par exemple « relachement 10 ».
 * lireCommande() range le mot dans mot[] et le nombre qui suit dans *valeur ;
 * elle renvoie false s'il n'y a rien à lire.
 */
#ifndef COMMANDES_H
#define COMMANDES_H

#include <Arduino.h>

static inline bool lireCommande(String &mot, float *valeur) {
  if (!Serial.available()) return false;
  String s = Serial.readStringUntil('\n');
  s.trim();
  if (s.length() == 0) return false;
  const int esp = s.indexOf(' ');
  mot = (esp < 0) ? s : s.substring(0, esp);
  mot.toLowerCase();
  *valeur = (esp < 0) ? 0.0f : s.substring(esp + 1).toFloat();
  return true;
}

static inline float dBversLineaire(float dB) { return powf(10.0f, dB / 20.0f); }

#endif // COMMANDES_H
