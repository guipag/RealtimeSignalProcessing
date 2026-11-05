/*
 * fft_cmsis.h — FFT de la bibliothèque CMSIS-DSP (FOURNI COMPLET, rien à modifier)
 *
 * CMSIS-DSP est la bibliothèque de traitement du signal d'ARM, optimisée pour les
 * processeurs Cortex-M (FFT, filtres FIR et IIR, opérations sur des vecteurs...).
 * Elle est livrée avec Teensyduino : il suffit d'inclure arm_math.h.
 *
 * Type complex : nombre complexe, partie réelle et partie imaginaire en float
 * (c'est std::complex<float>). complex(a, b) construit a + j·b.
 *   X[k] * H[k]       produit de deux complexes
 *   X[k].real()       partie réelle        X[k].imag()     partie imaginaire
 *   std::abs(X[k])    module               std::arg(X[k])  phase
 *
 * FFT complexe de CMSIS, calculée EN PLACE (le tableau x est remplacé par le
 * résultat), pour N puissance de 2 (128 .. FFT_NMAX) :
 *   const arm_cfft_instance_f32 *fft = instanceFFT(N);    // tables de la FFT de N points
 *   arm_cfft_f32(fft, (float *)x, FFT_DIRECTE, ORDRE_NATUREL);
 *        X[k] = Σ_n x[n] · exp(-j·2π·k·n/N)
 *   arm_cfft_f32(fft, (float *)x, FFT_INVERSE, ORDRE_NATUREL);
 *        x[n] = (1/N) · Σ_k X[k] · exp(+j·2π·k·n/N)          (division par N comprise)
 * (float *)x : CMSIS attend un tableau de float [ré0, im0, ré1, im1, ...] ; c'est
 * exactement la façon dont un tableau de complex est rangé en mémoire.
 */
#ifndef FFT_CMSIS_H
#define FFT_CMSIS_H

#include <arm_math.h>
#include <arm_const_structs.h>
#include <complex>

typedef std::complex<float> complex;

#define FFT_NMAX      4096            // plus grande FFT possible
#define FFT_DIRECTE   0               // 3e argument de arm_cfft_f32
#define FFT_INVERSE   1
#define ORDRE_NATUREL 1               // 4e argument : résultat rangé dans l'ordre 0, 1, 2...

// Tables précalculées de CMSIS pour une FFT de N points (N = 128 .. 4096)
static inline const arm_cfft_instance_f32 *instanceFFT(int N) {
  switch (N) {
    case 128:  return &arm_cfft_sR_f32_len128;
    case 256:  return &arm_cfft_sR_f32_len256;
    case 512:  return &arm_cfft_sR_f32_len512;
    case 1024: return &arm_cfft_sR_f32_len1024;
    case 2048: return &arm_cfft_sR_f32_len2048;
    default:   return &arm_cfft_sR_f32_len4096;
  }
}

// Plus petite puissance de 2 supérieure ou égale à n
static inline int puissance2(int n) { int p = 1; while (p < n) p <<= 1; return p; }

#endif // FFT_CMSIS_H
