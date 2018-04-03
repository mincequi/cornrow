#ifndef UTIL_H
#define UTIL_H

#include "Types.h"

bool computeBiQuad(int r, const Filter& filter, BiQuad* biquad);
bool computeResponse(const Filter& filter, const std::vector<float>& freqs, std::vector<float>* magnitudes, std::vector<float>* phases);

#endif // UTIL_H
