# Building Qt 5.15.0 for Windows

## Install depenencies from [old readme](README.old.md). Make sure to install Visual Studio 2019 instead! (I recommend using [Chocolatey](https://chocolatey.org))

## Inside a short length folder such as C:\q

### Clone Qt:

```bash
git clone --recursive git://code.qt.io/qt/qt5.git -b 5.15.0 --single-branch
```

### Apply patches:

No patches necessary!

```bash
xcopy /E /I C:\path\to\interface\tools\qt-builder\patches qt5\patches
cd qt5

git apply --ignore-space-change --ignore-whitespace patches/chromium-missing-import.patch
git apply --ignore-space-change --ignore-whitespace patches/qimage-scaled-multithread-fix.patch
git apply --ignore-space-change --ignore-whitespace patches/chromium-override-audio-output-permission.patch

cd ..
```

### Configure and build:

```bash
mkdir qt5-install qt5-build
cd qt5-build

C:\path\to\interface\tools\qt-builder\qt5vars.bat

..\qt5\configure -force-debug-info -release -opensource -confirm-license -recheck-all -opengl desktop -platform win32-msvc -openssl-linked OPENSSL_PREFIX="%TIVOLI_VCPKG_BASE_VERSION%\installed\x64-windows-release" -nomake tools -nomake tests -nomake examples -skip qttranslations -skip qtserialport -skip qt3d -skip qtlocation -skip qtwayland -skip qtsensors -skip qtgamepad -skip qtspeech -skip qtcharts -skip qtx11extras -skip qtmacextras -skip qtvirtualkeyboard -skip qtpurchasing -skip qtdatavis3d -skip qtpim -skip qtdocgallery -webengine-proprietary-codecs -no-warnings-are-errors -no-pch -prefix ..\qt5-install

jom
jom install

cd ..
```

### Fix installation (in WSL):

```bash
cd qt5-install

# http://www.linuxfromscratch.org/blfs/view/stable-systemd/x/qtwebengine.html
find . -name \*.prl -exec sed -i -e '/^QMAKE_PRL_BUILD_DIR/d' {} \;

cp /mnt/c/path/to/interface/tools/qt-builder/qt.conf bin

cd ..
```

### Archiving (in WSL):

```bash
tar -zcvf tivoli-qt5-install-5.15.0-windows.tar.gz qt5-install
```

Then upload the tar and update [hifi_qt.py](../../hifi_qt.py) to the new link.

You can test your Qt build by setting environment variable:

```env
QT_CMAKE_PREFIX_PATH=C:\path\to\interface\tools\qt-builder\qt5-install\lib\cmake
```
