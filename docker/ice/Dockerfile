FROM debian:10

RUN \
apt-get update -y && \
apt-get install -y curl glib2.0 libgl1-mesa-glx libharfbuzz-bin

COPY dist/. /root

ENV LD_LIBRARY_PATH=/root/lib
CMD /root/ice-server
