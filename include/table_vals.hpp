#pragma once

#include "pager.hpp"
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

using KeySize = uint16_t;
using Key = std::vector<std::byte>;

struct Record {
  KeySize key_size;
  Key key;
  std::vector<std::byte> record;
};

struct InsertResult {
  bool split;
  PageId old_page;
};

struct SplitResult {
  Key sep;
  PageId left_child;
  PageId right_child;
};

enum class ValueType : uint8_t {
  INT,
  LONG,
  STRING,
  BLOB,
};

using Value =
    std::variant<int32_t, int64_t, std::string, std::vector<std::byte>>;

using Row = std::vector<Value>;
using ColumnId = uint8_t;
using DataLen = uint16_t;

struct Column {
  ColumnId id;
  std::string name;

  ValueType type;
};

struct TableMetaData {
  std::string table_name;
  PageId root_page;
  // std::string create_str;
};

struct Schema {
  std::vector<Column> columns;
  std::vector<ColumnId> pk_cols;
};
