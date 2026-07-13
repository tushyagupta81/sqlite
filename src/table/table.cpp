#include "table.hpp"
#include "row_encoder.hpp"
#include <utility>

Table::Table(Btree &btree, TableMetaData meta, Schema schema, RowEncoder &row_encoder)
    : btree(btree), metadata(std::move(meta)), schema(std::move(schema)), row_encoder(row_encoder) {}

void Table::remove(Key &key) { btree.remove(metadata.root_page, key); }

auto Table::find(Key &key) -> std::optional<Row> {
  auto res = btree.find(metadata.root_page, key);
  if (!res.has_value()) {
    return {};
  }

  Row row = row_encoder.deserialize(res.value(), schema);
  return row;
}

void Table::insert(Row &row) {
  Record rec = row_encoder.serialize(row, schema);

  metadata.root_page = btree.insert(metadata.root_page, rec);
}

// auto Table::serialize(Row &row) -> Record {
//   auto n = row.size();
//   Key key;
//   for (ColumnId cid : schema.pk_cols) {
//     auto col_info = schema.columns[cid];
//     auto row_val = row[cid];
//     auto val_in_bytes = convertValueToBytes(row_val, col_info);
//     key.insert(key.end(), val_in_bytes.begin(), val_in_bytes.end());
//   }
//
//   KeySize key_size = key.size();
//
//   Record rec{
//       .key_size = key_size,
//       .key = key,
//   };
//   for (auto i = 0; i < schema.columns.size(); i++) {
//     auto col_info = schema.columns[i];
//     auto row_val = row[i];
//     auto val_in_bytes = convertValueToBytes(row_val, col_info);
//     rec.record.insert(rec.record.end(), val_in_bytes.begin(),
//                       val_in_bytes.end());
//   }
//
//   return rec;
// }
//
// auto Table::deserialize(Record &record) -> Row {
//   auto n_cols = schema.columns.size();
//   Row row;
//   row.reserve(n_cols);
//
//   auto *offset = record.record.data();
//   for (auto i = 0; i < n_cols; i++) {
//     auto col_info = schema.columns[i];
//     auto val = convertBytesToValue(offset, col_info);
//     row.push_back(val);
//     switch (col_info.type) {
//     case ValueType::INT:
//       offset += 4;
//       break;
//     case ValueType::LONG:
//       offset += 8;
//       break;
//     case ValueType::STRING:
//       offset += 2;
//       offset += std::get<std::string>(val).length();
//       break;
//     case ValueType::BLOB:
//       offset += 2;
//       offset += std::get<std::vector<std::byte>>(val).size();
//       break;
//     }
//   }
//
//   return row;
// }

// auto Table::convertBytesToValue(std::byte *bytes, Column &col_info) -> Value {
//   switch (col_info.type) {
//   case ValueType::INT:
//     return readBigEndian<int32_t>(bytes);
//
//   case ValueType::LONG:
//     return readBigEndian<int64_t>(bytes);
//
//   case ValueType::STRING: {
//     auto len = readBigEndian<DataLen>(bytes);
//
//     return std::string(reinterpret_cast<const char *>(bytes + sizeof(DataLen)),
//                        len);
//   }
//
//   case ValueType::BLOB: {
//     auto len = readBigEndian<DataLen>(bytes);
//
//     return std::vector<std::byte>(bytes + sizeof(DataLen),
//                                   bytes + sizeof(DataLen) + len);
//   }
//   default:
//     throw std::runtime_error("Unknown column type in convert value to bytes");
//   }
// }

// auto Table::convertValueToBytes(Value &val, Column &col_info)
//     -> std::vector<std::byte> {
//   std::vector<std::byte> res;
//   switch (col_info.type) {
//   case ValueType::INT: {
//     auto in = std::get<int32_t>(val);
//     appendBigEndian<int32_t>(res, in);
//     break;
//   }
//   case ValueType::LONG: {
//     auto ll = std::get<int64_t>(val);
//     appendBigEndian<int64_t>(res, ll);
//     break;
//   }
//   case ValueType::STRING: {
//     const auto &str = std::get<std::string>(val);
//
//     appendBigEndian<DataLen>(res, str.size());
//
//     res.insert(res.end(), reinterpret_cast<const std::byte *>(str.data()),
//                reinterpret_cast<const std::byte *>(str.data() + str.size()));
//     break;
//   }
//   case ValueType::BLOB: {
//     const auto &blob = std::get<std::vector<std::byte>>(val);
//
//     appendBigEndian<DataLen>(res, blob.size());
//
//     res.insert(res.end(), blob.begin(), blob.end());
//     break;
//   }
//   default:
//     throw std::runtime_error("Unknown column type in convert value to bytes");
//   }
//   return res;
// }
