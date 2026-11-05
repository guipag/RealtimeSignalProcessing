/*
 * bitstream_gpio.cpp — CNA 1 bit sur la broche 2  (VERSION ÉTUDIANT)
 */
#include "bitstream_gpio.h"

void AudioBitstreamGenerator::update(void) {
  audio_block_t *in = receiveReadOnly(0);
  if (!in) return;

  if (!actif_) {                    // sortie coupée : TDM2 émet des zéros (0 V)
    release(in);
    return;
  }

  // 16 blocs de sortie, un par entrée de AudioOutputTDM2
  audio_block_t *out[BS_CANAUX];
  for (int c = 0; c < BS_CANAUX; c++) {
    out[c] = allocate();
    if (!out[c]) {                  // plus de mémoire audio : on abandonne ce bloc
      for (int j = 0; j < c; j++) release(out[j]);
      release(in);
      return;
    }
  }

  const int ordre = ordre_;
  int32_t i1 = i1_, i2 = i2_, b = b_;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    // Q15 ; pleine échelle du modulateur = ±32768, on se limite à ±0,8
    // (26214/32768 = 0,8) pour garder le modulateur d'ordre 2 stable
    const int32_t x = ((int32_t)in->data[i] * 26214) >> 15;

    for (int c = 0; c < BS_CANAUX; c++) {
      uint32_t mot = 0;
      for (int k = 0; k < 16; k++) {
        // ----- Modulateur ΣΔ à 256·Fs, en entiers (calcul exact) -----
        // TODO (Q13) : même modulateur que dans le CAN (ordre 1 ou 2)
        b = (x >= 0) ? 32768 : -32768;    // provisoire

        // TODO (Q13) : empiler le bit (1 si b > 0) dans 'mot' de sorte que
        //              le 1er bit calculé soit le bit de poids fort (émis en 1er)
      }
      out[c]->data[i] = (int16_t)(uint16_t)mot;
    }
  }

  if (i2 > (1 << 24) || i2 < -(1 << 24)) { i1 = i2 = 0; }
  i1_ = i1; i2_ = i2; b_ = b;

  for (int c = 0; c < BS_CANAUX; c++) {
    transmit(out[c], c);
    release(out[c]);
  }
  release(in);
}
