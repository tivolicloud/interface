FROM ubuntu:18.04

RUN \
apt-get update -y && \
DEBIAN_FRONTEND=noninteractive apt-get install -y curl glib2.0 libgl1-mesa-glx libharfbuzz-bin libopengl0 && \
# cleanup
apt-get clean -y && \
rm -rf /var/lib/apt/lists/* /var/cache/apt/*

COPY dist/. /root

CMD /root/ice-server.AppDir/AppRun
