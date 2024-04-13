FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    software-properties-common \
    python3 \
    python3-pip \
    python3-setuptools \
    cmake \
    make \
    gcc-11 \
    g++-11 \
    libzip-dev \
    libxml2-dev \
    libmp3lame-dev \
    lame \
    festival \
    libavformat-dev \
    libavcodec-dev \
    libavutil-dev \
    libncurses-dev \
    zlib1g-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

COPY . /app
WORKDIR /app
RUN chmod +x install.sh

# Execute install.sh script
RUN ./install.sh

EXPOSE 4000

# Set the startup command
CMD ["bash"]
