#pragma once

#include "pager.hpp"
#include <cstdint>
#include <optional>
#include <vector>

struct Record{
  std::vector<std::byte> record;
};

using Key = uint64_t;

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
  void insert(PageId root, Key key, Record value);
  auto search(PageId root, Key key) -> std::optional<Record>;
  void erase(PageId root, Key key);

private:
  auto getLeaf(PageId root, Key key) -> PageId;
  Pager& pager;
};
