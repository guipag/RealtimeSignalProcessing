/*
 * conv_fft_ola.h — convolution rapide par FFT, overlap-add  (VERSION ÉTUDIANT)
 *                   (TP5b)
 *
 * Filtrer = multiplier les spectres : Y[k] = H[k] · X[k]. Mais la FFT de taille N
 * calcule une convolution CIRCULAIRE. Pour obtenir la convolution LINÉAIRE d'un
 * bloc de L = 128 échantillons par h (M coefficients), on complète le bloc par des
 * zéros jusqu'à N points, avec
 *              N  >=  L + M - 1        (puissance de 2)
 *
 * Le bloc filtré fait alors N valeurs : les L premières sortent maintenant, les
 * N - L suivantes (la « queue ») s'ajoutent aux sorties des blocs suivants.
 * Les sorties en cours sont rangées dans un buffer circulaire acc_ (comme
 * l'historique des entrées du TP4a) ; celles du bloc courant commencent à l'index r_.
 * Les FFT sont celles de la bibliothèque CMSIS-DSP d'ARM (voir fft_cmsis.h).
 */
#ifndef CONV_FFT_OLA_H
#define CONV_FFT_OLA_H

#include <Arduino.h>
#include <AudioStream.h>
#include "fft_cmsis.h"
#include "noyaux.h"
#include "conversion.h"

#define CONV_L   AUDIO_BLOCK_SAMPLES   // 128 échantillons par bloc
#define FFT_MASK (FFT_NMAX - 1)        // modulo FFT_NMAX (4096, puissance de 2)

class AudioFilterConvFFT : public AudioStream {
public:
  AudioFilterConvFFT() : AudioStream(1, inputQueueArray) {}

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
  complex X_[FFT_NMAX];                  // bloc courant (temps puis fréquence)
  float   acc_[FFT_NMAX];                // buffer circulaire des sorties en cours
  int     r_ = 0;                        // index de la première sortie du bloc courant
};

#endif // CONV_FFT_OLA_H
