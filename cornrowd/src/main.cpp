#include <iostream>
#include <glibmm/optioncontext.h>

#include "Controller.h"
#include "CornrowdOptionGroup.h"

using namespace std;

int main(int argc, char *argv[])
{
    Glib::OptionContext context;
    CornrowdOptionGroup group;
    context.set_main_group(group);

    try {
        context.parse(argc, argv);
    } catch (const Glib::Error& ex) {
        std::cerr << ex.what() << std::endl;
        return -1;
    }

    Controller controller({Source::Default, group.m_rate, group.m_format, group.m_watchFilename});

    return 0;
}
