/*
 * effect_gain.h — objet de gain custom pour l'Audio Library  (VERSION ÉTUDIANT)
 *
 * Classe AudioEffectGain héritant d'AudioStream : applique un gain contrôlable,
 * avec mute et bypass. Traitement échantillon par échantillon dans update().
 */
#ifndef EFFECT_GAIN_H
#define EFFECT_GAIN_H

#include <Arduino.h>
#include <AudioStream.h>

#define NBVOIES 1

class AudioEffectGain : public AudioStream {
public:
  // 1 entrée : on passe le nombre d'entrées + le tableau de files à AudioStream.
  AudioEffectGain() : AudioStream(NBVOIES, inputQueueArray) {}

  // Règle le gain linéaire (1.0 = 0 dB). Appelé depuis loop().
  void setGain(float g)      { gain_   = g; }

  // Court-circuit : le signal passe inchangé (comparaison A/B).
  void setBypass(bool b)     { bypass_ = b; }

  // Mute : sortie silencieuse (sans couper le flux).
  void mute(bool m)          { muted_  = m; }

  // Méthode appelée par le moteur audio à chaque bloc.
  virtual void update(void);

private:
  audio_block_t *inputQueueArray[NBVOIES];
  volatile float gain_   = 1.0f;   // partagé loop()/update() -> volatile
  volatile bool  bypass_ = false;
  volatile bool  muted_  = false;
};

#endif // EFFECT_GAIN_H
