/*
 * effect_biquad.cpp — cellule biquad DF2T  (VERSION ÉTUDIANT)
 *
 * Complétez les zones  // TODO
 */
#include "effect_biquad.h"
#include <math.h>

// Fréquence d'échantillonnage exacte fournie par la librairie (44100.0f).
static const float FS = AUDIO_SAMPLE_RATE_EXACT;

void AudioFilterBiquadManual::setLowpass(float freqHz, float Q) {
  // 1) Grandeurs intermédiaires
  float w0 = 2.0f * (float)M_PI * freqHz / FS;
  float cw = cosf(w0);
  float sw = sinf(w0);
  float alpha = sw / (2.0f * Q);

  // TODO 1 : calculer b0, b1, b2, a0, a1, a2

  // TODO 2 : diviser b0,b1,b2,a1,a2 par a0

  // TODO 3 : enregistrer les coefficients dans les attributs de la classe. Pensez à encadrer l'affectation des membres par AudioNoInterrupts()/AudioInterrupts()

}

void AudioFilterBiquadManual::setHighpass(float freqHz, float Q) {
  float w0 = 2.0f * (float)M_PI * freqHz / FS;
  float cw = cosf(w0);
  float sw = sinf(w0);
  float alpha = sw / (2.0f * Q);

  float b0 =  (1.0f + cw) / 2.0f;
  float b1 = -(1.0f + cw);
  float b2 =  (1.0f + cw) / 2.0f;
  float a0 =  1.0f + alpha;
  float a1 = -2.0f * cw;
  float a2 =  1.0f - alpha;

  b0 /= a0; b1 /= a0; b2 /= a0; a1 /= a0; a2 /= a0;

  AudioNoInterrupts();
  b0_ = b0; b1_ = b1; b2_ = b2; a1_ = a1; a2_ = a2;
  AudioInterrupts();
}

void AudioFilterBiquadManual::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;

  // Copies locales pour la boucle
  float b0 = b0_, b1 = b1_, b2 = b2_, a1 = a1_, a2 = a2_;
  float z1 = z1_, z2 = z2_;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
     // TODO : récupérer un échantillon du buffer et le caster en float

     // TODO : calculer le nouvel échantillon filtré puis mettre à jour l'état
     // float y = ...
     // z1 = 
     // z2 = 

     // TODO : saturation int16

    block->data[i] = (int16_t)y;
  }

  // Sauvegarder l'état pour le bloc suivant
  z1_ = z1; z2_ = z2;

  transmit(block);
  release(block);
}
