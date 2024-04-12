cmake_minimum_required(VERSION 3.10)
project(zunge VERSION 1.0)

# Set C standard to C11
set(CMAKE_C_COMPILER gcc-11)
# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -lzip -lz -lavformat -lavcodec -lavutil")

find_library(LIBZIP_LIBRARY NAMES zip)
find_library(LAME_LIBRARIES NAMES mp3lame)

find_package(LibXml2 REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
find_package(Curses REQUIRED)
find_package(ZLIB REQUIRED)


# Set the source files
set(SOURCE_FILES
    src/zunge.c
    src/doc_parser.c
    src/tts.c
    src/utils.c
    src/interactive.c
)

# Set the header files
set(HEADER_FILES
    include/doc_parser.h
    include/tts.h
    include/utils.h
    include/interactive.h
)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include /usr/include ${LIBXML2_INCLUDE_DIR})
configure_file(${SRC_DIR}/pdf_to_text.py ${BUILD_DIR}/pdf_to_text.py COPYONLY)
# Add an executable target
add_executable(zunge ${SOURCE_FILES} ${HEADER_FILES})

# Define the Python script path as a compile defination
target_compile_definitions(zunge PUBLIC PYTHON_SCRIPT_PATH=${CMAKE_SOURCE_DIR}/src/pdf_to_text.py)
# Specify include directories
target_include_directories(zunge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CURSES_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})
target_link_libraries(zunge ${LIBZIP_LIBRARY} ${LIBXML2_LIBRARIES} ${LAME_LIBRARIES} ${CURSES_LIBRARIES} ${ZLIB_LIBRARIES})

install(TARGETS zunge DESTINATION bin)
