#pragma once

#include "pager.hpp"
#include <cstdint>
#include <optional>
#include <vector>

using KeySize = uint16_t;
using Key = std::vector<std::byte>;

struct Record {
  KeySize key_size;
  Key key;
  std::vector<std::byte> record;
};

struct InsertResult {
  bool split;
  PageId old_page;
};

struct SplitResult {
  Key sep;
  PageId left_child;
  PageId right_child;
};

class Btree {
public:
  explicit Btree(Pager &pager);
  auto insert(PageId root, Key key, Record value) -> PageId;
  auto search(PageId root, Key key) -> std::optional<Record>;
  void remove(PageId root, Key key);
  auto contains(PageId root, Key key) -> bool;

private:
  auto getLeaf(PageId root, Key key) -> PageId;
  auto recursiveInsert(PageId root, Key key, Record record) -> SplitResult;
  Pager &pager;
};
