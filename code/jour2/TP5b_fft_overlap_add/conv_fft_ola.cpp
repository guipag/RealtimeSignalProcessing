/*
 * conv_fft_ola.cpp — convolution rapide par FFT, overlap-add  (VERSION ÉTUDIANT)
 *
 * Complétez le TODO (Q5) : l'overlap-add proprement dit.
 */
#include "conv_fft_ola.h"

void AudioFilterConvFFT::setKernel(const float *h, int M) {      // fourni (comme au TP5a)
  // taille de FFT : plus petite puissance de 2 >= L + M - 1
  const int N = puissance2(CONV_L + M - 1);

  // H = FFT de h complété par des zéros jusqu'à N points
  for (int n = 0; n < N; n++) {
    if (n < M) H_[n] = complex(h[n], 0.0f);
    else       H_[n] = complex(0.0f, 0.0f);
  }
  fft_ = instanceFFT(N);                                  // tables CMSIS pour N points
  arm_cfft_f32(fft_, (float *)H_, FFT_DIRECTE, ORDRE_NATUREL);   // H = FFT(h)

  N_ = N;
  M_ = M;
}

void AudioFilterConvFFT::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;
  const int N = N_;

  // 1) le bloc (L points), complété par des zéros jusqu'à N points
  for (int n = 0; n < CONV_L; n++) X_[n] = complex(block->data[n] / 32768.0f, 0.0f);
  for (int n = CONV_L; n < N; n++) X_[n] = complex(0.0f, 0.0f);

  // 2) passage en fréquence : FFT de CMSIS-DSP, sur place
  arm_cfft_f32(fft_, (float *)X_, FFT_DIRECTE, ORDRE_NATUREL);

  // 3) filtrage : produit des spectres, case par case
  for (int k = 0; k < N; k++) X_[k] = X_[k] * H_[k];

  // 4) retour en temps : N valeurs, sans repliement (division par N comprise)
  arm_cfft_f32(fft_, (float *)X_, FFT_INVERSE, ORDRE_NATUREL);

  // TODO (Q5) 5) overlap-add dans le buffer circulaire acc_ :
  //   - ajouter les N valeurs X_[i].real() dans acc_[(r_ + i) & FFT_MASK]
  //   - pour n = 0 .. L-1 : envoyer acc_[(r_ + n) & FFT_MASK] (versInt16),
  //     puis remettre cette case à 0
  //   - faire avancer r_ de L (avec bouclage)
  for (int n = 0; n < CONV_L; n++) block->data[n] = versInt16(X_[n].real());   // provisoire : sans la queue

  transmit(block);
  release(block);
}
