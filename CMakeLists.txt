cmake_minimum_required(VERSION 4.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sqlite)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_executable(sqlite)

target_sources(sqlite
  PRIVATE
   src/main.cpp
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

target_include_directories(sqlite
 PRIVATE
   include
)
