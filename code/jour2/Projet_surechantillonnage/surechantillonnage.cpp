/*
 * surechantillonnage.cpp — calcul du compresseur à K·Fs  (VERSION ÉTUDIANT)
 *
 * Complétez les TODO 5 à 7 (voir la fiche du projet, partie C) ; update() est fourni.
 *
 * Même buffer circulaire qu'au TP4 : on écrit à l'index w, la valeur d'il y a k
 * échantillons est à (w - k) & OS_MASK, puis w avance d'une case.
 * Les coefficients h_[0..OS_N-1] sont calculés dans filtre_sinc.cpp.
 */
#include "compresseur.h"

// Bloc de 128 échantillons à Fs : chaque échantillon passe à K·Fs (interpoler),
// est compressé K fois, puis revient à Fs (decimer).   (fourni)
void AudioEffectCompresseur::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;

  for (int n = 0; n < AUDIO_BLOCK_SAMPLES; n++) {
    const float x = block->data[n] / 32768.0f;
    float u[K], v[K];
    interpoler(x, u);                            // K échantillons à K·Fs
    for (int p = 0; p < K; p++) v[p] = compresser(u[p]);
    block->data[n] = versInt16(decimer(v));      // retour à Fs
  }

  transmit(block);
  release(block);
}

// Convolution, comme au TP4 : somme des h[k] · c[w - k], pour k = 0 .. OS_N - 1
float AudioEffectCompresseur::filtrer(const float *c, int w) {
  // TODO 5) s = somme des h_[k] · c[(w - k) & OS_MASK], pour k = 0 .. OS_N - 1 ; renvoyer s
  return c[w];                                   // provisoire : pas de filtrage
}

// x à Fs -> u[0..K-1] à K·Fs : insérer K·x puis K-1 zéros, en filtrant à chaque fois
void AudioEffectCompresseur::interpoler(float x, float *u) {
  // TODO 6) pour p = 0 .. K-1 :
  //         a. écrire dans zCirc_[zW_] : K·x pour p = 0, puis 0 pour p = 1 .. K-1
  //         b. u[p] = filtrer(zCirc_, zW_)
  //         c. avancer zW_ d'une case : zW_ = (zW_ + 1) & OS_MASK
  for (int p = 0; p < K; p++) u[p] = x;          // provisoire : x répété K fois, sans filtre
}

// v[0..K-1] à K·Fs -> un échantillon à Fs : on ne calcule qu'une sortie filtrée sur K
float AudioEffectCompresseur::decimer(const float *v) {
  // TODO 7) a. écrire v[0] dans vCirc_[vW_], calculer y = filtrer(vCirc_, vW_), avancer vW_
  //         b. pour p = 1 .. K-1 : écrire v[p] dans vCirc_[vW_] et avancer vW_ (sans filtrer)
  //         c. renvoyer y
  return v[0];                                   // provisoire : un sur K, sans filtre
}
