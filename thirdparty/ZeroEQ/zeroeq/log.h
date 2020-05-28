
/* Copyright (c) 2014-2015, Human Brain Project
 *                          Juan Hernando <jhernando@fi.upm.es>
 */

#ifndef ZEROEQ_LOG_H
#define ZEROEQ_LOG_H

#include <iostream>
#define ZEROEQERROR std::cerr
#define ZEROEQWARN std::cerr
#ifdef NDEBUG
#define ZEROEQINFO \
    if (false)     \
    std::cout
#define ZEROEQLOG \
    if (false)    \
    std::cout
#else
#define ZEROEQINFO std::cerr
#define ZEROEQLOG std::cerr
#endif
#define ZEROEQDONTCALL                                                     \
    {                                                                      \
        ZEROEQERROR << "Code is not supposed to be called in this context" \
                    << std::endl;                                          \
    }

#define ZEROEQTHROW(exc)                       \
    {                                          \
        ZEROEQINFO << exc.what() << std::endl; \
        throw exc;                             \
    }

#endif
