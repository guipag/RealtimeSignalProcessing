/*
 * bitstream_gpio.h — CNA 1 bit : flux sigma-delta sur une broche (GPIO)
 * TP avancé A2, partie 3  (VERSION ÉTUDIANT — fichier complet)
 *
 * IDÉE : détourner le second port audio série de la Teensy 4 (SAI2), piloté
 * par AudioOutputTDM2, pour en faire un « registre à décalage » à 11,29 MHz.
 *
 *   AudioOutputTDM2 émet, à chaque période d'échantillonnage, une trame de
 *   8 mots de 32 bits = 256 bits, sans aucun trou, sur la broche 2 (TX_DATA),
 *   à BCLK = 256·Fs = 11,294 MHz (broche 4).  Les 16 entrées de 16 bits sont
 *   émises dans l'ordre : entrée 0 (bit de poids fort d'abord), entrée 1, ...
 *
 *   => si l'on remplit ces 16 entrées avec les 256 bits successifs d'un
 *      modulateur ΣΔ fonctionnant à 256·Fs, la broche 2 devient la sortie
 *      1 bit d'un CNA sigma-delta. Un simple filtre RC reconstruit l'audio.
 *
 *   x[n] --> bloqueur ×256 --> ΣΔ ordre 2 --> 16 mots de 16 bits --> TDM2 --> broche 2
 *
 * Câblage (cf. ressources_generales/materiel/branchements.md) : filtre RC passe-bas d'ordre 2,
 * 2e cellule 10 fois plus impédante pour ne pas charger la 1re
 * (chaque cellule : fc = 1/(2πRC) ≈ 34 kHz ; ensemble : -3 dB vers 20 kHz)
 *
 *   broche 2 --[1 kΩ]--+--[10 kΩ]--+---> oscilloscope (sonde ×10)
 *                     4,7 nF      470 pF
 *                      |           |
 *   GND ---------------+-----------+
 *
 *   Tension de sortie : 1,65 V (= mi-alimentation) ± 1,32 V à 0 dBFS.
 *   Ne PAS brancher un casque directement (composante continue, impédance).
 *
 * >>> À VALIDER SUR CARTE : le principe repose sur le code source de
 *     AudioOutputTDM2 (Audio Library de PJRC, version 1.3 de mai 2026).
 */
#ifndef BITSTREAM_GPIO_H
#define BITSTREAM_GPIO_H

#include <Arduino.h>
#include <AudioStream.h>

#define BS_CANAUX 16                         // 16 entrées TDM2 de 16 bits
#define BS_OSR    (BS_CANAUX * 16)           // 256 bits par période Fs

class AudioBitstreamGenerator : public AudioStream {
public:
  AudioBitstreamGenerator() : AudioStream(1, inputQueueArray) {}

  void enable(bool on)   { actif_ = on; }
  void setOrder(int o)   { ordre_ = (o == 1) ? 1 : 2; }

  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];

  volatile bool actif_ = false;
  volatile int  ordre_ = 2;
  int32_t i1_ = 0, i2_ = 0, b_ = 0;           // état du modulateur (Q15)
};

#endif // BITSTREAM_GPIO_H
