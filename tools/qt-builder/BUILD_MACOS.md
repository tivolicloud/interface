# Building Qt 5.15.2 for macOS

## Install depenencies

### Make sure Xcode and Brew are installed

```bash
brew install git fontconfig dbus-glib pkg-config llvm openssl
```

<!-- ```bash
sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer
``` -->

## Inside this folder (interface/tools/qt-builder)

### Clone Qt:

```bash
git clone --recursive git://code.qt.io/qt/qt5.git -b 5.15.2 --single-branch
```

### Apply patches:

```bash
cd qt5

git apply --ignore-space-change --ignore-whitespace ../patches/chromium-override-audio-output-permission.patch

cd ..
```

### Configure and build:

```bash
mkdir qt5-install qt5-build
cd qt5-build

../qt5/configure -force-debug-info -release -opensource -confirm-license -recheck-all -openssl OPENSSL_PREFIX="/opt/homebrew/opt/openssl" -qt-zlib -qt-libjpeg -qt-libpng -qt-freetype -qt-pcre -qt-harfbuzz -nomake tools -nomake tests -nomake examples -skip qttranslations -skip qtserialport -skip qt3d -skip qtquick3d -skip qtlocation -skip qtsensors -skip qtgamepad -skip qtspeech -skip qtcharts -skip qtx11extras -skip qtmacextras -skip qtvirtualkeyboard -skip qtpurchasing -skip qtdatavis3d -skip qtpim -skip qtdocgallery -webengine-proprietary-codecs -no-warnings-are-errors -no-pch -c++std c++14 -prefix ../qt5-install

make -j$(sysctl -n hw.logicalcpu)
make install -j$(sysctl -n hw.logicalcpu)

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
tar -zcvf tivoli-qt5-install-5.15.2-macos.tar.gz qt5-install
```

Then upload the tar and update [hifi_qt.py](../../hifi_qt.py) to the new link.

You can test your Qt build by setting environment variable:

```env
QT_CMAKE_PREFIX_PATH=/path/to/interface/tools/qt-builder/qt5-install/lib/cmake
```
