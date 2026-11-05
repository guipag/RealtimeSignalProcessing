/*
 * effect_gain.cpp — implémentation de l'objet gain  (VERSION ÉTUDIANT)
 *
 * Complétez les zones  // TODO
 */
#include "effect_gain.h"

void AudioEffectGain::update(void) {
  // Récupérer un bloc MODIFIABLE sur l'entrée 0.
  audio_block_t *block = receiveWritable(0);

  // TODO : vérifier que le block n'est pas nul, sinon sortir prématurément de la fonction

  // TODO : si on a activé le bypass -> on transmet tel quel, puis on libère et on sort.

  // Choisir le gain effectif (0 si mute).
  float g = muted_ ? 0.0f : gain_;    

  // Traitement échantillon par échantillon.
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    // TODO : récupérer un échantillon dans block->data, le caster en float, puis le multiplier par le gain

    // TODO : SATURER y dans l'intervalle [-32768, +32767]
  
    // TODO : remplacer dans le bloc l'échantillon modifié casté en int16
  }

  // TODO : transmettre le bloc puis libérer le bloc
}
