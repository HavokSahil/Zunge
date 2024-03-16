# Use an official Ubuntu image as the base image
FROM ubuntu:latest

# Update package lists and install necessary dependencies
RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y gcc-11 g++-11

# Set the working directory in the container
WORKDIR /app

# Copy the source code into the container
COPY . /app

# Install other necessary dependencies
RUN apt-get install -y libzip-dev libxml2-dev cmake libmp3lame-dev python3 python3-pip lame festival libavformat-dev libavcodec-dev libavutil-dev && \
    pip3 install --upgrade pip && \
    pip3 install --no-cache-dir setuptools PyMuPDF

# Set the default compiler to gcc-11
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100

# Expose any ports the app runs on
EXPOSE 4000

# Set the startup command
CMD ["bash"]
