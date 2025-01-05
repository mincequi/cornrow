# Cornrow

Cornrow is an audio sink daemon for Bluetooth and Airplay streams. It is designed for low-powered boards like the Raspberry Pi, but can also be used for SBCs/VMs with a Bluetooth dongle instead of a built-in module (first Bluetooth adapter found will be used).

It is made for Debian based environments and compatible with Ubuntu and Debian. This means that this service runs as a dedicated user and can be cleanly installed and removed using Debian package management.
**Note:**
For Debian Bullseye and Ubuntu 20.04 (Focal) and later, please use cornrow **v0.8.1**.
For Debian Bookworm and later, please use cornrow **v0.9.0**.
For Debian Trixie and Ubuntu 24.10 (Oracular) and later, please use cornrow **v1.0.0**.

## Installation (binary)
Compiled debian/ubuntu packages are available for [armhf](https://github.com/mincequi/cornrow/releases/download/v1.0.0/cornrow_1.0.0_armhf.deb).

```
wget https://github.com/mincequi/cornrow/releases/download/v1.0.0/cornrow_1.0.0_armhf.deb
sudo apt update
sudo apt install ./cornrow_1.0.0_armhf.deb
sudo systemctl unmask cornrowd.service              # unmask service
sudo systemctl start cornrowd.service               # start-up service. You should now be able to connect any bluetooth audio device.
sudo systemctl enable cornrowd.service              # start-up service on each reboot.
```

## Installation (from source)
Consider downloading the release tarballs. Master might be broken from time to time.

### For Debian users
```
sudo apt install \
  bluez \
  build-essential \
  debhelper \
  fakeroot \
  libtool \
  cmake \
  libasound2-dev \
  libavahi-compat-libdnssd-dev \
  libavcodec-dev \
  libboost-dev \
  libboost-system-dev \
  libsbc-dev \
  libssl-dev \
  qtconnectivity5-dev \
  libqt5websockets5-dev
wget https://github.com/mincequi/cornrow/archive/refs/tags/v1.0.0.tar.gz
tar xf cornrowd-1.0.0.tar.xz
cd cornrow
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cpack                                               # build debian package
cd ..
sudo apt install ./cornrow_1.0.0_<your_arch>.deb   # install package
sudo systemctl unmask cornrowd.service              # unmask service
sudo systemctl start cornrowd.service               # start-up service. You should now be able to connect any bluetooth audio device.
sudo systemctl enable cornrowd.service              # start-up service on each reboot.
```

### Arch Linux
Cornrow is available in the [AUR](https://aur.archlinux.org/packages/cornrow). Install it with your
favorite AUR helper or manually:

```bash
git clone https://aur.archlinux.org/cornrow.git
cd cornrow
makepkg -si
```

### Arbitrary distro
Get the dependencies. Basically, these are ffmpeg (libav*), qt5bluetooth, qt5websockets
```
git clone https://github.com/mincequi/cornrow
cd cornrow
mkdir build
cd build
cmake ..
make
```

## Configuration
Cornrow can be configured via a configuration file `/etc/cornrowd.conf`. There are some options for inputs and outputs.

### Snapcast integration
There is a 'tcp_sink' option in cornrow to send audio to a snapcast server (which acts as a TCP server).
Add the following lines to your `/etc/cornrowd.conf`:
```sh
[tcp_sink]
host = "127.0.0.1" # the host you want to stream to
port = 4953 # the port your host is listening on
```
And this to your snapcast configuration:
```sh
tcp://<listen IP, e.g. 127.0.0.1>:<port>?name=<name>[&mode=server]&sampleformat=44100:16:2
```

## Donations
Cornrow is free to use and developed during personal free time. If you like it and/or have a feature request, consider a donation. If you click on the button below, you will be taken to the secure PayPal Web site. You don't need to have a paypal account in order to make a donation.

[![paypal](https://www.paypalobjects.com/en_US/DK/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=22P2UZ4H6Z8FS)

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
