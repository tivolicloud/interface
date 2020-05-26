docker run -it --rm \
-v $PWD/patches:/patches:ro \
-v $PWD/arch-linux/qt5:/qt5 \
-v $PWD/arch-linux/qt5-build:/qt5-build \
-v $PWD/arch-linux/qt5-install:/qt5-install \
archlinux:latest