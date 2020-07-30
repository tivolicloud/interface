FROM debian:10-slim

RUN \
apt-get update -y && \
apt-get install -y curl glib2.0 libgl1-mesa-glx libharfbuzz-bin && \
# cleanup
apt-get clean -y && \
rm -rf /var/lib/apt/lists/* /var/cache/apt/*

COPY dist/. /root

CMD /root/ice-server.AppDir/AppRun
