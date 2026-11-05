/*
 * conv_directe.cpp — convolution directe par blocs  (VERSION ÉTUDIANT)
 *
 * Complétez les trois TODO (Q3), dans l'ordre.
 */
#include "conv_directe.h"

void AudioFilterConvDirecte::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;

  for (int n = 0; n < CONV_L; n++) {
    // TODO (Q3) 1) écrire l'échantillon courant x[n] (normalisé : / 32768.0f)
    //              dans le buffer circulaire, à l'index w_

    // TODO (Q3) 2) y = somme pour k = 0 .. M_-1 de h_[k] · x[n - k],
    //              où x[n - k] est rangé dans circ_[(w_ - k) & CIRC_MASK]
    float y = block->data[n] / 32768.0f;           // provisoire : pas de filtrage

    // TODO (Q3) 3) faire avancer w_ d'une case, avec retour à 0 après CIRC_N - 1

    block->data[n] = versInt16(y);
  }

  transmit(block);
  release(block);
}
