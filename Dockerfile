FROM dorowu/ubuntu-desktop-lxde-vnc:latest
ENV USER root
RUN apt-get update && apt-get install -y \
    net-tools \
    iputils-ping \
    iproute2 \
    macchanger \
    make \
    gcc \
    musl-dev