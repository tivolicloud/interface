FROM debian:10

RUN \
apt-get update -y && \
apt-get install -y curl glib2.0 libgl1-mesa-glx libharfbuzz-bin

COPY dist/. /root

CMD /root/ice-server.AppDir/AppRun
