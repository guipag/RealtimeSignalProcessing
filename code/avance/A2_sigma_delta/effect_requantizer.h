/*
 * effect_requantizer.h — requantification sur B bits, dither TPDF,
 *                        mise en forme du bruit (ordre 0, 1 ou 2)
 * TP avancé A2, partie 1  (VERSION ÉTUDIANT — fichier complet)
 *
 *   v[n] = x[n] - Σ c_k·e[n-k]            (retour d'erreur)
 *   y[n] = Q( v[n] + d[n] )               (Q : arrondi au pas q = 2/2^B)
 *   e[n] = y[n] - v[n]                    (erreur commise, dither compris)
 *
 *   ordre 0 : pas de retour                 -> y = x + e[n]
 *   ordre 1 : v = x - e[n-1]                -> y = x + (1 - z^-1)·e
 *   ordre 2 : v = x - 2e[n-1] + e[n-2]      -> y = x + (1 - z^-1)²·e
 */
#ifndef EFFECT_REQUANTIZER_H
#define EFFECT_REQUANTIZER_H

#include <Arduino.h>
#include <AudioStream.h>

class AudioEffectRequantizer : public AudioStream {
public:
  AudioEffectRequantizer() : AudioStream(1, inputQueueArray) {}

  void setBits(int b)      { bits_  = b < 2 ? 2 : (b > 16 ? 16 : b); }
  void setDither(bool d)   { dither_ = d; }
  void setShaping(int o)   { ordre_ = o < 0 ? 0 : (o > 2 ? 2 : o); }

  virtual void update(void);

private:
  // Générateur pseudo-aléatoire xorshift32 : uniforme sur [0, 1)
  float aleatoire() {
    graine_ ^= graine_ << 13;
    graine_ ^= graine_ >> 17;
    graine_ ^= graine_ << 5;
    return graine_ * (1.0f / 4294967296.0f);
  }

  audio_block_t *inputQueueArray[1];

  volatile int  bits_   = 16;
  volatile bool dither_ = false;
  volatile int  ordre_  = 0;

  float    e1_ = 0.0f, e2_ = 0.0f;     // e[n-1], e[n-2]
  uint32_t graine_ = 22222u;
};

#endif // EFFECT_REQUANTIZER_H
