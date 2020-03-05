
/* Copyright (c) 2015, Human Brain Project
 *                     Daniel.Nachbaur@epfl.ch
 */

#ifndef ZEROEQ_URI_H
#define ZEROEQ_URI_H

#include <zeroeq/api.h>

#include <servus/uri.h> // base class

namespace zeroeq
{
/**
 * Enhances servus::URI to guarantee the existence of a schema and to allow
 * construction of [host][:port] URIs from string.
 */
class URI : private servus::URI
{
public:
    /** Create a default URI in the form "tcp://" */
    ZEROEQ_API URI();

    ZEROEQ_API ~URI();

    ZEROEQ_API URI(const URI& from);

    /** Create URI from string, set schema to "tcp" if empty */
    ZEROEQ_API explicit URI(const std::string& uri);

    /** Create URI from string, set schema to "tcp" if empty */
    ZEROEQ_API explicit URI(const char* uri);

    /** Convert from servus::URI, set schema to "tcp" if empty */
    ZEROEQ_API explicit URI(const servus::URI& from);

    ZEROEQ_API URI& operator=(const URI& rhs);

    /* Convert from servus::URI, set schema to "tcp" if empty */
    ZEROEQ_API URI& operator=(const servus::URI& rhs);

    ZEROEQ_API bool operator==(const URI& rhs) const;

    ZEROEQ_API bool operator==(const servus::URI& rhs) const;

    ZEROEQ_API bool operator!=(const URI& rhs) const;

    ZEROEQ_API bool operator!=(const servus::URI& rhs) const;

    /** Convert this URI to a servus::URI */
    const servus::URI& toServusURI() const { return *this; }

    /** @return true if the host and port are given for a tcp URI. */
    ZEROEQ_API bool isFullyQualified() const;

    /** @name servus::URI API */
    //@{
    using servus::URI::getScheme;
    using servus::URI::getHost;
    using servus::URI::getPort;
    using servus::URI::getPath;
    using servus::URI::getQuery;
    using servus::URI::setHost;
    using servus::URI::setPort;
    //@}
};

inline std::ostream& operator<<(std::ostream& os, const URI& uri)
{
    return os << uri.toServusURI();
}

} // namespace zeroeq

namespace std
{
inline std::string to_string(const zeroeq::URI& uri)
{
    return to_string(uri.toServusURI());
}
}

#endif
