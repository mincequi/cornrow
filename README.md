# Cornrow

Cornrow is a bluetooth audio daemon designed for embedded devices or low-powered boards like the Raspberry Pi. It accepts bluetooth audio sources and transforms your computer into a bluetooth speaker.

Currently, it is compatible with Ubuntu 18.04 Bionic and Debian Stretch (and later) and offers full integration into Debian based environments. This means that this service runs as a dedicated user and can be cleanly installed and removed using Debian package management.

All this is in an early stage. However, i am willing to provide an easy out-of-the-box solution for anyone. So, your experience with this little daemon is very appreciated. Please leave your comments and issues using it. Thanks a lot.

The daemon has a built-in equalizer which can be remote controlled via this app:
[corocomo](https://play.google.com/store/apps/details?id=org.cornrow.corocomo)
![alt text](https://github.com/mincequi/cornrow/blob/master/data/screenshot_1.png)

## Installation
### For Debian users
```
sudo apt install \
  cmake \
  libgstreamermm-1.0-dev \
  libqt5bluetooth5 \
  gstreamer1.0-alsa gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad # get dependecies
git clone --recursive https://github.com/mincequi/cornrow # clone repo including third-party libs
cd cornrow
dpkg-buildpackage -us -uc                           # build unsigned debian package
sudo dpkg -i ../cornrowd_<version>_<your_arch>.deb  # install package
sudo systemctl start cornrowd.service               # start-up service. You should now be able to connect any bluetooth audio device.
sudo systemctl enable cornrowd.service              # start-up service on each reboot.
```

### Arbitrary distro
Get the dependencies. Basically, these are gstreamermm and qt5bluetooth.
```
git clone --recursive https://github.com/mincequi/cornrow
cd cornrow
mkdir build
cd build
cmake ..
make
```

## Details

## To do

## Thanks
