# Use the official Ubuntu 20.04 LTS image
FROM ubuntu:20.04

# Set environment variables to avoid interactive sessions during installation
ENV DEBIAN_FRONTEND=noninteractive

# Add GCC 11 repository and update package lists
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    software-properties-common \
    && add-apt-repository ppa:ubuntu-toolchain-r/test -y \
    && apt-get update

# Install GCC 11 and essential build tools
RUN apt-get install -y --no-install-recommends \
    gcc-11 \
    g++-11 \
    cmake \
    build-essential \
    libzip-dev \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 11 as the default compiler alternatives
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100

# Set a default working directory
WORKDIR /app

# Copy all files from the host into the container
COPY . .

# Define a volume for the build folder
VOLUME /app/build

# Entry point command
CMD ["bash"]
