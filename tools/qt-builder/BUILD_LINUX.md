# Building Qt 5.14.1 for Linux

https://wiki.qt.io/Building_Qt_5_from_Git

## Install depenencies

### Debian 9 (Stretch)

```bash
grep '^deb ' /etc/apt/sources.list | perl -pe 's/deb /deb-src /' >> /etc/apt/sources.list
apt-get update
apt-get build-dep qt5-default
apt-get install libssl-dev libxcursor-dev libxcomposite-dev libxdamage-dev libxrandr-dev libdbus-1-dev libfontconfig1-dev libcap-dev libxtst-dev libpulse-dev libudev-dev libpci-dev libnss3-dev libasound2-dev libxss-dev libegl1-mesa-dev gperf bison flex python git
```

### More at [old readme](README.old.md)

## Inside this folder (interface/tools/qt-builder)

### Clone Qt:

```bash
git clone --recursive git://code.qt.io/qt/qt5.git -b 5.14.1 --single-branch
```

### Apply patches:

```bash
cp -R patches qt5
cd qt5

git apply --ignore-space-change --ignore-whitespace patches/qtscript-crash-fix.patch

cd ..
```

### Configure and build:

```bash
mkdir qt5-install qt5-build
cd qt5-build

../qt5/configure -force-debug-info -release -opensource -confirm-license -recheck-all -nomake tools -nomake tests -nomake examples -skip qttranslations -skip qtserialport -skip qt3d -skip qtlocation -skip qtwayland -skip qtsensors -skip qtgamepad -skip qtspeech -skip qtcharts -skip qtx11extras -skip qtmacextras -skip qtvirtualkeyboard -skip qtpurchasing -skip qtdatavis3d -skip qtpim -skip qtdocgallery -webengine-proprietary-codecs -no-warnings-are-errors -no-pch -c++std c++14 -prefix ../qt5-install

make -j$(nproc)
make install

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
tar -zcvf tivoli-qt5-install-5.14.1-debian-9.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.14.1-arch-linux.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.14.1-ubuntu-18.04.tar.gz qt5-install
```

Then upload the tar and update [hifi_qt.py](../../hifi_qt.py) to the new link.

You can test your Qt build by setting environment variable:

```env
QT_CMAKE_PREFIX_PATH=path/to/interface/tools/qt-builder/qt5-install/lib/cmake
```
