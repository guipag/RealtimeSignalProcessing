/*
 * compresseur.cpp — compresseur de dynamique et limiteur  (VERSION ÉTUDIANT)
 *
 * Remplacer ce fichier par votre compresseur.cpp du projet Projet_compresseur :
 * le code est le même, seule la fréquence de calcul change (fe = K·Fs, prise
 * en compte dans coefficient(), voir compresseur.h).
 */
#include "compresseur.h"

// Un échantillon, à la fréquence de calcul fe
float AudioEffectCompresseur::compresser(float x) {
  const float r = fabsf(x);                      // redressement : le détecteur travaille sur |x|

  // TODO 1) enveloppe : env_ = a · env_ + (1 - a) · r,
  //         avec a = aAtt_ si r dépasse env_ (le niveau monte), aRel_ sinon

  // TODO 2) niveau = 20·log10(env_) (ajouter 1e-9f pour éviter log10(0)) ;
  //         au-dessus du seuil : gain_dB = (seuil_ - niveau) · (1 - 1/taux_), sinon 0
  float gain_dB = 0.0f;                          // provisoire

  // TODO 3) renvoyer x · 10^(gain_dB / 20)        (powf)
  return x;                                      // provisoire
}
