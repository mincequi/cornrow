#ifndef UTIL_H
#define UTIL_H

#include "Types.h"

bool computeBiQuad(int r, Type t, float f, float g, float q, BiQuad* biquad);
bool computeResponse(Type t, float f, float g, float q, const std::vector<float>& freqs, std::vector<float>* magnitudes , std::vector<float>* phases);

#endif // UTIL_H
