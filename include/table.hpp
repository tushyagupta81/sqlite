#pragma once

#include "btree.hpp"
#include <cstdint>
#include <optional>
#include <vector>

enum ValueType : uint8_t {
  INT,
  LONG,
  STRING,
  BLOB,
  NONE,
};

using Value = std::string;

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

public:
  explicit Table(Btree &btree, TableMetaData meta);
  void insert(Row row);
  auto search(Key key) -> std::optional<Row>;
  void remove(Key key);
};
