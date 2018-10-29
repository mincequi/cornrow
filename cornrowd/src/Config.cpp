#include "Config.h"

#include <boost/property_tree/json_parser.hpp>
#include <QDebug>

#include <common/Types.h>

#include "audio/Controller.h"

namespace pt = boost::property_tree;

static const std::string audioPath("/var/lib/cornrowd/audio.conf");

void readConfig(audio::Controller& audioController)
{
    pt::ptree tree;

    try {
        pt::read_json(audioPath, tree);
    } catch (...) {
        return;
    }

    std::vector<common::Filter> peq;
    for (const auto& filterNode : tree.get_child("Peq")) {
        common::Filter filter;
        filter.type = static_cast<common::FilterType>(filterNode.second.get<uint>("type"));
        filter.f = filterNode.second.get<float>("f");
        filter.g = filterNode.second.get<float>("g");
        filter.q = filterNode.second.get<float>("q");
        peq.push_back(filter);
    }

    audioController.setPeq(peq);
}

void writeConfig(const audio::Controller& audioController)
{
    pt::ptree tree;
    pt::ptree peq;
    pt::ptree crossover;
    pt::ptree loudness;

    for (const auto& filter : audioController.peq()) {
        pt::ptree child;
        child.put("type", static_cast<uint>(filter.type));
        child.put("f", filter.f);
        child.put("g", filter.g);
        child.put("q", filter.q);

        peq.push_back(std::make_pair("", child));
    }
    tree.add_child("Peq", peq);

    try {
        pt::write_json(audioPath, tree);
    } catch (...) {
        return;
    }
}
