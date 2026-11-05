/*
 * TP6 — Écho sur ligne à retard  (VERSION ÉTUDIANT)
 *
 * Sortie LINE OUT (et casque) :  gauche = signal direct + écho
 *                                droite = source non traitée (référence ; déclencher
 *                                         l'oscilloscope sur cette voie)
 *
 * Pour changer de source ou de réglage : modifier setup(), puis téléverser à nouveau.
 * Outils -> USB Type -> « Serial + MIDI + Audio » (musique du PC par USB)
 */
#include <Audio.h>
#include "impulsions.h"
#include "echo.h"

AudioSynthImpulsions   impulsions;     // une impulsion toutes les 100 ms
AudioSynthWaveformSine sinus;          // sinus de test
AudioInputUSB          usb;            // musique du PC
AudioMixer4            source;         // choix de la source (gain 1 sur une seule voie)
AudioEffectEcho        echo;           // écho
AudioOutputI2S         sortie;
AudioControlSGTL5000   codec;

AudioConnection c1(impulsions, 0, source, 0);
AudioConnection c2(sinus,      0, source, 1);
AudioConnection c3(usb,        0, source, 2);
AudioConnection c4(source,     0, echo,   0);
AudioConnection c5(echo,       0, sortie, 0);   // gauche : signal traité
AudioConnection c6(source,     0, sortie, 1);   // droite : référence non traitée

void setup() {
  // Réglages de l'écho
  echo.setDelay(20.0f);                      // retard en ms (moins de 740 ms)
  echo.setGain(0.5f);                        // gain de l'écho (0 .. 0,9)

  // Source : gain 1 sur UNE seule voie
  source.gain(0, 1.0f);                      // impulsions : h[n] à l'oscilloscope
  source.gain(1, 0.0f);                      // sinus
  source.gain(2, 0.0f);                      // musique USB
  sinus.frequency(1000.0f);
  sinus.amplitude(0.5f);

  AudioMemory(20);                           // l'audio démarre ici
  codec.enable();
  codec.volume(0.5f);
}

void loop() {
  // rien à faire : tout se passe dans echo.update()
}
