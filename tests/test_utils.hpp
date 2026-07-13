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

template <typename T> inline auto makeKey(T value) -> Key {
  using U = std::make_unsigned_t<T>;

  U u = static_cast<U>(value);
  Key key(sizeof(T));

  for (size_t i = 0; i < sizeof(T); ++i) {
    key[i] = static_cast<std::byte>((u >> ((sizeof(T) - 1 - i) * 8)) & 0xFF);
  }

  return key;
}
