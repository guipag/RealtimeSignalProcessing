/*
 * A2 — Quantification, mise en forme du bruit, CAN et CNA sigma-delta 1 bit
 * (TP avancé, VERSION ÉTUDIANT — fichier complet)
 *
 *   source --> [requantification B bits + dither + mise en forme]   (partie 1)
 *          --> [CAN ΣΔ 1 bit simulé : modulateur + CIC]             (partie 2)
 *          --> casque (G et D) + mesure spectrale (port série)
 *   source --> [CNA ΣΔ 1 bit à 256·Fs] --> AudioOutputTDM2 --> broche 2  (partie 3)
 *
 * Paramètres pilotés en MIDI (pupitre : ressources_generales/logiciels/pupitre_midi.html, preset "A2") :
 *   CC 21 = niveau de la source (-60..0 dBFS)     CC 81 = source USB (sinon sinus)
 *   CC 24 = fréquence du sinus (200..10000 Hz)    CC 82 = dither TPDF
 *   CC 25 = bits B : 2, 3, 4, 6, 8, 12, 16        CC 83 = flux 1 bit sur la broche 2
 *   CC 26 = mise en forme du bruit : ordre 0, 1, 2
 *   CC 27 = CAN sigma-delta : arrêt, ordre 1, ordre 2
 *   CC 28 = OSR : 16, 32, 64
 *   CC 29 = bande de mesure du SNR (1..20 kHz, log)
 *
 * >>> Outils -> USB Type -> "Serial + MIDI + Audio"
 */
#include <Audio.h>
#include <Wire.h>
#include "effect_requantizer.h"
#include "effect_sigmadelta_adc.h"
#include "bitstream_gpio.h"
#include "mesure.h"
#include "midi_map.h"

AudioSynthSineCoherent   sinus;
AudioInputUSB            usb;
AudioMixer4              source;      // voie 0 : sinus, voie 1 : USB gauche
AudioEffectRequantizer   requant;
AudioEffectSigmaDeltaADC can;
AudioRecordQueue         file;
AudioOutputI2S           out;
AudioControlSGTL5000     sgtl;
AudioBitstreamGenerator  bitstream;
AudioOutputTDM2          tdm2;        // SAI2 : broche 2 = données, 3 = FS, 4 = BCLK

AudioConnection c1(sinus,   0, source,  0);
AudioConnection c2(usb,     0, source,  1);
AudioConnection c3(source,  0, requant, 0);
AudioConnection c4(requant, 0, can,     0);
AudioConnection c5(can,     0, out,     0);
AudioConnection c6(can,     0, out,     1);
AudioConnection c7(can,     0, file,    0);
AudioConnection c8(source,  0, bitstream, 0);

// 16 sorties du générateur de flux -> 16 entrées de la sortie TDM2
AudioConnection t0 (bitstream,  0, tdm2,  0), t1 (bitstream,  1, tdm2,  1);
AudioConnection t2 (bitstream,  2, tdm2,  2), t3 (bitstream,  3, tdm2,  3);
AudioConnection t4 (bitstream,  4, tdm2,  4), t5 (bitstream,  5, tdm2,  5);
AudioConnection t6 (bitstream,  6, tdm2,  6), t7 (bitstream,  7, tdm2,  7);
AudioConnection t8 (bitstream,  8, tdm2,  8), t9 (bitstream,  9, tdm2,  9);
AudioConnection t10(bitstream, 10, tdm2, 10), t11(bitstream, 11, tdm2, 11);
AudioConnection t12(bitstream, 12, tdm2, 12), t13(bitstream, 13, tdm2, 13);
AudioConnection t14(bitstream, 14, tdm2, 14), t15(bitstream, 15, tdm2, 15);

MesureSpectrale mes;

const int BITS[7]  = {2, 3, 4, 6, 8, 12, 16};
const int OSRS[3]  = {16, 32, 64};
float curNiveau = -6.0f, curFreq = 1000.0f, curBande = 4000.0f;
int   curBits = 16, curNS = 0, curSD = 0, curOSR = 64;
bool  srcUSB = false, dither = false, flux = false;
bool  changement = false;

void appliquerSource() {
  float g = powf(10.0f, curNiveau / 20.0f);
  sinus.amplitude(srcUSB ? 0.0f : g);
  source.gain(0, 1.0f);
  source.gain(1, srcUSB ? g : 0.0f);
}

void onControlChange(byte canal, byte cc, byte val) {
  switch (cc) {
    case 21: curNiveau = midiLin(val, -60.0f, 0.0f); appliquerSource(); break;
    case 24: curFreq = sinus.frequency(midiLog(val, 200.0f, 10000.0f)); break;
    case 25: curBits = BITS[midiStep(val, 7)]; requant.setBits(curBits); break;
    case 26: curNS = midiStep(val, 3); requant.setShaping(curNS); break;
    case 27: curSD = midiStep(val, 3); can.setOrder(curSD); break;
    case 28: curOSR = OSRS[midiStep(val, 3)]; can.setOSR(curOSR); break;
    case 29: curBande = midiLog(val, 1000.0f, 20000.0f); break;
    case 81: srcUSB = midiBool(val); appliquerSource(); break;
    case 82: dither = midiBool(val); requant.setDither(dither); break;
    case 83: flux = midiBool(val); bitstream.enable(flux); break;
  }
  changement = true;
}

void setup() {
  Serial.begin(115200);
  AudioMemory(120);                 // file de mesure + 16 blocs pour TDM2

  sgtl.enable();
  sgtl.volume(0.4);

  curFreq = sinus.frequency(curFreq);
  appliquerSource();
  requant.setBits(curBits);
  can.setOSR(curOSR);

  mes.begin(file);
  usbMIDI.setHandleControlChange(onControlChange);
}

void loop() {
  usbMIDI.read();

  if (changement) {
    changement = false;
    mes.relancer();
  }

  if (mes.collecter()) {
    mes.analyser(sinus.bin(), curBande);

    Serial.print("B=");          Serial.print(curBits);
    Serial.print(dither ? " +dither" : "        ");
    Serial.print(" NS=");        Serial.print(curNS);
    Serial.print(" | CAN SD ");
    if (curSD == 0) Serial.print("arret     ");
    else { Serial.print("o"); Serial.print(curSD); Serial.print(" OSR "); Serial.print(curOSR); }

    if (srcUSB) {
      Serial.print(" | source USB : pas de mesure");
    } else {
      Serial.print(" | ");         Serial.print(mes.niveau_dBFS, 1);
      Serial.print(" dBFS  SINAD ");  Serial.print(mes.sinad, 1);
      Serial.print("  SNR ");      Serial.print(mes.snr, 1);
      Serial.print("  THD ");      Serial.print(mes.thd, 1);
      Serial.print("  SNR[0-");    Serial.print(curBande / 1000.0f, 1);
      Serial.print("k] ");         Serial.print(mes.snrBande, 1);
      Serial.print(" dB  ENOB ");  Serial.print(mes.enob, 1);
    }
    Serial.print(" | CPU ");     Serial.print(AudioProcessorUsageMax(), 1);
    Serial.print(" %  mem ");    Serial.print(AudioMemoryUsageMax());
    Serial.println(flux ? "  | FLUX 1 BIT ACTIF (broche 2)" : "");

    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
    mes.relancer();
  }
}
