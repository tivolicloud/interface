# Building Qt 5.15 for Linux

https://wiki.qt.io/Building_Qt_5_from_Git

## Install depenencies

Recommended to be done in a Docker container

### Debian 10 (Buster) and Ubuntu 20.04

```bash
grep '^deb ' /etc/apt/sources.list | perl -pe 's/deb /deb-src /' >> /etc/apt/sources.list
apt-get update -y
apt-get build-dep -y qt5-default
apt-get install -y build-essential perl python git '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev flex bison gperf libicu-dev libxslt-dev ruby libssl-dev libxcursor-dev libxcomposite-dev libxdamage-dev libxrandr-dev libdbus-1-dev libfontconfig1-dev libcap-dev libxtst-dev libpulse-dev libudev-dev libpci-dev libnss3-dev libasound2-dev libxss-dev libegl1-mesa-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libproxy-dev xinput libopenal-dev ninja-build libre2-dev libminizip-dev libevent-dev libprotobuf-dev liblcms2-dev libharfbuzz-dev
```

### Arch Linux

There might be too many packages

```bash
pacman -Syu --noconfirm git libxcb xcb-proto xcb-util xcb-util-cursor xcb-util-image xcb-util-keysyms xcb-util-renderutil xcb-util-wm libxi base-devel gperf python python2 mesa xf86-video-vesa xf86-video-intel vulkan-intel xf86-input-libinput libxcursor libxcomposite libxdamage libxrandr xorg-xrandr libxtst alsa-lib eglexternalplatform libglvnd gtk3 libinput tslib libxkbcommon libxkbcommon-x11 pulseaudio pulseaudio-alsa gstreamer nss re2 icu libwebp opus ffmpeg libvpx snappy minizip libevent jsoncpp protobuf libxml2 libxslt ninja gn ccache double-conversion libproxy openssl md4c
```

### More at [old readme](README.old.md)

## Inside this folder (interface/tools/qt-builder)

### Clone Qt:

```bash
git clone --recursive git://code.qt.io/qt/qt5.git -b 5.15 --single-branch
```

### Apply patches:

No patches necessary!

<!-- ```bash
cp -R patches qt5
cd qt5

git apply --ignore-space-change --ignore-whitespace patches/qtscript-crash-fix.patch

cd ..
``` -->

### Configure and build:

```bash
mkdir qt5-install qt5-build
cd qt5-build

../qt5/configure -force-debug-info -release -opensource -confirm-license -recheck-all -nomake tools -nomake tests -nomake examples -skip qttranslations -skip qtserialport -skip qt3d -skip qtlocation -skip qtwayland -skip qtsensors -skip qtgamepad -skip qtspeech -skip qtcharts -skip qtx11extras -skip qtmacextras -skip qtvirtualkeyboard -skip qtpurchasing -skip qtdatavis3d -skip qtpim -skip qtdocgallery -webengine-proprietary-codecs -no-warnings-are-errors -no-pch -c++std c++14 -prefix ../qt5-install

make -j$(nproc)
make install -j$(nproc)
cd ..
```

### Fix installation:

```bash
cd qt5-install

# http://www.linuxfromscratch.org/blfs/view/stable-systemd/x/qtwebengine.html
find . -name \*.prl -exec sed -i -e '/^QMAKE_PRL_BUILD_DIR/d' {} \;

cp ../qt.conf bin

cd ..
```

### Archiving:

```bash
tar -zcvf tivoli-qt5-install-5.15-debian-10.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.15-ubuntu-20.04.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.15-arch-linux.tar.gz qt5-install
```

Then upload the tar and update [hifi_qt.py](../../hifi_qt.py) to the new link.

You can test your Qt build by setting environment variable:

```env
QT_CMAKE_PREFIX_PATH=/path/to/interface/tools/qt-builder/qt5-install/lib/cmake
```
