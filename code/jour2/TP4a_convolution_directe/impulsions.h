/*
 * impulsions.h — générateur d'impulsions  (FOURNI COMPLET, rien à modifier)
 *
 * Source audio qui envoie une impulsion (un seul échantillon non nul) toutes les
 * 100 ms. Filtrée par h, chaque impulsion devient... h[n] : la réponse
 * impulsionnelle apparaît directement à l'oscilloscope.
 * Déclencher l'oscilloscope sur la voie droite (impulsion non filtrée).
 */
#ifndef IMPULSIONS_H
#define IMPULSIONS_H

#include <Arduino.h>
#include <AudioStream.h>

class AudioSynthImpulsions : public AudioStream {
public:
  AudioSynthImpulsions() : AudioStream(0, NULL) {}

  virtual void update(void) {
    audio_block_t *block = allocate();
    if (!block) return;
    for (int n = 0; n < AUDIO_BLOCK_SAMPLES; n++) {
      if (compteur_ == 0) block->data[n] = 29491;   // 0,9 × pleine échelle
      else                block->data[n] = 0;
      compteur_++;
      if (compteur_ == PERIODE) compteur_ = 0;
    }
    transmit(block);
    release(block);
  }

private:
  static const int PERIODE = 4410;     // 100 ms à 44,1 kHz
  int compteur_ = 0;
};

#endif // IMPULSIONS_H
