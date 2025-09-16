cmake_minimum_required(VERSION 3.13)

project(codecrafters-interpreter)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

add_compile_options(-Wall -Wextra -Winline -Werror -fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(interpreter ${SOURCE_FILES})
