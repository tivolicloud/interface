# FROM archlinux:latest
FROM ubuntu:18.04

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

# RUN \
# pacman -Syu --noconfirm curl glib2 mesa harfbuzz nodejs npm && \
# npm i -g pm2 && \
# rm -rf /var/cache/pacman

RUN \
apt-get update -y && \
apt-get install -y curl glib2.0 libgl1-mesa-glx libharfbuzz-bin libopengl0 && \
curl -sL https://deb.nodesource.com/setup_12.x | bash - && \
apt-get install -y nodejs && \
npm i -g pm2 && \
# cleanup
apt-get clean -y && \
rm -rf /var/lib/apt/lists/* /var/cache/apt/*

COPY dist/. /root

CMD pm2-runtime start /root/ecosystem.config.js
