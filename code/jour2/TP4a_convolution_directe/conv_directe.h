/*
 * conv_directe.h — convolution directe par blocs, buffer circulaire  (VERSION ÉTUDIANT)
 *
 *   y[n] = somme pour k = 0 .. M-1 de h[k] · x[n - k]
 *
 * update() ne reçoit que 128 échantillons à la fois : pour les premières sorties
 * du bloc, les x[n - k] sont dans le bloc PRÉCÉDENT. On range donc tous les
 * échantillons reçus dans un buffer circulaire circ_ de CIRC_N cases :
 *   - x[n] est écrit à l'index w_, puis w_ avance d'une case (retour à 0 en fin de tableau) ;
 *   - x[n - k] se trouve k cases avant w_, à l'index (w_ - k) & CIRC_MASK.
 */
#ifndef CONV_DIRECTE_H
#define CONV_DIRECTE_H

#include <Arduino.h>
#include <AudioStream.h>
#include "noyaux.h"
#include "conversion.h"

#define CONV_L    AUDIO_BLOCK_SAMPLES  // 128 échantillons par bloc
#define CIRC_N    4096                 // taille du buffer : puissance de 2, au moins CONV_MMAX
#define CIRC_MASK (CIRC_N - 1)         // i & CIRC_MASK = i modulo CIRC_N, même si i < 0

class AudioFilterConvDirecte : public AudioStream {
public:
  AudioFilterConvDirecte() : AudioStream(1, inputQueueArray) {}

  // Copie la réponse impulsionnelle (M coefficients). À appeler dans setup().
  void setKernel(const float *h, int M) {
    for (int k = 0; k < M; k++) h_[k] = h[k];
    M_ = M;
  }
  int length() const { return M_; }

  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];
  float h_[CONV_MMAX];                 // réponse impulsionnelle
  int   M_ = 1;                        // nombre de coefficients
  float circ_[CIRC_N];                 // buffer circulaire des derniers échantillons reçus
  int   w_ = 0;                        // index d'écriture
};

#endif // CONV_DIRECTE_H
