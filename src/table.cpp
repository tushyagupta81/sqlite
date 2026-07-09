#include "table.hpp"
#include <utility>

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

void Table::insert(Row row) {
}

auto Table::serialize(Row row) -> Record {}
auto Table::deserialize(Record record) -> Row {}
