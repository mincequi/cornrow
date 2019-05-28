#include "Persistence.h"

#include <boost/property_tree/json_parser.hpp>

namespace config
{

namespace pt = boost::property_tree;

static const std::string audioPath("/var/lib/cornrowd/audio.conf");

std::vector<common::Filter> readPeq()
{
    pt::ptree tree;

    try {
        pt::read_json(audioPath, tree);
    } catch (...) {
        return {};
    }

    std::vector<common::Filter> peq;
    for (const auto& filterNode : tree.get_child("peq")) {
        common::Filter filter;
        filter.type = static_cast<common::FilterType>(filterNode.second.get<uint>("t"));
        filter.f = filterNode.second.get<float>("f");
        filter.g = filterNode.second.get<float>("g");
        filter.q = filterNode.second.get<float>("q");
        peq.push_back(filter);
    }

    return peq;
}

void writeConfig(const std::vector<common::Filter>& filters)
{
    pt::ptree tree;
    pt::ptree peq;
    pt::ptree crossover;
    pt::ptree loudness;

    for (const auto& filter : filters) {
        pt::ptree child;
        child.put("t", static_cast<uint>(filter.type));
        child.put("f", filter.f);
        child.put("g", filter.g);
        child.put("q", filter.q);

        peq.push_back(std::make_pair("", child));
    }
    tree.add_child("peq", peq);

    try {
        pt::write_json(audioPath, tree);
    } catch (...) {
        return;
    }
}

} // namespace config
