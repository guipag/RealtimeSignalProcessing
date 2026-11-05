/*
 * effect_biquad.h — cellule biquad (DF2T) custom  (VERSION ÉTUDIANT)
 *
 * Forme directe II transposée, calcul en float (FPU du Cortex-M7).
 *   y  = b0*x + z1
 *   z1 = b1*x - a1*y + z2
 *   z2 = b2*x - a2*y
 * Coefficients (a0 normalisé à 1) via le cookbook RBJ.
 */
#ifndef EFFECT_BIQUAD_H
#define EFFECT_BIQUAD_H

#include <Arduino.h>
#include <AudioStream.h>
#include <Audio.h>

class AudioFilterBiquadManual : public AudioStream {
public:
  AudioFilterBiquadManual() : AudioStream(1, inputQueueArray) {}

  // Configure une réponse passe-bas (fréquence de coupure, facteur de qualité).
  void setLowpass(float freqHz, float Q);
  // Configure une réponse passe-haut.
  void setHighpass(float freqHz, float Q);

  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];

  // Coefficients (a0 = 1 après normalisation)
  volatile float b0_ = 1.0f, b1_ = 0.0f, b2_ = 0.0f;
  volatile float a1_ = 0.0f, a2_ = 0.0f;

  // États DF2T (persistent d'un bloc au suivant)
  float z1_ = 0.0f, z2_ = 0.0f;
};

#endif // EFFECT_BIQUAD_H
