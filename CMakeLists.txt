cmake_minimum_required(VERSION 4.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.10.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

project(sqlite)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_library(sqlite_lib)

target_sources(sqlite_lib
  PRIVATE
   src/disk.cpp
   src/sql/executor.cpp
   src/sql/parser.cpp
   src/sql/planner.cpp
   src/sql/tokenizer.cpp
   src/pager/pager.cpp
   src/pager/freelist.cpp
   src/pager/wal.cpp
   src/pager/bufferpool.cpp
   src/btree/btree.cpp
   src/btree/internal_node.cpp
   src/btree/leaf_node.cpp
   src/btree/page_utils.cpp
)

target_include_directories(sqlite_lib
  PUBLIC
   include
)

add_executable(sqlite src/main.cpp)

target_link_libraries(sqlite
  PRIVATE
    sqlite_lib
)

enable_testing()

add_subdirectory(tests)
