#ifndef CORNROWDOPTIONGROUP_H
#define CORNROWDOPTIONGROUP_H

#include <glibmm/optiongroup.h>

#include "common/Types.h"

class CornrowdOptionGroup : public Glib::OptionGroup
{
    friend int main(int argc, char *argv[]);

public:
    CornrowdOptionGroup();

private:
    bool onRate(const Glib::ustring& /*option_name*/, const std::string& value, bool has_value);
    bool onFormat(const Glib::ustring& /*option_name*/, const std::string& value, bool has_value);
    bool onWatchFilename(const Glib::ustring& /*option_name*/, const std::string& value, bool has_value);

    bool    m_daemonize = false;
    int     m_rate      = 44100;
    SampleFormat    m_format = SampleFormat::I16;
    std::string     m_watchFilename;
};

#endif // CORNROWDOPTIONGROUP_H
