#pragma once

namespace audio
{
class Controller;
}

void readConfig(audio::Controller& audioController);
void writeConfig(const audio::Controller& audioController);
