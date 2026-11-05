/*
 * noyaux.h — réponses impulsionnelles h[n]  (FOURNI COMPLET, rien à modifier)
 *
 * Chaque fonction remplit h[0..M-1] et renvoie le nombre M de coefficients.
 *
 *   noyauIdentite(h)            h = δ[n]                  -> y = x
 *   noyauRetard(h, D)           h = δ[n - D]              -> un retard EST une convolution
 *   noyauEcho(h, D, g)          h = δ[n] + g·δ[n - D]     -> un écho unique (normalisé)
 *   noyauMoyenne(h, M)          h = 1/M sur M points      -> moyenne glissante
 *   noyauPasseBas(h, M, fc)     sinus cardinal fenêtré    -> passe-bas de coupure fc (Hz)
 *   noyauSalle(h, M)            bruit à décroissance exponentielle (-60 dB sur M points)
 *                                                         -> réponse de salle synthétique
 */
#ifndef NOYAUX_H
#define NOYAUX_H

#define CONV_MMAX 3968                 // longueur maximale d'une réponse impulsionnelle

int noyauIdentite(float *h);
int noyauRetard  (float *h, int D);
int noyauEcho    (float *h, int D, float g);
int noyauMoyenne (float *h, int M);
int noyauPasseBas(float *h, int M, float fc);
int noyauSalle   (float *h, int M);

#endif // NOYAUX_H
