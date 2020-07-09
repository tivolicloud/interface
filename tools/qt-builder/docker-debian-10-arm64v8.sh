# https://wiki.debian.org/DebianReleases

docker run -it --rm \
-v $PWD/patches:/patches:ro \
-v $PWD/qt.conf:/qt.conf:ro \
-v $PWD/qt5:/qt5 \
-v $PWD/debian-10-arm64v8/qt5-build:/qt5-build \
-v $PWD/debian-10-arm64v8/qt5-install:/qt5-install \
arm64v8/debian:10