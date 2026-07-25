#include "table.hpp"
#include "row_encoder.hpp"
#include <utility>

Table::Table(Btree &btree, TableMetaData meta, Schema schema, RowEncoder &row_encoder)
    : btree(btree), metadata(std::move(meta)), schema(std::move(schema)), row_encoder(row_encoder) {}

void Table::remove(Key &key) { btree.remove(metadata.root_page, key); }

auto Table::find(Key &key) -> std::optional<Row> {
  auto res = btree.findRec(metadata.root_page, key);

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

auto Table::serializeKey(Row &row) -> Key {
  return row_encoder.serializeKey(row, schema);
}
