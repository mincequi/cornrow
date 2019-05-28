#pragma once

#include <common/Types.h>

namespace config
{

std::vector<common::Filter> readPeq();
void writeConfig(const std::vector<common::Filter>& filters);

} // namespace config
