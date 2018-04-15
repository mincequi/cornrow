#include "RateFileWatcher.h"

#include <iostream>

#include <glibmm/fileutils.h>
#include <glibmm/main.h>

RateFileWatcher::RateFileWatcher(const std::string& filename)
    : m_filename(filename),
      m_sampleRate(0)
{
}

void RateFileWatcher::start()
{
    Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &RateFileWatcher::onTimeout), 1);
}

bool RateFileWatcher::onTimeout()
{
    try {
        std::string strRate = Glib::file_get_contents(m_filename);
        int rate = std::stoi(strRate);
        if (rate != m_sampleRate) {
            m_sampleRate = rate;
            rateChanged(m_sampleRate);
        }
    } catch (...) {
        std::cerr << "error watching file: " << m_filename << std::endl;
        return false;
    }

    return true;
}
