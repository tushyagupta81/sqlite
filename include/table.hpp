#pragma once

#include "btree.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

enum ValueType : uint8_t {
  INT,
  LONG,
  STRING,
  BLOB,
};

using Value =
    std::variant<int32_t, int64_t, std::string, std::vector<std::byte>>;

using Row = std::vector<Value>;
using ColumnId = uint8_t;

struct TypeInfo {
  bool fixed_size;
  uint16_t size;
};

struct Column {
  ColumnId id;
  std::string name;

  ValueType type;
};

struct TableMetaData {
  std::string table_name;
  PageId root_page;
  std::string create_str;
};

struct Schema {
  std::vector<Column> columns;
  std::vector<ColumnId> pk_cols;
};

class Table {
private:
  Btree &btree;
  Schema schema;
  TableMetaData metadata;

  auto serialize(Row row) -> Record;
  auto deserialize(Record record) -> Row;
  auto convertValueToBytes(Value &val, Column &col_info)
      -> std::vector<std::byte>;

  auto isFixedWidthValue(ValueType &val_type) -> bool;
  auto getValueWidth(ValueType &val_type) -> size_t;

public:
  explicit Table(Btree &btree, TableMetaData meta);
  void insert(Row row);
  auto search(Key key) -> std::optional<Row>;
  void remove(Key key);
};
