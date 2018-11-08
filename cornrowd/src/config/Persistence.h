#pragma once

#include <common/Types.h>

namespace config
{

std::vector<common::Filter> readConfig();
void writeConfig(const std::vector<common::Filter>& filters);

} // namespace config
