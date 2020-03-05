
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *                          Stefan.Eilemann@epfl.ch
 */

#pragma once

#include "../log.h"
#include "constants.h"

#include <zeroeq/uri.h>

#include <cstring>
#include <sstream>

// getlogin()
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <Lmcons.h>
#include <windows.h>
#else
#include <climits>
#include <unistd.h>
#endif

#if __APPLE__
#include <dirent.h>
#include <mach-o/dyld.h>
#endif

namespace
{
inline std::string buildZmqURI(const std::string& schema, std::string host,
                               const uint16_t port)
{
    if (host.empty())
        host = "*";

    const std::string zmqURI(schema + "://" + host);
    if (port == 0) // zmq expects host:* instead of host:0
        return zmqURI + ":*";

    return zmqURI + ":" + std::to_string(int(port));
}

inline std::string buildZmqURI(const zeroeq::URI& uri)
{
    if (uri.getScheme() == DEFAULT_SCHEMA)
        return buildZmqURI(uri.getScheme(), uri.getHost(), uri.getPort());
    return std::to_string(uri);
}

inline std::string getUserName()
{
#ifdef _MSC_VER
    char user[UNLEN + 1];
    DWORD userLength = UNLEN + 1;
    GetUserName(user, &userLength);
#else
    const char* user = getlogin();
#endif
    return user ? user : UNKNOWN_USER;
}

inline std::string getApplicationName()
{
// http://stackoverflow.com/questions/933850
#ifdef _MSC_VER
    char result[MAX_PATH];
    const std::string execPath(result,
                               GetModuleFileName(NULL, result, MAX_PATH));
#elif __APPLE__
    char result[PATH_MAX + 1];
    uint32_t size = sizeof(result);
    if (_NSGetExecutablePath(result, &size) != 0)
        return std::string();
    const std::string execPath(result);
#else
    char result[PATH_MAX];
    const ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count < 0)
    {
        // Not all UNIX have /proc/self/exe
        ZEROEQWARN << "Could not find absolute executable path" << std::endl;
        return std::string();
    }
    const std::string execPath(result, count > 0 ? count : 0);
#endif

#ifdef _MSC_VER
    const size_t lastSeparator = execPath.find_last_of('\\');
#else
    const size_t lastSeparator = execPath.find_last_of('/');
#endif
    if (lastSeparator == std::string::npos)
        return execPath;
    // lastSeparator + 1 may be at most equal to filename.size(), which is good
    return execPath.substr(lastSeparator + 1);
}

inline std::string getDefaultPubSession()
{
    const char* pubSession = getenv(zeroeq::ENV_PUB_SESSION.c_str());
    const char* session = getenv(ENV_SESSION.c_str());
    if (session)
        ZEROEQWARN << "Found deprecated " << ENV_SESSION
                   << " in environment, please use " << zeroeq::ENV_PUB_SESSION
                   << std::endl;

    if (pubSession && strcmp(pubSession, "") != 0)
        return pubSession;
    if (session && strcmp(session, "") != 0)
        return session;
    return getUserName();
}

inline std::string getDefaultRepSession()
{
    const char* session = getenv(zeroeq::ENV_REP_SESSION.c_str());
    return session && strcmp(session, "") != 0 ? session : getApplicationName();
}
}
