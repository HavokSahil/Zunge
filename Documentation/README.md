
# Zunge: How to make it
Zunge is an basically a text to audio converter with some enhanced functionality to enable it to read document files like PDF, EPUB and text files. It exports the audio file in mp3 format. It also includes an interactive reader which reads through document.

> **Note that we are not creating an application for speech generation from text**. But we are using opensource [`festival`](https://www.cstr.ed.ac.uk/projects/festival/) for speech generation. 
*Festival* is well suited for light weight free text to speech service with long text reader and document reader. It generates audio file in uncompressed *wave* file format, which is comparatively larger in size. Therefore for that we have used [`lame`](https://lame.sourceforge.io/) for audio compression from *.wav* to *.mp3*.

# Setup: How to Start

This project targets linux environment. And has been developed and tested in Ubuntu.
So our first step is going to be to install Ubuntu Operating System in Virtual Box.

### I. Setting up VirtualBox

For setup we will be downloading two files:
 - Start by downloading Virtual Box for Windows. [click here](https://download.virtualbox.org/virtualbox/7.0.14/VirtualBox-7.0.14-161095-Win.exe)
 - Download Ubuntu LTS .iso image [click here](https://ubuntu.com/download/desktop/thank-you?version=22.04.4&architecture=amd64)

Follow this [link](https://www.youtube.com/watch?v=v1JVqd8M3Yc) to setup VirtualBox with Ubuntu on Windows

### II. Installing Required Libraries
```bash
sudo apt update
sudo apt upgrade

# install required libraries
sudo apt-get install -y libzip-dev libxml2-dev cmake libmp3lame-dev \
    python3 python3-pip lame festival libavformat-dev libavcodec-dev \
    libavutil-dev  gedit && \
    pip3 install --no-cache-dir setuptools PyMuPDF

```

### III. Familiarising with CMAKE build tool.
![logo](https://github.com/HavokSahil/Zunge/assets/87008169/dada207b-8c1e-4c75-8b81-2b6b2d6316ae) 

For a large project with multiple libraries and dependencies we generally use build tools to make our project.
> *Imagine a project where you have 2000 lines of codes around 10-12 files. And you have finally managed to complete the project after 2 long weekends. Now you want to compile your project. Where will you start? Its gets disastorous quickly to link all dependencies together and compilation is too messy.*
*To avoid that and to ease our life we use build tools, **CMAKE** is just one of them. You just have to tell it what are the source files what are the libraries and dependencies and the headers and flags. It handles everything for you, there is just no reason why one shouldn't use it.*
> 
**Note: You don't have to learn CMAKE now, just have an idea what it is. I will provide the code for it to you. Its not important to know it for this project.**

### Project Directory Structure
Following is the project structure of Zunge
```md
Zunge/
├── CMakeLists.txt
├── include/
│   ├── epub_parser.h
│   ├── interactive.h
│   ├── pdf_parser.h
│   ├── tts.h
│   └── utils.h
├── src/
│   ├── epub_parser.c
│   ├── interactive.c
│   ├── main.c
│   ├── pdf_parser.c
│   ├── pdf_to_text.py
│   ├── tts.c
│   └── utils.c
└── structure.md
```
The root contains two folders and one file
 - `src`: this folder contains all source files and implementations.
 - `include`: it contains all header files for function defination
 - `CMakeLists.txt`: this is the file for CMAKE build tool. You don't have to worry about it. We just put instructions here to compile our apparently long project

Use following code to create this project structure
```bash
mkdir Zunge
cd Zunge && mkdir src include && touch CMakeLists.txt
touch src/epub_parser.c src/interactive.c src/pdf_parser.c src/utils.c src/tts.c src/main.c src/pdf_to_text.py
touch include/epub_parser.h include/interactive.h include/pdf_parser.h include/utils.h include/tts.h
```

Now since the project directory is ready. You can start editing those files.

# Writing Code
Finally the second best of project. Writing Code!

I will be giving here the code for all the files and will be explaining each of them.

```
currently working on it
```
