#include "btree.hpp"
#include "leaf_node.hpp"
#include "pager.hpp"

#include <cstring>
#include <iostream>
#include <string>

static auto makeRecord(const std::string &s) -> Record {
  Record r;
  r.record.resize(s.size());

  std::memcpy(r.record.data(), s.data(), s.size());

  return r;
}

static void printRecord(const Record &r) {
  std::string s(reinterpret_cast<const char *>(r.record.data()),
                r.record.size());

  std::cout << s << '\n';
}

auto main() -> int {
  Pager pager("test.db");
  Btree tree(pager);

  //
  // Create first page as the root.
  //
  PageId root = pager.allocatePage();

  Page &page = pager.read(root);

  LeafNode leaf(page);
  leaf.init();

  //
  // Insert.
  //
  for (uint32_t i = 1; i <= 1000; ++i) {
    root = tree.insert(root, i, makeRecord("Value " + std::to_string(i)));
  }

  //
  // Fetch.
  //
  for (uint32_t i = 1; i <= 1000; ++i) {
    auto rec = tree.search(root, i);
    if (rec) {
      printRecord(rec.value());
    } else {
      std::cout << "Rec " << i << " not found\n";
    }
  }
  //
  // Delete.
  //

  for (uint32_t i = 2; i <= 1000; i += 2) {
    tree.delete_rec(root, i);
  }
  //
  // Verify.
  //

  for (uint32_t i = 1; i <= 1000; ++i) {
    auto rec = tree.search(root, i);
    if (rec) {
    } else {
      std::cout << "Rec " << i << " not found\n";
    }
  }

  return 0;
}
