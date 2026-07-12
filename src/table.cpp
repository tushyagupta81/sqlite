#include "table.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

Table::Table(Btree &btree, TableMetaData meta)
    : btree(btree), metadata(std::move(meta)) {}

void Table::remove(Key key) { btree.remove(metadata.root_page, key); }

auto Table::search(Key key) -> std::optional<Row> {
  auto res = btree.search(metadata.root_page, key);
  if (!res) {
    return {};
  }

  Row row = deserialize(res.value());
  return row;
}

void Table::insert(Row row) {}

auto Table::serialize(Row row) -> Record {
  auto n = row.size();
  Key key;
  for (ColumnId cid : schema.pk_cols) {
    auto col_info = schema.columns[cid];
    auto row_val = row[cid];
    auto val_in_bytes = convertValueToBytes(row_val, col_info);
    key.insert(key.end(), val_in_bytes.begin(), val_in_bytes.end());
  }

  KeySize key_size = key.size();

  Record rec{
      .key_size = key_size,
      .key = key,
  };
  for (auto i = 0; i < schema.columns.size(); i++) {
    auto col_info = schema.columns[i];
    auto row_val = row[i];
    auto val_in_bytes = convertValueToBytes(row_val, col_info);
    rec.record.insert(rec.record.end(), val_in_bytes.begin(),
                      val_in_bytes.end());
  }

  return rec;
}

auto Table::deserialize(Record record) -> Row {}

auto Table::convertValueToBytes(Value &val, Column &col_info)
    -> std::vector<std::byte> {
  std::vector<std::byte> res;
  if (isFixedWidthValue(col_info.type)) {
    switch (col_info.type) {
    case INT: {
      auto in = std::get<int32_t>(val);
      res.reserve(sizeof(in));
      memcpy(res.data(), &in, getValueWidth(col_info.type));
      break;
    }
    case LONG: {
      auto ll = std::get<int64_t>(val);
      res.reserve(sizeof(ll));
      memcpy(res.data(), &ll, getValueWidth(col_info.type));
      break;
    }
    default:
      std::runtime_error("Unknown fixed width column type");
    }
  } else {
    switch (col_info.type) {
    case STRING: {
      auto str = std::get<std::string>(val);
      auto str_len = str.length();
      auto str_len_len = getValueWidth(col_info.type);
      res.reserve(str_len_len + str_len);
      memcpy(res.data(), &str_len, str_len_len);
      memcpy(res.data() + str_len_len, str.data(), str.length());
      break;
    }
    case BLOB: {
      auto blob = std::get<std::vector<std::byte>>(val);
      auto blob_len = blob.size();
      auto blob_len_len = getValueWidth(col_info.type);
      res.reserve(blob_len_len + blob_len);
      memcpy(res.data(), &blob_len, blob_len_len);
      memcpy(res.data() + blob_len_len, blob.data(), blob.size());
      break;
    }
    default:
      std::runtime_error("Unknown fixed width column type");
    }
  }
  return res;
}

auto Table::isFixedWidthValue(ValueType &val_type) -> bool {
  switch (val_type) {
  case INT:
    return true;
  case LONG:
    return true;
  case STRING:
    return false;
  case BLOB:
    return false;
  default:
    std::runtime_error("Unknown column type");
  }
}

auto Table::getValueWidth(ValueType &val_type) -> size_t {
  switch (val_type) {
  case INT:
    return 4;
  case LONG:
    return 8;
  case STRING:
    return 2;
  case BLOB:
    return 2;
  default:
    std::runtime_error("Unknown column type");
  }
}
