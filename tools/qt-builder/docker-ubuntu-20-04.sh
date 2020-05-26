docker run -it --rm \
-v $PWD/patches:/patches:ro \
-v $PWD/qt5:/qt5 \
-v $PWD/ubuntu-20-04/qt5-build:/qt5-build \
-v $PWD/ubuntu-20-04/qt5-install:/qt5-install \
ubuntu:20.04