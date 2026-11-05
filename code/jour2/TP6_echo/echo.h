/*
 * echo.h — écho sur ligne à retard (buffer circulaire)  (VERSION ÉTUDIANT)
 *
 * Ligne à retard : on range le signal dans un buffer circulaire de BUF_N cases ;
 * ce qu'on a écrit à l'index w se relit D échantillons plus tard, à l'index
 * (w - D) & BUF_MASK.
 *
 *   1) Écho simple (Q1) : on écrit l'ENTRÉE dans le buffer
 *         y[n] = x[n] + g · x[n - D]                 -> une seule répétition
 *   2) Écho répété (Q2) : on écrit la SORTIE dans le buffer
 *         y[n] = x[n] + g · y[n - D]                 -> répétitions g, g², g³...
 *      L'écho ressort, est réécrit, ressort D plus tard multiplié par g, etc.
 */
#ifndef ECHO_H
#define ECHO_H

#include <Arduino.h>
#include <AudioStream.h>
#include "conversion.h"

#define BUF_N    32768                 // puissance de 2 : retard max ≈ 740 ms
#define BUF_MASK (BUF_N - 1)

class AudioEffectEcho : public AudioStream {
public:
  AudioEffectEcho() : AudioStream(1, inputQueueArray) {}

  // Réglages, à appeler dans setup()
  void setDelay(float ms) { D_ = (int)(ms * 0.001f * AUDIO_SAMPLE_RATE_EXACT); }   // < 740 ms
  void setGain(float g)   { g_ = g; }  // gain de l'écho, 0 .. 0,9 (1 ou plus : instable)

  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];
  float buf_[BUF_N];                   // ligne à retard (128 ko)
  int   w_ = 0;                        // index d'écriture
  int   D_ = 11025;                    // retard en échantillons (250 ms)
  float g_ = 0.5f;                     // gain de l'écho
};

#endif // ECHO_H
