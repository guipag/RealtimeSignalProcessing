/*
 * filtre_sinc.cpp — coefficients du filtre de suréchantillonnage  (FOURNI COMPLET, rien à modifier)
 *
 * Un seul passe-bas h, commun à l'interpolation et à la décimation : sinus cardinal
 * fenêtré (Blackman), coupure 0,9 × Fs/2, OS_N = 32·K + 1 coefficients à K·Fs.
 * Chaque filtre retarde de (OS_N - 1)/2 = 16·K échantillons à K·Fs, soit 16 à Fs :
 * 32 au total (0,73 ms).
 */
#include "compresseur.h"

void AudioEffectCompresseur::calculerFiltre() {
  const float c  = (OS_N - 1) / 2.0f;          // centre du filtre
  const float fc = 0.45f / K;                  // coupure, en cycles par échantillon à K·Fs
  float somme = 0.0f;
  for (int k = 0; k < OS_N; k++) {
    const float t = k - c;
    const float s = (t == 0.0f) ? 2.0f * fc : sinf(2.0f * (float)M_PI * fc * t) / ((float)M_PI * t);
    const float w = 0.42f - 0.5f * cosf(2.0f * (float)M_PI * k / (OS_N - 1))
                          + 0.08f * cosf(4.0f * (float)M_PI * k / (OS_N - 1));
    h_[k] = s * w;
    somme += h_[k];
  }
  for (int k = 0; k < OS_N; k++) h_[k] /= somme;  // gain 1 en continu
}
