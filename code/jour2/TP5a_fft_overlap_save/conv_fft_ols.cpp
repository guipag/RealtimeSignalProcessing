/*
 * conv_fft_ols.cpp — convolution rapide par FFT, overlap-save  (VERSION ÉTUDIANT)
 *
 * Complétez les TODO dans l'ordre des questions de la fiche (Q1, Q2, Q3).
 */
#include "conv_fft_ols.h"

void AudioFilterConvOLS::setKernel(const float *h, int M) {
  // TODO (Q1) : taille de FFT N = plus petite puissance de 2 >= L + M - 1
  //             (fonction puissance2() de fft_cmsis.h)
  const int N = 128;                   // provisoire

  // TODO (Q1) : H_[n] = h[n] pour n < M, 0 au-delà (zéro-padding), puis H = FFT(h) :
  //               fft_ = instanceFFT(N);
  //               arm_cfft_f32(fft_, (float *)H_, FFT_DIRECTE, ORDRE_NATUREL);
  for (int n = 0; n < N; n++) H_[n] = complex(1.0f, 0.0f);    // provisoire : H = 1

  N_ = N;
  M_ = M;
}

void AudioFilterConvOLS::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;
  const int N = N_;

  // TODO (Q3) 1) écrire les L échantillons du bloc (normalisés) dans entree_[w_],
  //              en faisant avancer w_ avec bouclage (& FFT_MASK)

  // TODO (Q3) 2) copier les N dernières entrées dans X_ :
  //              X_[i] = complex(entree_[(w_ - N + i) & FFT_MASK], 0.0f)
  // provisoire : le bloc seul, complété par des zéros (aucune mémoire)
  for (int n = 0; n < CONV_L; n++) X_[n] = complex(block->data[n] / 32768.0f, 0.0f);
  for (int n = CONV_L; n < N; n++) X_[n] = complex(0.0f, 0.0f);

  // passage en fréquence : FFT de CMSIS-DSP, sur place
  arm_cfft_f32(fft_, (float *)X_, FFT_DIRECTE, ORDRE_NATUREL);

  // TODO (Q2) : filtrage = produit des spectres X_[k] · H_[k], case par case

  // retour en temps : FFT inverse (division par N comprise)
  arm_cfft_f32(fft_, (float *)X_, FFT_INVERSE, ORDRE_NATUREL);

  // TODO (Q3) 3) garder les L DERNIÈRES valeurs : sortie n = X_[N - L + n].real()
  for (int n = 0; n < CONV_L; n++) block->data[n] = versInt16(X_[n].real());   // provisoire

  transmit(block);
  release(block);
}
