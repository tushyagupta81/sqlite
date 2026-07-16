#pragma once

#include "btree.hpp"
#include "table_vals.hpp"
#include <vector>

class RowEncoder {
private:
  auto convertBytesToValue(std::byte *bytes, ValueType val_type) -> Value;
  auto convertValueToBytes(Value &val, ValueType val_type)
      -> std::vector<std::byte>;

  auto getValueType(const Value &value) -> ValueType;

public:
  auto serialize(Row &row, Schema &schema) -> Record;
  auto serializeKey(Row &row, Schema &schema) -> Key;
  auto serializeValue(Value &value) -> std::vector<std::byte>;
  auto deserialize(Record &record, Schema &schema) -> Row;
};
