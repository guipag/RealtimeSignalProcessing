/*
 * compresseur.h — compresseur de dynamique et limiteur  (VERSION ÉTUDIANT)
 *
 * Un compresseur réduit le gain quand le niveau du signal dépasse un seuil.
 *   1) enveloppe : moyenne RÉCURSIVE de |x| (filtre passe-bas à un pôle)
 *          env[n] = a · env[n-1] + (1 - a) · |x[n]|,   a = exp(-1 / (tau · fe))
 *      tau court quand le niveau monte (attaque), long quand il descend (relâchement) ;
 *   2) niveau L = 20·log10(env) ; au-dessus du seuil T, le gain vaut
 *          G = (T - L) · (1 - 1/R)  dB      (R : taux de compression)
 *   3) sortie y = x · 10^(G/20).
 * Limiteur : même code, avec un taux très grand (R = 100) et des constantes de temps courtes.
 * Ici, le compresseur est calculé à fe = Fs (44,1 kHz).
 */
#ifndef COMPRESSEUR_H
#define COMPRESSEUR_H

#include <Arduino.h>
#include <AudioStream.h>
#include <math.h>
#include "conversion.h"

class AudioEffectCompresseur : public AudioStream {
public:
  AudioEffectCompresseur() : AudioStream(1, inputQueueArray) {}

  // Réglages : utilisables dans setup() ou à tout moment depuis loop()
  void setSeuil(float dB)       { seuil_ = dB; }
  void setTaux(float R)         { taux_ = R; }
  void setAttaque(float ms)     { attaqueMs_ = ms; aAtt_ = coefficient(ms); }
  void setRelachement(float ms) { relachementMs_ = ms; aRel_ = coefficient(ms); }

  float seuil() const       { return seuil_; }
  float taux() const        { return taux_; }
  float attaque() const     { return attaqueMs_; }
  float relachement() const { return relachementMs_; }

  virtual void update(void);           // bloc.cpp (fourni)

private:
  float compresser(float x);           // un échantillon, à fe       (compresseur.cpp, à compléter)

  // coefficient a de la moyenne récursive pour une constante de temps tau (en ms), à fe
  static float coefficient(float ms) {
    return expf(-1000.0f / (ms * AUDIO_SAMPLE_RATE_EXACT));
  }

  audio_block_t *inputQueueArray[1];
  volatile float seuil_ = -20.0f;      // seuil T (dBFS)
  volatile float taux_  = 4.0f;        // taux de compression R
  volatile float attaqueMs_ = 5.0f, relachementMs_ = 100.0f;
  volatile float aAtt_  = coefficient(5.0f);    // coefficient d'attaque
  volatile float aRel_  = coefficient(100.0f);  // coefficient de relâchement
  float env_ = 0.0f;                   // enveloppe : mémoire de la moyenne récursive
};

#endif // COMPRESSEUR_H
