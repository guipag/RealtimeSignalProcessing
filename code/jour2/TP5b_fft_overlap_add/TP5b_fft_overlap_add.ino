/*
 * TP5b — Convolution rapide par FFT : overlap-add  (VERSION ÉTUDIANT)
 *
 * Sortie LINE OUT (et casque) :  gauche = source filtrée par h
 *                                droite = source non traitée (référence ; déclencher
 *                                         l'oscilloscope sur cette voie)
 * Moniteur série : charge processeur, une fois par seconde.
 *
 * Pour changer de source ou de réglage : modifier setup(), puis téléverser à nouveau.
 * Outils -> USB Type -> « Serial + MIDI + Audio » (musique du PC par USB)
 */
#include <Audio.h>
#include "impulsions.h"
#include "noyaux.h"
#include "conv_fft_ola.h"

AudioSynthImpulsions   impulsions;     // une impulsion toutes les 100 ms
AudioSynthWaveformSine sinus;          // sinus de test
AudioInputUSB          usb;            // musique du PC
AudioMixer4            source;         // choix de la source (gain 1 sur une seule voie)
AudioFilterConvFFT     conv;           // convolution par FFT
AudioOutputI2S         sortie;
AudioControlSGTL5000   codec;

AudioConnection c1(impulsions, 0, source, 0);
AudioConnection c2(sinus,      0, source, 1);
AudioConnection c3(usb,        0, source, 2);
AudioConnection c4(source,     0, conv,   0);
AudioConnection c5(conv,       0, sortie, 0);   // gauche : signal traité
AudioConnection c6(source,     0, sortie, 1);   // droite : référence non traitée

float h[CONV_MMAX];

void setup() {
  Serial.begin(115200);

  // Réponse impulsionnelle : garder UNE ligne
  int M = noyauRetard(h, 2000);              // retard de 2000 échantillons
  // int M = noyauEcho(h, 2000, 0.5f);       // écho : 2000 échantillons, gain 0,5
  // int M = noyauMoyenne(h, 64);            // moyenne sur 64 points
  // int M = noyauPasseBas(h, 255, 1000.0f); // passe-bas 1 kHz, 255 coefficients
  // int M = noyauSalle(h, 3968);            // salle, 3968 coefficients
  conv.setKernel(h, M);                    // calcule H = FFT(h)

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
  Serial.print("M = ");
  Serial.print(conv.length());
  Serial.print("   N = ");
  Serial.print(conv.fftSize());
  Serial.print("   CPU = ");
  Serial.print(conv.processorUsageMax());          // % du temps d'un bloc (2,9 ms)
  Serial.println(" %");
  conv.processorUsageMaxReset();
  delay(1000);
}
