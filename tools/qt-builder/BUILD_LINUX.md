# Building Qt 5.15.1 for Linux

https://wiki.qt.io/Building_Qt_5_from_Git

## Install depenencies

Recommended to be done in a Docker container

If you want to run a Docker ARM image on x86_64 hardware, run this once on the host machine:

```bash
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
```

### Ubuntu 16.04+

```bash
grep '^deb ' /etc/apt/sources.list | perl -pe 's/deb /deb-src /' >> /etc/apt/sources.list
apt-get update -y
apt-get build-dep -y qt5-default
DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential curl perl python git '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev flex bison gperf libicu-dev libxslt-dev ruby libssl-dev libxcursor-dev libxcomposite-dev libxdamage-dev libxrandr-dev libdbus-1-dev libfontconfig1-dev libcap-dev libxtst-dev libpulse-dev libudev-dev libpci-dev libnss3-dev libasound2-dev libxss-dev libegl1-mesa-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libproxy-dev xinput libopenal-dev ninja-build libre2-dev libminizip-dev libevent-dev libprotobuf-dev liblcms2-dev libharfbuzz-dev
# if nodejs not installed
curl -sL https://deb.nodesource.com/setup_14.x | bash -
apt-get install -y nodejs
```

If using Ubuntu 16.04, you'll have to update OpenSSL to 1.1:

```bash
# download and compile
cd /usr/local/src
curl -O https://www.openssl.org/source/openssl-1.1.1g.tar.gz
tar -xvf openssl-1.1.1g.tar.gz
cd openssl-1.1.1g
./config --prefix=/usr/local/ssl --openssldir=/usr/local/ssl
make -j$(nproc)
make test -j$(nproc)
make install -j$(nproc)
# config openssl shared objects
echo "/usr/local/ssl/lib" > /etc/ld.so.conf.d/openssl-1.1.1g.conf
ldconfig -v
# config openssl binary
mv /usr/bin/c_rehash /usr/bin/c_rehash.backup
mv /usr/bin/openssl /usr/bin/openssl.backup
# add /usr/local/ssl/bin to /etc/environment PATH at the end
source /etc/environment
# check if updated!
which openssl
openssl version -a
# make sure to remove old openssl
apt-get remove -y libssl-dev
```

Guide from https://cloudwafer.com/blog/installing-openssl-on-ubuntu-16-04-18-04

### Arch Linux

There might be too many packages

```bash
pacman -Syu --noconfirm git libxcb xcb-proto xcb-util xcb-util-cursor xcb-util-image xcb-util-keysyms xcb-util-renderutil xcb-util-wm libxi base-devel gperf python python2 mesa xf86-video-vesa xf86-video-intel vulkan-intel xf86-input-libinput libxcursor libxcomposite libxdamage libxrandr xorg-xrandr libxtst alsa-lib eglexternalplatform libglvnd gtk3 libinput tslib libxkbcommon libxkbcommon-x11 pulseaudio pulseaudio-alsa gstreamer nss re2 icu libwebp opus ffmpeg libvpx snappy minizip libevent jsoncpp protobuf libxml2 libxslt ninja gn ccache double-conversion libproxy openssl md4c
```

### More at [old readme](README.old.md)

## Inside this folder (interface/tools/qt-builder)

### Clone Qt:

```bash
git clone --recursive git://code.qt.io/qt/qt5.git -b 5.15.1 --single-branch
```

### Apply patches:

```bash
cd qt5

git apply --ignore-space-change --ignore-whitespace ../patches/chromium-override-audio-output-permission.patch

cd ..
```

### Configure and build:

If using Ubuntu 16.04, you'll want to add this in the configure command after `-openssl`:

```bash
OPENSSL_PREFIX="/usr/local/ssl"
```

```bash
mkdir qt5-install qt5-build
cd qt5-build

../qt5/configure -force-debug-info -release -opensource -confirm-license -recheck-all -openssl -nomake tools -nomake tests -nomake examples -skip qttranslations -skip qtserialport -skip qt3d -skip qtquick3d -skip qtlocation -skip qtsensors -skip qtgamepad -skip qtspeech -skip qtcharts -skip qtx11extras -skip qtmacextras -skip qtvirtualkeyboard -skip qtpurchasing -skip qtdatavis3d -skip qtpim -skip qtdocgallery -webengine-proprietary-codecs -no-warnings-are-errors -no-pch -c++std c++14 -prefix ../qt5-install

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
tar -zcvf tivoli-qt5-install-5.15.1-ubuntu-16.04.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.15.1-ubuntu-18.04.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.15.1-ubuntu-18.04-arm64v8.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.15.1-ubuntu-20.04.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.15.1-arch-linux.tar.gz qt5-install
```

Then upload the tar and update [hifi_qt.py](../../hifi_qt.py) to the new link.

You can test your Qt build by setting environment variable:

```env
QT_CMAKE_PREFIX_PATH=/path/to/interface/tools/qt-builder/qt5-install/lib/cmake
```
