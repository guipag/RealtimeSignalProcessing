/*
 * echo.cpp — écho sur ligne à retard  (VERSION ÉTUDIANT)
 *
 * Q1 : complétez les quatre TODO pour un écho simple.
 * Q2 : modifiez UNE ligne pour que l'écho se répète.
 */
#include "echo.h"

void AudioEffectEcho::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;

  for (int n = 0; n < AUDIO_BLOCK_SAMPLES; n++) {
    const float x = block->data[n] / 32768.0f;

    // TODO (Q1) 1) lire ce qui a été écrit D_ échantillons plus tôt :
    //              case (w_ - D_) & BUF_MASK
    const float retarde = 0.0f;                    // provisoire

    // TODO (Q1) 2) sortie : signal direct + g_ × valeur retardée
    const float y = x;                             // provisoire

    // TODO (Q1) 3) écrire l'entrée x dans le buffer, case w_
    // TODO (Q2)    puis : écrire la sortie y à la place de x

    // TODO (Q1) 4) avancer w_ d'une case, avec bouclage

    block->data[n] = versInt16(y);
  }

  transmit(block);
  release(block);
}
