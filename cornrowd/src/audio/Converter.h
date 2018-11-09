#pragma once

#include <common/Types.h>
#include <Types.h>

namespace audio
{

std::vector<common::Filter> fromGstDsp(const std::vector<GstDsp::Filter>& in);
std::vector<GstDsp::Filter> toGstDsp(const std::vector<common::Filter>& in);

} // namespace audio
