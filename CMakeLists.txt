cmake_minimum_required(VERSION 4.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sqlite)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_executable(sqlite)

target_sources(sqlite
  PRIVATE
   src/main.c
   src/read_headers.c
   src/file.c
   src/cells.c
   src/mem.c
   src/page.c
)

target_include_directories(sqlite
   PRIVATE
   include)
