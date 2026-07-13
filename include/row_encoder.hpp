#pragma once

#include "btree.hpp"
#include "table_vals.hpp"

class RowEncoder {
private:
  auto convertBytesToValue(std::byte *bytes, Column &col_info) -> Value;
  auto convertValueToBytes(Value &val, Column &col_info)
      -> std::vector<std::byte>;

public:
  auto serialize(Row &row, Schema &schema) -> Record;
  auto deserialize(Record &record, Schema &schema) -> Row;
};
