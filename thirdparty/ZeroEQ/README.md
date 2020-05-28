
# Introduction

ZeroEQ is a cross-platform C++ library for modern messaging. It provides pub-sub
messaging using ZeroMQ and integrates REST APIs with JSON payload in C++
applications using an optional http::Server. ZeroEQ applications are linked
using automatic discovery based on ZeroConf protocol or through explicit
connection addressing using hostname and port. Applications can define a
vocabulary for the published messages, provided by
[ZeroBuf](https://github.com/HBPVIS/ZeroBuf) or by implementing a simple
Serializable interface.

# Features

ZeroEQ provides the following major features:

* Publish events using zeroeq::Publisher
* Subscribe to events using zeroeq::Subscriber
* Web service APIs using zeroeq::http::Server using REST and JSON
* Client-Server request-reply using zeroeq::Server and zeroeq::Client
* Asynchronous, reliable transport using [ZeroMQ](http://www.zeromq.org)
* Automatic publisher discovery using [Zeroconf](https://en.wikipedia.org/wiki/Zero-configuration_networking)
* Serialization of events using [ZeroBuf](https://github.com/HBPVIS/ZeroBuf)
* [List of RFCs](@ref rfcs)

# Example Use Cases

* [Interactive Supercomputing](https://www.youtube.com/watch?v=wATHwvRFGz0&t=1m36s):
  loose, robust and fast coupling of unrelated applications
* [Integration of C++ applications into Jupyter Notebooks](https://www.youtube.com/watch?v=pczckc9HSsA&t=14m30s):
  automatic python code generation to remote control C++ applications using the
  zeroeq::http::Server

# Building from Source

ZeroEQ is a cross-platform library, designed to run on any modern operating
system, including all Unix variants. It requires a C++11 compiler and uses CMake
to create a platform-specific build environment. The following platforms and
build environments are tested:

* Linux: Ubuntu 16.04, RHEL 6.8 (Makefile, Ninja)
* Mac OS X: 10.9 (Makefile, Ninja)

ZeroEQ requires the following external, pre-installed dependencies:

* ZeroMQ 4.0 or later
* Boost for unit tests; version 1.58 for optional cppnetlib

Building from source is as simple as:

    git clone --recursive https://github.com/HBPVIS/ZeroEQ.git
    mkdir ZeroEQ/build
    cd ZeroEQ/build
    cmake -GNinja -DCLONE_SUBPROJECTS=ON ..
    ninja
This work has been partially funded by the European Union Seventh Framework Program (FP7/2007­2013) under grant agreement no. 604102 (HBP).
