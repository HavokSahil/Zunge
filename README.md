![logo](https://github.com/HavokSahil/Zunge/assets/87008169/a5d7f23e-d5e3-4348-9a20-70345ece156e)



# Zunge

Zunge is a text documents to audio converter with interactive reader. It uses [festival library](http://festvox.org/festival/) for audio generation and [lame](https://lame.sourceforge.io/) for audio compression.

## Features

- PDF, EPUB to MP3
- Interactive Reader
- Lightweight
- Runs on terminal
- Multiple theme options


## Deployment

To deploy this project, clone this repository

```bash
  git clone https://github.com/havoksahil/zunge
  cd zunge
  chmod +x install.sh
  bash install.sh

  # to uninstall zunge
  chmod +x uninstall.sh
  bash uninstall.sh
```


## Usage/Examples

```bash
# create mp3 file from pdf
zunge -f example.pdf -o out.mp3

# create mp3 file from epub
zunge -f example.epub -o story.mp3

# start interactive reader
zunge -f example.epub -i

# start interactive reader with speak on with theme active
zunge -f example.pdf -i -s -t 3
```


## Screenshots

![App Screenshot](https://github.com/HavokSahil/Zunge/assets/87008169/602fa937-0ab7-4040-b82d-c44324bc20ec)


## Arguments



| Flags     | Name     | Description                |
| :-------- | :------- | :------------------------- |
| `-f`      | `filename`| **Required**. input filename |
| `-o`      | `filename` | **Optional**. output filename |
| `-i`      | `interactive` | **Optional**. interactive mode |
| `-s`      | `reader` | **Optional**. read out the text |
| `-t`      | `theme` | **Optional**. choose theme |
| `-h`      | `filename` | **Optional**. show help |


## Tech Stack

**Application:** C, Bash, Python


## Support

For support, email havoksahil@gmail.com
