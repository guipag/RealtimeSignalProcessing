/*
 * effect_requantizer.cpp — requantification + dither + mise en forme  (VERSION ÉTUDIANT)
 */
#include "effect_requantizer.h"
#include <math.h>

void AudioEffectRequantizer::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;

  const int   B     = bits_;
  const bool  dith  = dither_;     // utilisé en Q4
  const int   ordre = ordre_;
  const float q     = 2.0f / (float)(1 << B);   // pas de quantification
  const float yMax  = 1.0f - q;                 // plus grand niveau représentable
  float e1 = e1_, e2 = e2_;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float x = block->data[i] / 32768.0f;

    // TODO (Q2) : quantification « mid-tread » au pas q :
    //             yq = q · arrondi(v / q)   (arrondi(u) = floorf(u + 0.5f))
    // TODO (Q4) : ajouter avant l'arrondi un dither TPDF d = q·(aleatoire() - aleatoire())
    //             si dith est vrai
    // TODO (Q6) : mise en forme du bruit — partir de v = x - (retour d'erreur) :
    //             ordre 1 : v = x - e1          ordre 2 : v = x - 2·e1 + e2
    //             puis, après quantification : e = yq - v ; e2 = e1 ; e1 = e
    float v  = x;                    // provisoire
    float yq = v;                    // provisoire : aucune quantification

    // Saturation à la plage représentable (fournie)
    float y = yq;
    if      (y >  yMax) y =  yMax;
    else if (y < -1.0f) y = -1.0f;

    // y est un multiple de q = 2^(1-B) >= 2^-15 : conversion int16 exacte
    block->data[i] = (int16_t)lrintf(y * 32768.0f);
  }

  e1_ = e1; e2_ = e2;
  transmit(block);
  release(block);
}
