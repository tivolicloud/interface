FROM debian:bullseye-slim

COPY dist/. /root

RUN \
apt-get update -y && \
DEBIAN_FRONTEND=noninteractive apt-get install -y \
# wont run without these...
libopengl0 libglib2.0-0 libgl1-mesa-glx libharfbuzz0b \
# curl (ca-certificates?) is necessary for tls
curl && \
# cleanup
apt-get autoremove -y && \
apt-get clean -y && \
rm -rf /var/lib/apt/* /var/lib/cache/* /var/lib/dpkg/* /var/cache/*

CMD /root/ice-server.AppDir/AppRun
