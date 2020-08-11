docker run -it --rm \
-v $PWD/patches:/patches:ro \
-v $PWD/qt.conf:/qt.conf:ro \
-v $PWD/qt5:/qt5 \
-v $PWD/ubuntu-18.04-arm64v8/qt5-build:/qt5-build \
-v $PWD/ubuntu-18.04-arm64v8/qt5-install:/qt5-install \
arm64v8/ubuntu:18.04