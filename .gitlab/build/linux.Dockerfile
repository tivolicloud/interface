FROM ubuntu:18.04

# arch linux
# - pacman -Syu --noconfirm base-devel git curl unzip gcc cmake python python-distutils-extra nodejs openssl double-conversion libpng harfbuzz mesa alsa-lib libxmu libxi freeglut jack libxrandr libudev0-shim zlib md4c

# ubuntu 18.04

ENV DEBIAN_FRONTEND=noninteractive

RUN \
apt-get update -y && \
apt-get install -y build-essential gcc g++ ninja-build git curl zip unzip python3 patchelf wget libssl-dev libdouble-conversion-dev libpng-dev libharfbuzz-dev libgl1-mesa-dev libasound2 libxmu-dev libxi-dev freeglut3-dev libasound2-dev libjack0 libjack-dev libxrandr-dev libudev-dev zlib1g-dev libpcre2-dev libpulse-mainloop-glib0 liblcms2-2 libjpeg-dev libfontconfig1 libxcb-icccm4-dev libxcb-image0-dev libpulse0 libnss3 libnspr4 libxcursor1 libxcomposite1 libxtst6 libxslt1.1 libxss-dev libpci-dev libevent-dev libminizip-dev libxcb-keysyms1-dev libxcb-render-util0-dev libxcb-xinerama0-dev libxcb-xkb-dev libxkbcommon-x11-dev libegl1-mesa-dev libgstreamer-plugins-base1.0-dev libgtk-3-dev libmtdev-dev libinput-dev libxcb-xinput-dev && \
\
# cmake 3.14 required for vcpkg
curl -O https://cmake.org/files/v3.14/cmake-3.14.2-Linux-x86_64.sh && \
chmod +x cmake-3.14.2-Linux-x86_64.sh && \
./cmake-3.14.2-Linux-x86_64.sh --prefix=/usr/local --skip-license && \
rm -f cmake-3.14.2-Linux-x86_64.sh && \
\
# # ninja 1.10.0+ is required
# git clone git://github.com/ninja-build/ninja.git && cd ninja && \
# git checkout release && \
# python3 configure.py --bootstrap && \
# mv ninja /usr/bin/ninja && \
# cd .. && \
\
# install nodejs 12
curl -sL https://deb.nodesource.com/setup_12.x | bash - && \
apt-get install -y nodejs && \
npm i -g @sentry/cli