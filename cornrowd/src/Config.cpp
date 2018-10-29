#include "Config.h"

#include <boost/property_tree/json_parser.hpp>
#include <QDebug>

#include <common/Types.h>

#include "audio/Controller.h"

namespace pt = boost::property_tree;

static const std::string audioPath("/var/lib/cornrowd/audio.conf");

void readConfig(audio::Controller& audioController)
{
    pt::ptree root;

    try {
        pt::read_json(audioPath, root);
    } catch (...) {
    }
}

void writeConfig(const audio::Controller& audioController)
{
    qDebug() << "close1";
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

    qDebug() << "close2";
    write_json(audioPath, tree);
    qDebug() << "close3";
}
