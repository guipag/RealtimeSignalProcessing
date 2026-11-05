/*
 * Projet — Compresseur-limiteur  (VERSION ÉTUDIANT)
 *
 * Sortie LINE OUT (et casque) :  gauche = signal traité
 *                                droite = source non traitée (référence ; déclencher
 *                                         l'oscilloscope sur cette voie ; entrée X en mode XY)
 * Source : sinus de test, en salves (le niveau alterne entre NIVEAU_BAS et NIVEAU_HAUT
 *          toutes les DUREE_SALVE ms) ou à niveau fixe ; ou musique du PC par USB.
 *
 * Tous les réglages se font depuis le moniteur série, sans recompiler (115 200 bauds,
 * fin de ligne « Nouvelle ligne ») :
 *   amplitude -20      sinus de niveau fixe, en dBFS (arrête les salves)
 *   salves             reprend les salves
 *   frequence 1000     fréquence du sinus en Hz
 *   musique / sinus    choix de la source
 *   seuil -20          seuil T, en dBFS
 *   taux 4             taux de compression R
 *   attaque 5          constante de temps d'attaque, en ms
 *   relachement 100    constante de temps de relâchement, en ms
 *   compresseur        réglages du compresseur (T = -20 dBFS, R = 4, 5 ms, 100 ms)
 *   limiteur           réglages du limiteur (T = -20 dBFS, R = 100, 0,01 ms, 0,1 ms)
 *   reglages           affiche les réglages en cours
 * Outils -> USB Type -> « Serial + MIDI + Audio »
 */
#include <Audio.h>
#include "commandes.h"
#include "compresseur.h"

// Salves de sinus
const float NIVEAU_BAS  = -30.0f;      // dBFS
const float NIVEAU_HAUT = -6.0f;       // dBFS
const int   DUREE_SALVE = 300;         // ms

AudioSynthWaveformSine sinus;          // sinus de test
AudioInputUSB          usb;            // musique du PC
AudioMixer4            source;         // choix de la source (gain 1 sur une seule voie)
AudioEffectCompresseur comp;
AudioOutputI2S         sortie;
AudioControlSGTL5000   codec;

AudioConnection c1(sinus,  0, source, 0);
AudioConnection c2(usb,    0, source, 1);
AudioConnection c3(source, 0, comp,   0);
AudioConnection c4(comp,   0, sortie, 0);   // gauche : signal traité
AudioConnection c5(source, 0, sortie, 1);   // droite : référence non traitée

bool salves = true;

void reglerCompresseur(float T, float R, float att, float rel) {
  comp.setSeuil(T); comp.setTaux(R); comp.setAttaque(att); comp.setRelachement(rel);
}

void afficherReglages() {
  Serial.print("seuil "); Serial.print(comp.seuil()); Serial.print(" dBFS | taux "); Serial.print(comp.taux());
  Serial.print(" | attaque "); Serial.print(comp.attaque(), 3); Serial.print(" ms | relachement ");
  Serial.print(comp.relachement(), 3); Serial.println(" ms");
}

void setup() {
  Serial.begin(115200);
  reglerCompresseur(-20.0f, 4.0f, 5.0f, 100.0f);

  // Source : gain 1 sur UNE seule voie
  source.gain(0, 1.0f);                      // sinus
  source.gain(1, 0.0f);                      // musique USB
  sinus.frequency(1000.0f);
  sinus.amplitude(dBversLineaire(NIVEAU_BAS));

  AudioMemory(20);                           // l'audio démarre ici
  codec.enable();
  codec.volume(0.9f);
}

void loop() {
  // Salves : on change le niveau du sinus toutes les DUREE_SALVE ms
  static elapsedMillis t;
  static bool haut = false;
  if (salves && t >= DUREE_SALVE) {
    t = 0;
    haut = !haut;
    sinus.amplitude(dBversLineaire(haut ? NIVEAU_HAUT : NIVEAU_BAS));
  }

  // Commandes du moniteur série
  String mot; float v;
  if (lireCommande(mot, &v)) {
    if      (mot == "amplitude")   { salves = false; sinus.amplitude(dBversLineaire(v)); }
    else if (mot == "salves")      { salves = true; }
    else if (mot == "frequence")   { sinus.frequency(v); }
    else if (mot == "musique")     { source.gain(0, 0.0f); source.gain(1, 1.0f); }
    else if (mot == "sinus")       { source.gain(0, 1.0f); source.gain(1, 0.0f); }
    else if (mot == "seuil")       { comp.setSeuil(v); }
    else if (mot == "taux")        { comp.setTaux(v); }
    else if (mot == "attaque")     { comp.setAttaque(v); }
    else if (mot == "relachement") { comp.setRelachement(v); }
    else if (mot == "compresseur") { reglerCompresseur(-20.0f, 4.0f, 5.0f, 100.0f); }
    else if (mot == "limiteur")    { reglerCompresseur(-20.0f, 100.0f, 0.01f, 0.1f); }
    else if (mot != "reglages")    { Serial.println("commande inconnue"); }
    afficherReglages();
  }

  // Charge processeur, toutes les 2 s
  static elapsedMillis tCpu;
  if (tCpu >= 2000) {
    tCpu = 0;
    Serial.print("CPU compresseur = ");
    Serial.print(comp.processorUsageMax());
    Serial.println(" %");
    comp.processorUsageMaxReset();
  }
}
