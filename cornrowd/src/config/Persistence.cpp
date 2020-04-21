/*
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Persistence.h"

#include <boost/property_tree/json_parser.hpp>
#include <loguru/loguru.hpp>

namespace config
{

namespace pt = boost::property_tree;

static const std::string audioPath("/var/lib/cornrowd/audio.conf");

Persistence::Persistence()
{
}

Persistence::~Persistence()
{
}

std::vector<common::Filter> Persistence::readConfig()
{
    pt::ptree tree;
    std::vector<common::Filter> filters;

    try {
        pt::read_json(audioPath, tree);
    } catch (...) {
        return {};
    }

    for (const auto& filterNode : tree.get_child("filters", {})) {
        common::Filter filter;
        filter.type = static_cast<common::FilterType>(filterNode.second.get<uint>("t"));
        filter.f = filterNode.second.get<float>("f");
        filter.g = filterNode.second.get<float>("g");
        filter.q = filterNode.second.get<float>("q");
        filters.push_back(filter);
    }

    return filters;
}

void Persistence::writeConfig(const std::vector<common::Filter>& filters)
{
    LOG_F(INFO, "filter count: %d", filters.size());

    pt::ptree root;
    pt::ptree tree;

    for (const auto& filter : filters) {
        pt::ptree child;
        child.put("t", static_cast<uint>(filter.type));
        child.put("f", filter.f);
        child.put("g", filter.g);
        child.put("q", filter.q);

        tree.push_back(std::make_pair("", child));
    }
    root.add_child("filters", tree);

    try {
        pt::write_json(audioPath, root);
    } catch (...) {
        return;
    }
}

} // namespace config
