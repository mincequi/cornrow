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

namespace config
{

namespace pt = boost::property_tree;

static const std::string audioPath("/var/lib/cornrowd/audio.conf");

void readConfig(std::vector<common::Filter>* peqFilters,
                std::vector<common::Filter>* auxFilters)
{
    pt::ptree tree;

    try {
        pt::read_json(audioPath, tree);
    } catch (...) {
        return;
    }

    for (const auto& filterNode : tree.get_child("peq", {})) {
        common::Filter filter;
        filter.type = static_cast<common::FilterType>(filterNode.second.get<uint>("t"));
        filter.f = filterNode.second.get<float>("f");
        filter.g = filterNode.second.get<float>("g");
        filter.q = filterNode.second.get<float>("q");
        peqFilters->push_back(filter);
    }

    for (const auto& filterNode : tree.get_child("aux", {})) {
        common::Filter filter;
        filter.type = static_cast<common::FilterType>(filterNode.second.get<uint>("t"));
        filter.f = filterNode.second.get<float>("f");
        filter.g = filterNode.second.get<float>("g");
        filter.q = filterNode.second.get<float>("q");
        auxFilters->push_back(filter);
    }
}

void writeConfig(const std::vector<common::Filter>& peqFilters,
                 const std::vector<common::Filter>& auxFilters)
{
    pt::ptree tree;
    pt::ptree peq;
    pt::ptree aux;

    for (const auto& filter : peqFilters) {
        pt::ptree child;
        child.put("t", static_cast<uint>(filter.type));
        child.put("f", filter.f);
        child.put("g", filter.g);
        child.put("q", filter.q);

        peq.push_back(std::make_pair("", child));
    }
    tree.add_child("peq", peq);

    for (const auto& filter : auxFilters) {
        pt::ptree child;
        child.put("t", static_cast<uint>(filter.type));
        child.put("f", filter.f);
        child.put("g", filter.g);
        child.put("q", filter.q);

        aux.push_back(std::make_pair("", child));
    }
    tree.add_child("aux", aux);

    try {
        pt::write_json(audioPath, tree);
    } catch (...) {
        return;
    }
}

} // namespace config
