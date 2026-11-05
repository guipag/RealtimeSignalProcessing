/*
 * noyaux.cpp — réponses impulsionnelles  (FOURNI COMPLET, rien à modifier)
 */
#include "noyaux.h"
#include <Arduino.h>
#include <AudioStream.h>               // AUDIO_SAMPLE_RATE_EXACT
#include <math.h>
#include <stdint.h>

static int borne(int M) { return M < 1 ? 1 : (M > CONV_MMAX ? CONV_MMAX : M); }

int noyauIdentite(float *h) {
  h[0] = 1.0f;
  return 1;
}

int noyauRetard(float *h, int D) {
  const int M = borne(D + 1);
  for (int n = 0; n < M; n++) h[n] = 0.0f;
  h[M - 1] = 1.0f;
  return M;
}

int noyauEcho(float *h, int D, float g) {
  const int M = borne(D + 1);
  for (int n = 0; n < M; n++) h[n] = 0.0f;
  h[0]     = 1.0f / (1.0f + g);        // normalisé : la somme des coefficients vaut 1
  h[M - 1] += g / (1.0f + g);
  return M;
}

int noyauMoyenne(float *h, int M) {
  M = borne(M);
  for (int n = 0; n < M; n++) h[n] = 1.0f / M;
  return M;
}

int noyauPasseBas(float *h, int M, float fc) {
  M = borne(M);
  const float c = (M - 1) / 2.0f, f = fc / AUDIO_SAMPLE_RATE_EXACT;
  float somme = 0.0f;
  for (int n = 0; n < M; n++) {
    const float t = n - c;
    const float s = (fabsf(t) < 1e-6f) ? 2.0f * f : sinf(2.0f * (float)M_PI * f * t) / ((float)M_PI * t);
    const float w = (M > 1) ? 0.42f - 0.5f * cosf(2.0f * (float)M_PI * n / (M - 1))
                                    + 0.08f * cosf(4.0f * (float)M_PI * n / (M - 1)) : 1.0f;
    h[n] = s * w;                      // sinus cardinal × fenêtre de Blackman
    somme += h[n];
  }
  for (int n = 0; n < M; n++) h[n] /= somme;    // gain 1 en continu
  return M;
}

int noyauSalle(float *h, int M) {
  M = borne(M);
  uint32_t r = 12345u;
  float energie = 0.0f;
  for (int n = 0; n < M; n++) {
    r = r * 1664525u + 1013904223u;    // bruit pseudo-aléatoire reproductible
    const float bruit = ((r >> 8) * (1.0f / 16777216.0f)) * 2.0f - 1.0f;
    h[n] = bruit * expf(-6.9078f * n / M);      // décroissance de 60 dB sur M points
    energie += h[n] * h[n];
  }
  energie += 1.0f - h[0] * h[0];
  h[0] = 1.0f;                         // son direct
  const float k = 1.0f / sqrtf(energie);        // énergie unité : niveau comparable
  for (int n = 0; n < M; n++) h[n] *= k;
  return M;
}
