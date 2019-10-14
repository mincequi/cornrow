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
  cmake extra-cmake-modules \
  libboost-dev \
  libgstreamermm-1.0-dev \
  gstreamer1.0-alsa gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
  qtconnectivity5-dev # get dependecies
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
git clone --branch v0.1.0 --recursive https://github.com/mincequi/cornrow
cd cornrow
mkdir build
cd build
cmake ..
make
```

## Known issues
### Raspberry Pi 3 Model B (BCM43143)
Apparently, there are issues with on-board Bluetooth. Depending on the installed firmware i get stuttering Bluetooth audio playback. Additionally, there seem to be issues when operating in classic Bluetooth and Low Energy simultaneously.
Here is an incomplete list of working/non-working firmware versions. Use the rpi-update to flash your Raspberry Pi to the according version:
```shell
sudo rpi-update <commit-id>
```
Commit | Date | Working
--- | --- | ---
6aec73e | 2019-01-09 | OK
883750d | 2019-02-05 | OK
29952e8 | 2019-03-08 | NOK

## To Do
* ~~Implement volume control~~
* ~~Set discoverable/pairable timeout to forever~~
