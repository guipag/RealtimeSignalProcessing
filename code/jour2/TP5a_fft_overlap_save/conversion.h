/*
 * conversion.h — conversion float -> int16 avec saturation  (FOURNI COMPLET)
 *
 * Les calculs se font en float, entre -1 et +1 (échantillon / 32768).
 * Pour revenir au format du bloc audio : × 32768, saturation, arrondi.
 */
#ifndef CONVERSION_H
#define CONVERSION_H

#include <stdint.h>
#include <math.h>

static inline int16_t versInt16(float y) {
  float s = y * 32768.0f;
  if (s >  32767.0f) s =  32767.0f;
  if (s < -32768.0f) s = -32768.0f;
  return (int16_t)lrintf(s);
}

#endif // CONVERSION_H
