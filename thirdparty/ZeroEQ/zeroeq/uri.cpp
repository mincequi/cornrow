
/* Copyright (c) 2015, Human Brain Project
 *                     Daniel.Nachbaur@epfl.ch
 */

#include "uri.h"
#include "detail/constants.h"
#include "log.h"

#include <cassert>

namespace zeroeq
{
namespace
{
servus::URI createURI(std::string string)
{
    servus::URI uri(string);
    if (uri.getScheme().empty())
    {
        if (string.empty() || string[0] == ':')
            string = std::string("*") + string;
        return servus::URI(DEFAULT_SCHEMA + "://" + string);
    }

    return uri;
}
}

URI::URI()
    : servus::URI()
{
    setScheme(DEFAULT_SCHEMA);
}

URI::~URI()
{
}

URI::URI(const std::string& uri)
    : servus::URI(createURI(uri.c_str()))
{
}

URI::URI(const char* uri)
    : servus::URI(createURI(uri))
{
}

URI::URI(const URI& from)
    : servus::URI(from)
{
    assert(!getScheme().empty());
}

URI::URI(const servus::URI& from)
    : servus::URI(from)
{
    if (getScheme().empty())
        setScheme(DEFAULT_SCHEMA);
}

URI& URI::operator=(const URI& rhs)
{
    if (this == &rhs)
        return *this;
    servus::URI::operator=(rhs);
    assert(!getScheme().empty());
    return *this;
}

URI& URI::operator=(const servus::URI& rhs)
{
    servus::URI::operator=(rhs);
    if (getScheme().empty())
        setScheme(DEFAULT_SCHEMA);
    return *this;
}

bool URI::operator==(const URI& rhs) const
{
    return servus::URI::operator==(rhs);
}

bool URI::operator==(const servus::URI& rhs) const
{
    return servus::URI::operator==(rhs);
}

bool URI::operator!=(const URI& rhs) const
{
    return servus::URI::operator!=(rhs);
}

bool URI::operator!=(const servus::URI& rhs) const
{
    return servus::URI::operator!=(rhs);
}

bool URI::isFullyQualified() const
{
    return getScheme() != DEFAULT_SCHEMA ||
           (!getHost().empty() && getPort() != 0);
}
}
