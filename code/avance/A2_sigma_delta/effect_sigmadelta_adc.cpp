/*
 * effect_sigmadelta_adc.cpp — CAN sigma-delta 1 bit simulé  (VERSION ÉTUDIANT)
 */
#include "effect_sigmadelta_adc.h"
#include <math.h>

void AudioEffectSigmaDeltaADC::reinitialiser() {
  i1_ = i2_ = 0;
  b_ = 0;
  for (int k = 0; k < SD_KMAX; k++) { integ_[k] = 0; peigne_[k] = 0; }
}

void AudioEffectSigmaDeltaADC::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;

  if (ordreReq_ != ordre_ || osrReq_ != osr_) {
    ordre_ = ordreReq_;
    osr_   = osrReq_;
    reinitialiser();
  }

  const int ordre = ordre_;
  if (ordre == 0) {                 // arrêt : on laisse passer le signal
    transmit(block);
    release(block);
    return;
  }

  const int   R = osr_;
  const int   K = ordre + 1;
  const float gain = 1.0f / ((float)R * R * (K == 3 ? R : 1));   // 1 / R^K
  int32_t i1 = i1_, i2 = i2_, b = b_;
  uint32_t nUns = 0;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    // Entrée en Q15 (±32768 = pleine échelle), maintenue pendant OSR périodes.
    // Calcul ENTIER : exact, comme dans un circuit numérique (voir la fiche).
    const int32_t x = block->data[i];

    for (int r = 0; r < R; r++) {
      // ----- Modulateur ΣΔ (b = ±32768 : la pleine échelle) -----
      // TODO (Q8)  : ordre 1 : i1 += x - b ;  b = +32768 si i1 >= 0, sinon -32768
      // TODO (Q10) : ordre 2 : i1 += x - b ;  i2 += i1 - b ;  b = signe(i2)·32768
      b = (x >= 0) ? 32768 : -32768;      // provisoire : simple comparateur 1 bit

      const int32_t bit = (b > 0) ? 1 : -1;
      nUns += (bit > 0);

      // ----- Intégrateurs du CIC, à OSR·Fs -----
      integ_[0] += (uint32_t)bit;
      for (int k = 1; k < K; k++) integ_[k] += integ_[k - 1];
    }

    // ----- Décimation : 1 valeur sur R, puis peignes à Fs -----
    uint32_t v = integ_[K - 1];
    // TODO (Q9) : K peignes en cascade, pour k = 0..K-1 :
    //             d = v - peigne_[k] ;  peigne_[k] = v ;  v = d
    float y = (float)(int32_t)v * gain;          // ramené dans [-1, 1]

    float s = y * 32768.0f;
    if      (s >  32767.0f) s =  32767.0f;
    else if (s < -32768.0f) s = -32768.0f;
    block->data[i] = (int16_t)lrintf(s);
  }

  // Sécurité : un modulateur d'ordre 2 surchargé (|x| trop proche de 1)
  // peut devenir instable -> on réinitialise ses intégrateurs.
  if (i2 > (1 << 24) || i2 < -(1 << 24)) { i1 = i2 = 0; }

  i1_ = i1; i2_ = i2; b_ = b;
  densite_ = (float)nUns / (AUDIO_BLOCK_SAMPLES * R);

  transmit(block);
  release(block);
}
