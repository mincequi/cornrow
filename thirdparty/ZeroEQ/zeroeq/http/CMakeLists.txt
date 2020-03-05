# Copyright (c) HBP 2014-2017 Daniel.Nachbaur@epfl.ch

set(ZEROEQHTTP_PUBLIC_HEADERS
  helpers.h
  request.h
  response.h
  server.h
  types.h
)
set(ZEROEQHTTP_HEADERS
  requestHandler.h
  jsoncpp/json/json.h
  jsoncpp/json/json-forwards.h
)
set(ZEROEQHTTP_SOURCES
  requestHandler.cpp
  server.cpp
  jsoncpp/jsoncpp.cpp
)
set(ZEROEQHTTP_LINK_LIBRARIES PUBLIC ZeroEQ PRIVATE ${CPPNETLIB_LIBRARIES}
  ${ZeroMQ_LIBRARY})

if(MSVC)
  list(APPEND ZEROEQHTTP_LINK_LIBRARIES Ws2_32)
else()
  set_source_files_properties(jsoncpp/jsoncpp.cpp PROPERTIES COMPILE_FLAGS
    -Wno-shadow)
endif()

set(ZEROEQHTTP_INCLUDE_NAME zeroeq/http)
set(ZEROEQHTTP_NAMESPACE zeroeqhttp)
set(ZEROEQHTTP_OMIT_LIBRARY_HEADER ON)
common_library(ZeroEQHTTP)
