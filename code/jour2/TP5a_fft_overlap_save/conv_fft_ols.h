/*
 * conv_fft_ols.h — convolution rapide par FFT, overlap-save  (VERSION ÉTUDIANT)
 *                   (TP5a)
 *
 * On recouvre les ENTRÉES : on transforme les N DERNIERS échantillons reçus,
 * bloc courant compris, sans zéro-padding :
 *        [ N - L échantillons passés | bloc courant (L) ]
 *   --FFT--> × H --FFT⁻¹--> y : les N - L premières valeurs sont repliées
 *   (fausses), les L dernières sont exactes si N - L >= M - 1.
 * Les entrées sont rangées dans un buffer circulaire entree_ (index d'écriture w_),
 * comme l'historique de la convolution directe du TP4a.
 * Les FFT sont celles de la bibliothèque CMSIS-DSP d'ARM (voir fft_cmsis.h).
 */
#ifndef CONV_FFT_OLS_H
#define CONV_FFT_OLS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "fft_cmsis.h"
#include "noyaux.h"
#include "conversion.h"

#define CONV_L   AUDIO_BLOCK_SAMPLES   // 128 échantillons par bloc
#define FFT_MASK (FFT_NMAX - 1)        // modulo FFT_NMAX (4096, puissance de 2)

class AudioFilterConvOLS : public AudioStream {
public:
  AudioFilterConvOLS() : AudioStream(1, inputQueueArray) {}

  // Choisit N et calcule H = FFT(h complété par des zéros). Calcul lourd :
  // à appeler dans setup(), AVANT AudioMemory() (l'audio ne tourne pas encore).
  void setKernel(const float *h, int M);
  int  length()  const { return M_; }
  int  fftSize() const { return N_; }

  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];
  complex H_[FFT_NMAX];                  // spectre du filtre, calculé une fois
  int     N_ = 128, M_ = 1;              // taille de FFT, nombre de coefficients
  const arm_cfft_instance_f32 *fft_ = &arm_cfft_sR_f32_len128;   // tables CMSIS de la FFT de N points
  complex X_[FFT_NMAX];                  // N dernières entrées (temps puis fréquence)
  float   entree_[FFT_NMAX];             // buffer circulaire des dernières entrées
  int     w_ = 0;                        // index d'écriture
};

#endif // CONV_FFT_OLS_H
