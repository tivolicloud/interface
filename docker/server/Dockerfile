# FROM archlinux:latest
FROM debian:bullseye-slim

ENV \
HIFI_DOMAIN_SERVER_HTTP_PORT=40100 \
HIFI_DOMAIN_SERVER_HTTPS_PORT=40101 \
HIFI_DOMAIN_SERVER_PORT=40102 \
HIFI_DOMAIN_SERVER_DTLS_PORT=40103 \
\
HIFI_ASSIGNMENT_CLIENT_AUDIO_MIXER_PORT=48000 \
HIFI_ASSIGNMENT_CLIENT_AVATAR_MIXER_PORT=48001 \
HIFI_ASSIGNMENT_CLIENT_ASSET_SERVER_PORT=48003 \
HIFI_ASSIGNMENT_CLIENT_MESSAGES_MIXER_PORT=48004 \
HIFI_ASSIGNMENT_CLIENT_ENTITY_SCRIPT_SERVER_PORT=48005 \
HIFI_ASSIGNMENT_CLIENT_ENTITIES_SERVER_PORT=48006 \
\
DEBIAN_FRONTEND=noninteractive

WORKDIR /root

COPY dist/. /root

# RUN \
# pacman -Syu --noconfirm curl glib2 mesa harfbuzz nodejs npm && \
# npm i -g pm2 && \
# rm -rf /var/cache/pacman

RUN \
apt-get update -y && \
apt-get install -y wget libopengl0 libglib2.0-0 libgl1-mesa-glx libharfbuzz0b && \
# install nodejs and npm
if [ $(uname -m) = "x86_64" ]; then \
    wget https://nodejs.org/dist/v14.15.1/node-v14.15.1-linux-x64.tar.gz && \
	tar -C /usr/local --strip-components 1 -xzf node-v14.15.1-linux-x64.tar.gz && \
	rm -f node-v14.15.1-linux-x64.tar.gz; \
else \
	wget https://nodejs.org/dist/v14.15.1/node-v14.15.1-linux-arm64.tar.gz && \
	tar -C /usr/local --strip-components 1 -xzf node-v14.15.1-linux-arm64.tar.gz && \
	rm -f node-v14.15.1-linux-arm64.tar.gz; \
fi; \
npm install -g pm2 && \
# cleanup rest
apt-get remove -y wget && \
apt-get autoremove -y && \
apt-get clean -y && \
npm remove -g npm && \
rm -rf /var/lib/apt/* /var/lib/cache/* /var/lib/dpkg/* /var/cache/* ~/.npm

CMD pm2-runtime start /root/ecosystem.config.js
