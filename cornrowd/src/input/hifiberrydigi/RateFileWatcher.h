#ifndef DEVICEWATCHERHIFIBERRYDIGIPLUS_H
#define DEVICEWATCHERHIFIBERRYDIGIPLUS_H

#include <string>

#include <sigc++/sigc++.h>

class RateFileWatcher : public sigc::trackable
{
public:
    RateFileWatcher(const std::string& filename);

    void start();
    sigc::signal<void(int)> rateChanged;

private:
    bool onTimeout();
    std::string m_filename;
    int m_sampleRate;
};

#endif // DEVICEWATCHERHIFIBERRYDIGIPLUS_H
