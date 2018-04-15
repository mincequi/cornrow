#include "CornrowdOptionGroup.h"

#include <algorithm>

#include <glibmm/fileutils.h>
#include <glibmm/optionentry.h>

CornrowdOptionGroup::CornrowdOptionGroup()
    : Glib::OptionGroup("cornrowd options", "description of cornrowd options")
{
    Glib::OptionEntry entry1;
    entry1.set_long_name("daemonize");
    entry1.set_short_name('d');
    entry1.set_description("Start as daemon");
    add_entry(entry1, m_daemonize);

    Glib::OptionEntry entry2;
    entry2.set_long_name("samplerate");
    entry2.set_short_name('r');
    entry2.set_description("The sample rate [44100|48000]");
    entry2.set_flags(Glib::OptionEntry::Flags::FLAG_OPTIONAL_ARG);
    add_entry(entry2, sigc::mem_fun(*this, &CornrowdOptionGroup::onRate));

    Glib::OptionEntry entry3;
    entry3.set_long_name("sampleformat");
    entry3.set_short_name('f');
    entry3.set_description("The sample format [i16]");
    entry3.set_flags(Glib::OptionEntry::Flags::FLAG_OPTIONAL_ARG);
    add_entry(entry3, sigc::mem_fun(*this, &CornrowdOptionGroup::onFormat));

    Glib::OptionEntry entry4;
    entry4.set_long_name("watch");
    entry4.set_short_name('w');
    entry4.set_description("Watch file for rate changes (e.g. /proc/asound/sndrpihifiberry/rx_rate)");
    entry4.set_flags(Glib::OptionEntry::Flags::FLAG_OPTIONAL_ARG);
    add_entry_filename(entry4, sigc::mem_fun(*this, &CornrowdOptionGroup::onWatchFilename));
}

bool CornrowdOptionGroup::onRate(const Glib::ustring& /*option_name*/, const std::string& value, bool has_value)
{
    if (!has_value || value.empty()) {
        return false;
    }

    int rate = std::stoi(value);
    if (validSampleRates.count(rate)) {
        m_rate = rate;
        return true;
    }

    return false;
}

bool CornrowdOptionGroup::onFormat(const Glib::ustring& /*option_name*/, const std::string& value, bool has_value)
{
    if (!has_value || value.empty()) {
        return false;
    }

    std::string formatString(value);
    std::transform(value.begin(), value.end(), formatString.begin(), ::tolower);

    if (formatString == "i16") {
        m_format = SampleFormat::I16;
        return true;
    } else if (formatString == "f32") {
        //m_format = SampleFormat::F32;
        return false;
    }

    return false;
}

bool CornrowdOptionGroup::onWatchFilename(const Glib::ustring& /*option_name*/, const std::string& value, bool has_value)
{
    if (!has_value || value.empty()) {
        return false;
    }

    if (Glib::file_test(value, Glib::FILE_TEST_EXISTS)) {
        m_watchFilename = value;
        return true;
    }

    return false;
}
