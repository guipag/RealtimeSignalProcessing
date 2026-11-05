/*
 * mesure.h — banc de mesure spectrale embarqué (TP avancé A2)
 *
 * FICHIER FOURNI COMPLET : vous n'avez rien à modifier ici.
 *
 * Principe : ÉCHANTILLONNAGE COHÉRENT (norme IEEE 1241)
 * ------------------------------------------------------
 *  On capture N = 4096 échantillons d'un sinus dont la fréquence vaut EXACTEMENT
 *  m·Fs/N, avec m entier impair. Le sinus fait alors exactement m périodes dans
 *  la fenêtre : sa FFT (fenêtre rectangulaire) tombe pile sur la case m, sans
 *  aucune fuite spectrale. Chaque case de la FFT se lit directement :
 *    - case m                       : le signal
 *    - cases h·m (h = 2..10, repliées) : les harmoniques
 *    - toutes les autres cases      : bruit (et raies parasites, repliements...)
 *  m impair et N puissance de 2 => pgcd(m, N) = 1 : tous les codes du
 *  quantificateur sont parcourus, l'erreur de quantification est bien « étalée ».
 *
 * Deux classes :
 *   AudioSynthSineCoherent : générateur de sinus cohérent (incrément de phase
 *                            exact m·2^20, calcul en flottant -> très pur)
 *   MesureSpectrale        : capture via AudioRecordQueue + FFT + indicateurs
 */
#ifndef MESURE_H
#define MESURE_H

#include <Arduino.h>
#include <AudioStream.h>
#include <Audio.h>          // AudioRecordQueue, AudioNoInterrupts()

#define MES_LOG2N 12
#define MES_N     (1 << MES_LOG2N)          // 4096 échantillons = 32 blocs = 93 ms

// ---------------------------------------------------------------------------
// Générateur de sinus cohérent
// ---------------------------------------------------------------------------
class AudioSynthSineCoherent : public AudioStream {
public:
  AudioSynthSineCoherent() : AudioStream(0, NULL) {}

  // Choisit la case m (impaire) la plus proche de f et renvoie la fréquence réelle
  float frequency(float f);
  void  amplitude(float a) { amp_ = a < 0.0f ? 0.0f : (a > 1.0f ? 1.0f : a); }
  uint32_t bin() const { return m_; }

  virtual void update(void);

private:
  volatile uint32_t inc_ = 93u << (32 - MES_LOG2N);   // m·2^32/N
  volatile uint32_t m_   = 93;                        // 93·Fs/N ≈ 1001,7 Hz
  volatile float    amp_ = 0.5f;
  uint32_t phase_ = 0;
};

// ---------------------------------------------------------------------------
// Mesure spectrale
// ---------------------------------------------------------------------------
class MesureSpectrale {
public:
  // À appeler une fois dans setup() (calcule la table des twiddles)
  void begin(AudioRecordQueue &q);

  // Relance une acquisition ; les 'ignorer' premiers blocs sont jetés
  // (régime transitoire après un changement de paramètre)
  void relancer(int ignorer = 12);

  // À appeler à chaque tour de loop() : renvoie true quand N échantillons
  // ont été capturés (la capture est alors stoppée jusqu'au prochain relancer())
  bool collecter();

  // Analyse le tampon capturé :
  //   m          : case du sinus (AudioSynthSineCoherent::bin())
  //   bandeHz    : borne haute de la bande utile pour snrBande
  void analyser(uint32_t m, float bandeHz);

  // ----- Résultats (en dB) -----
  float niveau_dBFS;   // amplitude du fondamental / pleine échelle
  float sinad;         // signal / (bruit + distorsion), toute la bande 0..Fs/2
  float snr;           // signal / bruit (harmoniques 2..10 exclues), 0..Fs/2
  float thd;           // harmoniques 2..10 / signal (dBc, négatif)
  float snrBande;      // signal / bruit dans la bande [0, bandeHz]
  float enob;          // nombre effectif de bits, ramené à la pleine échelle
  float pireRaie;      // pire raie NON harmonique (dBc) : repliements, parasites
  float pireRaieHz;    // ... et sa fréquence

  // Accès au tampon brut (pour un tracé éventuel sur le port série)
  const int16_t *echantillons() const { return buf_; }

private:
  void fft();                         // FFT complexe radix-2 en place

  AudioRecordQueue *q_ = nullptr;
  int16_t buf_[MES_N];
  float   re_[MES_N], im_[MES_N];
  float   cosT_[MES_N / 2], sinT_[MES_N / 2];
  int     rempli_ = 0, ignorer_ = 0;
  bool    actif_ = false;
};

#endif // MESURE_H
