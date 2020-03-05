
/* Copyright (c) 2015-2017, Human Brain Project
 *                          Daniel.Nachbaur@epfl.ch
 */

#ifndef ZEROEQ_DETAIL_CONSTANTS_H
#define ZEROEQ_DETAIL_CONSTANTS_H

#include <servus/uint128_t.h>

#include <string>

namespace
{
const std::string PUBLISHER_SERVICE("_zeroeq_pub._tcp");
const std::string SERVER_SERVICE("_zeroeq_rep._tcp");

const std::string KEY_INSTANCE("Instance");
const std::string KEY_SESSION("Session");
const std::string KEY_USER("User");
const std::string KEY_APPLICATION("Application");

const std::string ENV_SESSION("ZEROEQ_SESSION");
const std::string UNKNOWN_USER("Unknown user");

const std::string DEFAULT_SCHEMA("tcp");

const servus::uint128_t MEERKAT(servus::make_uint128("zeroeq::Meerkat"));
}

#endif
