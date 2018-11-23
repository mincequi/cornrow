# Cornrow

Cornrow is a bluetooth audio daemon designed for embedded devices or low-powered boards like the Raspberry Pi.
It accepts bluetooth audio sources and transforms your computer into a bluetooth speaker.

# Building
## Requirements
* Qt 5.7
* Gstreamer 1.0
* Bluez

## Arbitrary distro
git clone --recursive https://github.com/mincequi/cornrow
cd cornrow
mkdir build
cd build
cmake ..
make

## Debian based distro (e.g. Ubuntu)
git clone --recursive https://github.com/mincequi/cornrow
cd cornrow
dpkg-buildpackage

# TODO

# Thanks to
