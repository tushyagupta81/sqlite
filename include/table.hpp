#pragma once

#include "btree.hpp"
#include "row_encoder.hpp"

class Table {
private:
  Btree &btree;
  Schema schema;
  TableMetaData metadata;
  RowEncoder &row_encoder;

public:
  explicit Table(Btree &btree, TableMetaData meta, Schema schema,
                 RowEncoder &row_encoder);
  void insert(Row &row);
  auto find(Key &key) -> std::optional<Row>;
  void remove(Key &key);
  auto serializeKey(Row &row) -> Key;
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
                .name = "root_page",
                .type = ValueType::INT,
            },
            Column{
                .id = 3,
                .name = "n_cols",
                .type = ValueType::INT,
            },
            Column{
                .id = 4,
                .name = "create_str",
                .type = ValueType::STRING,
            },
        },
    .pk_cols = {1},
};
