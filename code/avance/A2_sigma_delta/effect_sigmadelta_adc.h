/*
 * effect_sigmadelta_adc.h — simulation d'un CAN sigma-delta 1 bit
 * TP avancé A2, partie 2  (VERSION ÉTUDIANT — fichier complet)
 *
 *  x[n] (« analogique »)
 *    --> bloqueur d'ordre 0 ×OSR          (on « échantillonne » OSR fois plus vite)
 *    --> modulateur ΣΔ 1 bit d'ordre 1 ou 2 (sortie b = ±1)
 *    --> filtre CIC d'ordre K = ordre + 1, décimation par R = OSR
 *    --> y[n] à Fs
 *
 *  Modulateur d'ordre 1 :  i1 += x - b ;            b = signe(i1)
 *  Modulateur d'ordre 2 :  i1 += x - b ; i2 += i1 - b ; b = signe(i2)
 *  (calcul en ENTIERS Q15 : x et b = ±32768 ; |i1|, |i2| < 4·32768)
 *
 *  CIC (Hogenauer) : K intégrateurs à OSR·Fs, puis K dérivateurs (peignes)
 *  à Fs. Gain continu R^K (64^3 = 262144) : arithmétique entière MODULAIRE
 *  sur uint32_t, les débordements des intégrateurs se compensent exactement.
 */
#ifndef EFFECT_SIGMADELTA_ADC_H
#define EFFECT_SIGMADELTA_ADC_H

#include <Arduino.h>
#include <AudioStream.h>

#define SD_KMAX 3                            // ordre CIC maximal (= ordre ΣΔ + 1)

class AudioEffectSigmaDeltaADC : public AudioStream {
public:
  AudioEffectSigmaDeltaADC() : AudioStream(1, inputQueueArray) {}

  // 0 = arrêt (le signal traverse sans modification), 1 ou 2 = ordre du modulateur
  void setOrder(int o) { ordreReq_ = o < 0 ? 0 : (o > 2 ? 2 : o); }
  void setOSR(int r)   { osrReq_ = r; }        // 16, 32 ou 64

  int  order() const   { return ordre_; }
  int  osr() const     { return osr_; }
  float densiteUns() const { return densite_; }  // proportion de « 1 » du dernier bloc

  virtual void update(void);

private:
  void reinitialiser();

  audio_block_t *inputQueueArray[1];

  volatile int ordreReq_ = 0, osrReq_ = 64;
  int   ordre_ = 0, osr_ = 64;

  int32_t  i1_ = 0, i2_ = 0, b_ = 0;              // état du modulateur (Q15)
  uint32_t integ_[SD_KMAX]  = {0};                    // intégrateurs CIC (OSR·Fs)
  uint32_t peigne_[SD_KMAX] = {0};                    // mémoires des peignes (Fs)
  float    densite_ = 0.5f;
};

#endif // EFFECT_SIGMADELTA_ADC_H
