# https://wiki.debian.org/DebianReleases

docker run -it --rm \
-v $PWD/patches:/patches:ro \
-v $PWD/qt.conf:/qt.conf:ro \
-v $PWD/qt5:/qt5 \
-v $PWD/debian-9/qt5-build:/qt5-build \
-v $PWD/debian-9/qt5-install:/qt5-install \
debian:9