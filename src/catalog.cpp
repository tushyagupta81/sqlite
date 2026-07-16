#include "catalog.hpp"
#include "btree.hpp"
#include "row_encoder.hpp"
#include "table.hpp"
#include <cstring>
#include <memory>

Catalog::Catalog(Pager &pager)
    : pager(pager), btree(Btree(pager)), row_encoder(RowEncoder()) {
  auto page_zero = pager.read(0);

  std::memcpy(&db_header, page_zero.data.data(), sizeof(db_header));

  TableMetaData main_table_meta = {
      .table_name = "main_table",
      .root_page = db_header.main_table,
  };

  main_table = std::make_unique<Table>(btree, main_table_meta,
                                       table_table_schema, row_encoder);
}

auto Catalog::getTable(std::string table_name) -> std::optional<Table> {
  Value val = table_name;
  auto table_entry_serialized = row_encoder.serializeValue(val);
  Key table_entry_key = table_entry_serialized;
  auto table_entry = main_table->find(table_entry_key);

  if (!table_entry.has_value()) {
    return {};
  }

  auto root_page = std::get<int32_t>(table_entry.value()[2]);
  TableMetaData table_meta = {
      .table_name = table_name,
      .root_page = static_cast<uint32_t>(root_page),
  };

  Schema schema{};

  auto tbl = Table(btree, table_meta, schema, row_encoder);

  return tbl;
}
