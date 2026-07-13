#pragma once

#include "btree.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

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

class Table {
private:
  Btree &btree;
  Schema schema;
  TableMetaData metadata;

  auto serialize(Row &row) -> Record;
  auto deserialize(Record &record) -> Row;
  auto convertValueToBytes(Value &val, Column &col_info)
      -> std::vector<std::byte>;

  auto convertBytesToValue(std::byte *bytes, Column &col_info) -> Value;

public:
  explicit Table(Btree &btree, TableMetaData meta, Schema schema);
  void insert(Row &row);
  auto find(Key &key) -> std::optional<Row>;
  void remove(Key &key);
};

const Schema table_table_schema = {
    .columns =
        {
            Column{
                .id = 0,
                .name = "table_idx",
                .type = ValueType::INT,
            },
            Column{
                .id = 1,
                .name = "table_name",
                .type = ValueType::STRING,
            },
            Column{
                .id = 2,
                .name = "n_cols",
                .type = ValueType::INT,
            },
            Column{
                .id = 3,
                .name = "create_str",
                .type = ValueType::STRING,
            },
        },
    .pk_cols = {0},
};
