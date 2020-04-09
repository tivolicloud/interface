# Building Qt 5.14.1 for Linux

## Install depenencies from [old readme](README.old.md)

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

../qt5/configure -force-debug-info -release -opensource -confirm-license -recheck-all -nomake tests -nomake examples -skip qttranslations -skip qtserialport -skip qt3d -skip qtlocation -skip qtwayland -skip qtsensors -skip qtgamepad -skip qtspeech -skip qtcharts -skip qtx11extras -skip qtmacextras -skip qtvirtualkeyboard -skip qtpurchasing -skip qtdatavis3d -skip qtpim -skip qtdocgallery -webengine-proprietary-codecs -no-warnings-are-errors -no-pch -c++std c++14 -prefix ../qt5-install

make
make install


cd ..
```

### Fix installation:

```bash
cd qt5-install

# if qmake isn't present in qt5-install/bin
cp ../qt5-build/qtbase/bin/qmake bin

# http://www.linuxfromscratch.org/blfs/view/stable-systemd/x/qtwebengine.html
find . -name \*.prl -exec sed -i -e '/^QMAKE_PRL_BUILD_DIR/d' {} \;

cp ../qt.conf bin

cd ..
```

### Archiving:

```bash
tar -zcvf tivoli-qt5-install-5.14.1-arch-linux.tar.gz qt5-install
tar -zcvf tivoli-qt5-install-5.14.1-ubuntu-18.04.tar.gz qt5-install
```

Then upload the tar and update [hifi_qt.py](../../hifi_qt.py) to the new link.

You can test your Qt build by setting environment variable:

```env
QT_CMAKE_PREFIX_PATH=path/to/interface/tools/qt-builder/qt5-install/lib/cmake
```