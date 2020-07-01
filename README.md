# Cornrow

Cornrow is an audio sink daemon designed for low-powered boards like the Raspberry Pi. It accepts bluetooth audio sources and transforms your computer into a bluetooth speaker.

It is made for Debian based environments and compatible with Ubuntu 18.04 Bionic and Debian Stretch (and later). This means that this service runs as a dedicated user and can be cleanly installed and removed using Debian package management.

I am willing to provide an easy out-of-the-box solution for anyone. So, your experience with this little daemon is very appreciated. Please leave your comments and issues using it. Thanks a lot.

The daemon has a built-in equalizer which can be remote controlled via this app:
[corocomo](https://play.google.com/store/apps/details?id=org.cornrow.corocomo)
![alt text](https://github.com/mincequi/cornrow/blob/master/data/screenshot_1.png)



## Installation (binary)
Compiled debian/ubuntu packages are available for [armhf](https://github.com/mincequi/cornrow/releases/download/v0.7.0/cornrowd_0.7.0_armhf.deb) and [amd64](https://github.com/mincequi/cornrow/releases/download/v0.6.0/cornrowd_0.6.0_amd64.deb).
Also a special version for Raspberry Pi exists [rpi-armhf](https://github.com/mincequi/cornrow/releases/download/v0.7.0/cornrowd-rpi_0.7.0_armhf.deb), which supports multichannel audio output via HDMI.

Get dependencies first
```
sudo apt update
sudo apt install \
  libqt5bluetooth5 \
  libqt5network5 \
  libasound2 \
  libavahi-compat-libdnssd1 \
  libavcodec58 \
  libavutil56 \
  libboost-system1.67.0 \
  libsbc1
dpkg -i cornrowd__<version>_<your_arch>.deb
sudo systemctl unmask cornrowd.service              # unmask service
sudo systemctl start cornrowd.service               # start-up service. You should now be able to connect any bluetooth audio device.
sudo systemctl enable cornrowd.service              # start-up service on each reboot.
```

## Installation (from source)
Consider downloading the release tarballs. Master might be broken from time to time.

### For Debian users
```
sudo apt install \
  cmake extra-cmake-modules \
  libasio-dev \
  libasound2-dev \
  libavahi-compat-libdnssd-dev \
  libavcodec-dev \
  libavutil-dev \
  libboost-system-dev \
  libsbc-dev \
  qtconnectivity5-dev \
  libqt5websockets5-dev # get dependecies
wget https://github.com/mincequi/cornrow/releases/download/v0.7.0/cornrowd_0.7.0.tar.gz
tar xf cornrowd_0.7.0.tar.gz
dpkg-buildpackage -us -uc -nc                       # build unsigned debian package
sudo dpkg -i ../cornrowd_<version>_<your_arch>.deb  # install package
sudo systemctl unmask cornrowd.service              # unmask service
sudo systemctl start cornrowd.service               # start-up service. You should now be able to connect any bluetooth audio device.
sudo systemctl enable cornrowd.service              # start-up service on each reboot.
```

### Arbitrary distro
Get the dependencies. Basically, these are ffmpeg (libav*), qt5bluetooth, qt5websockets
```
git clone --recursive https://github.com/mincequi/cornrow
cd cornrow
mkdir build
cd build
cmake ..
make
```

## Known issues
### Raspberry Pi 3 Model B (BCM43143)
#### Firmware issues
Apparently, there are issues with on-board Bluetooth. Depending on the installed firmware i get stuttering Bluetooth audio playback. Additionally, there seem to be issues when operating in classic Bluetooth and Low Energy simultaneously.
Here is an incomplete list of working/non-working firmware versions (https://github.com/Hexxeh/rpi-firmware/commits/master). Use the rpi-update to flash your Raspberry Pi to the according version:
```shell
sudo rpi-update <commit-id>
```
Commit | Date | Working
--- | --- | ---
6aec73e | 2019-01-09 | OK
883750d | 2019-02-05 | OK
29952e8 | 2019-03-08 | NOK

#### UART issues
A lot of Bluetooth packets seem to be dropped when CPU usage is **low**. As soon as the system is put under load, bluetooth packet reception is a **lot** better. This seems to be related to core frequency scaling (https://www.raspberrypi.org/documentation/configuration/uart.md).

#### GCC-9
Currently, this does not compile with GCC9 and higher.
