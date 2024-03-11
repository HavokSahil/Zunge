cmake_minimum_required(VERSION 3.10)
project(zunge VERSION 1.0)

# Set C standard to C11
set(CMAKE_C_COMPILER gcc-11)

# Set the source files
set(SOURCE_FILES
    src/main.c
    src/epub_parser.c
    src/pdf_parser.c
    src/tts.c
    src/utils.c
)

# Set the header files
set(HEADER_FILES
    include/pdf_parser.h
    include/epub_parser.h
    include/tts.h
    include/utils.h
    /usr/include/zip.h
)

# Add an executable target
add_executable(zunge ${SOURCE_FILES} ${HEADER_FILES})

# Specify include directories
target_include_directories(zunge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include /usr/include)