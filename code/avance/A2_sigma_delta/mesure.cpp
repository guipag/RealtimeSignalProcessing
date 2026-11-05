/*
 * mesure.cpp — banc de mesure spectrale embarqué (FOURNI COMPLET)
 */
#include "mesure.h"
#include <math.h>
#include <string.h>

// ===========================================================================
// Générateur de sinus cohérent
// ===========================================================================
float AudioSynthSineCoherent::frequency(float f) {
  int m = (int)lroundf(f * MES_N / AUDIO_SAMPLE_RATE_EXACT);
  m |= 1;                                   // m impair  =>  pgcd(m, N) = 1
  if (m < 1) m = 1;
  if (m > MES_N / 2 - 1) m = MES_N / 2 - 1;
  AudioNoInterrupts();
  m_   = (uint32_t)m;
  inc_ = (uint32_t)m << (32 - MES_LOG2N);   // exactement m·2^32/N : aucune dérive
  AudioInterrupts();
  return m * AUDIO_SAMPLE_RATE_EXACT / MES_N;
}

void AudioSynthSineCoherent::update(void) {
  audio_block_t *block = allocate();
  if (!block) return;

  const float k = 6.283185307f / 4294967296.0f;  // phase (0..2^32) -> radians
  uint32_t ph = phase_, inc = inc_;
  float a = amp_ * 32767.0f;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    block->data[i] = (int16_t)lrintf(a * sinf((float)ph * k));
    ph += inc;                               // débordement modulo 2^32 voulu
  }
  phase_ = ph;

  transmit(block);
  release(block);
}

// ===========================================================================
// Mesure spectrale
// ===========================================================================
void MesureSpectrale::begin(AudioRecordQueue &q) {
  q_ = &q;
  for (int k = 0; k < MES_N / 2; k++) {
    cosT_[k] = cosf(6.283185307f * k / MES_N);
    sinT_[k] = sinf(6.283185307f * k / MES_N);
  }
  relancer();
}

void MesureSpectrale::relancer(int ignorer) {
  if (!q_) return;
  q_->end();
  q_->clear();
  rempli_  = 0;
  ignorer_ = ignorer;
  actif_   = true;
  q_->begin();
}

bool MesureSpectrale::collecter() {
  if (!q_ || !actif_) return false;

  while (q_->available() > 0) {
    int16_t *p = q_->readBuffer();
    if (ignorer_ > 0) {
      ignorer_--;
    } else if (rempli_ < MES_N) {
      memcpy(buf_ + rempli_, p, AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
      rempli_ += AUDIO_BLOCK_SAMPLES;
    }
    q_->freeBuffer();
  }

  if (rempli_ >= MES_N) {        // capture complète : on libère la mémoire audio
    q_->end();
    q_->clear();
    actif_ = false;
    return true;
  }
  return false;
}

// FFT complexe radix-2, décimation temporelle, en place
void MesureSpectrale::fft() {
  const int N = MES_N;

  // 1) Permutation par inversion de bits
  for (int i = 1, j = 0; i < N; i++) {
    int bit = N >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) {
      float t = re_[i]; re_[i] = re_[j]; re_[j] = t;
      t = im_[i]; im_[i] = im_[j]; im_[j] = t;
    }
  }

  // 2) Papillons
  for (int len = 2; len <= N; len <<= 1) {
    int half = len >> 1, step = N / len;
    for (int i = 0; i < N; i += len) {
      for (int k = 0; k < half; k++) {
        float wr =  cosT_[k * step];
        float wi = -sinT_[k * step];         // exp(-j·2πk/len)
        int a = i + k, b = a + half;
        float tr = re_[b] * wr - im_[b] * wi;
        float ti = re_[b] * wi + im_[b] * wr;
        re_[b] = re_[a] - tr;  im_[b] = im_[a] - ti;
        re_[a] += tr;          im_[a] += ti;
      }
    }
  }
}

static inline float dB10(double r) {
  if (r <= 1e-30) return -300.0f;
  return (float)(10.0 * log10(r));
}

void MesureSpectrale::analyser(uint32_t m, float bandeHz) {
  const int N = MES_N, H = MES_N / 2;

  for (int n = 0; n < N; n++) { re_[n] = buf_[n] / 32768.0f; im_[n] = 0.0f; }
  fft();

  // Puissance par case (cases 1..N/2-1 comptées deux fois : spectre bilatéral)
  auto P = [&](int k) -> double {
    double p = (double)re_[k] * re_[k] + (double)im_[k] * im_[k];
    return (k == H) ? p : 2.0 * p;
  };

  int kb = (int)(bandeHz * N / AUDIO_SAMPLE_RATE_EXACT);
  if (kb > H) kb = H;

  // Cases des harmoniques 2..10, repliées dans [0, N/2]
  int harm[9];
  for (int h = 2; h <= 10; h++) {
    int b = (int)(((uint32_t)h * m) % N);
    if (b > H) b = N - b;
    harm[h - 2] = b;
  }

  double ps = P(m), pd = 0.0, pn = 0.0, pnb = 0.0, pire = 0.0;
  int kPire = 0;
  for (int k = 1; k <= H; k++) {
    if (k == (int)m) continue;
    double p = P(k);
    bool estHarm = false;
    for (int i = 0; i < 9; i++) if (k == harm[i]) estHarm = true;
    if (estHarm) pd += p;
    else {
      pn += p;
      if (k <= kb) pnb += p;
    }
    // Raie non harmonique = case qui n'est pas un multiple de m
    if ((k % m) != 0 && p > pire) { pire = p; kPire = k; }
  }

  // Amplitude du sinus : A = sqrt(2·Ps) (Ps = A²/2), normalisée à N²
  double ps1 = ps / ((double)N * N);
  niveau_dBFS = dB10(2.0 * ps1);
  sinad       = dB10(ps / (pn + pd));
  snr         = dB10(ps / pn);
  thd         = dB10(pd / ps);
  snrBande    = dB10(ps / pnb);
  enob        = (sinad - 1.76f - niveau_dBFS) / 6.02f;
  pireRaie    = dB10(pire / ps);
  pireRaieHz  = kPire * AUDIO_SAMPLE_RATE_EXACT / N;
}
