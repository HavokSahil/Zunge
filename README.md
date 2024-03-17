# Zunge
**Zunge** is a OpenSource document reader. It generates mp3 file from the documents like *PDF* or *EPUB*. It uses [festival library](https://github.com/festvox/festival) for voice generation and [lame library](https://lame.sourceforge.io/)  for audio compression.

## Requirements
Install required libraries with 

`$ apt-get install -y libzip-dev libxml2-dev cmake libmp3lame-dev python3 python3-pip lame festival libavformat-dev libavcodec-dev libavutil-dev` 

`$ pip3 install --no-cache-dir setuptools PyMuPDF` 

**GCC-11**

## Installation
Clone the repository with \
`$ git clone https://github.com/HavokSahil/Zunge` \
`$ cd Zunge`


Build Zunge binaries \
`$ mkdir build` \
`$ cd build` \
`$ cmake ..` \
`$ make` 

Install the binaries \
`$ make install` 

See Usage with \
`$ zunge -h`

## Interactive Reader Mode
![image](https://github.com/HavokSahil/Zunge/assets/87008169/e81237f7-c34f-4f2a-a3c1-9eae62be4879)

## Running with Docker
You must have docker installed.\
`$ docker build -t zunge-env .` \
`$ docker run -it zunge-env`
