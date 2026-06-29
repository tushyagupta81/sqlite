#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>

class DiskManager{
  private:
    std::fstream file;
    std::filesystem::path filepath;
  public:
    void read(uint64_t offset, void *buffer, size_t bytes);
    void write(uint64_t offset, const void *buffer, size_t bytes);

    void flush();

    auto filesize() -> uint64_t;
    void resize(uint64_t size);
};
