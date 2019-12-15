#pragma once

#include "Types.h"

namespace common
{

bool computeBiQuad(int r, const Filter& filter, BiQuad* biquad);
/*
bool computeResponse(const Filter& filter, const std::vector<float>& freqs, std::vector<float>* magnitudes, std::vector<float>* phases);
*/

} // namespace common
