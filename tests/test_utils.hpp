#pragma once

#include "btree.hpp"
#include <cstring>
#include <filesystem>
#include <string>

class TempDB {
public:
  explicit TempDB(std::string name)
      : path(std::filesystem::temp_directory_path() / (name + ".db")) {
    std::filesystem::remove(path);
  }

  ~TempDB() { std::filesystem::remove(path); }

  [[nodiscard]] auto file() const -> const std::filesystem::path & {
    return path;
  }

private:
  std::filesystem::path path;
};

static auto makeRecord(const std::string &s, Key key) -> Record {
  Record r;
  r.key_size = key.size();
  r.key = key;
  r.record.resize(s.size());

  std::memcpy(r.record.data(), s.data(), s.size());

  return r;
}

static auto recordToString(const Record &record) -> std::string {
  std::string s;
  s.reserve(record.record.size());

  for (std::byte b : record.record) {
    s.push_back(static_cast<char>(b));
  }

  return s;
}

inline auto makeKey(uint64_t value) -> Key {
  Key key(8);

  for (int i = 7; i >= 0; --i) {
    key[7 - i] = static_cast<std::byte>(value >> (i * 8));
  }

  return key;
}
