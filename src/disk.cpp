#include "disk.hpp"
#include <cstdint>
#include <filesystem>

void DiskManager::read(uint64_t offset, void *buffer, size_t bytes) {
  file.seekg(static_cast<long>(offset));
  file.read(static_cast<char *>(buffer), static_cast<long>(bytes));
}

void DiskManager::write(uint64_t offset, const void *buffer, size_t bytes) {
  file.seekp(static_cast<std::streamsize>(offset));
  file.write(static_cast<const char *>(buffer),
             static_cast<std::streamsize>(bytes));
}

void DiskManager::flush() { file.flush(); }

auto DiskManager::filesize() -> uint64_t {
  auto curr = file.tellg();

  file.seekg(0, std::fstream::end);
  uint64_t filesize = file.tellg();

  file.seekg(curr, std::fstream::beg);
  return filesize;
}

void DiskManager::resize(uint64_t size){
  file.flush();
  std::filesystem::resize_file(filepath, size);
}
