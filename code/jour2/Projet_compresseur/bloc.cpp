/*
 * bloc.cpp — traitement d'un bloc de 128 échantillons  (FOURNI COMPLET, rien à modifier)
 *
 * Chaque échantillon passe par compresser(), à fe = Fs.
 */
#include "compresseur.h"

void AudioEffectCompresseur::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;

  for (int n = 0; n < AUDIO_BLOCK_SAMPLES; n++) {
    const float x = block->data[n] / 32768.0f;
    block->data[n] = versInt16(compresser(x));
  }

  transmit(block);
  release(block);
}
