#pragma once

#include "cursor.hpp"
#include "pager.hpp"
#include "table_vals.hpp"
#include <optional>

class Btree {
public:
  explicit Btree(Pager &pager);
  auto insert(PageId root, Record value) -> PageId;
  auto find(PageId root, Key key) -> std::optional<Cursor>;
  auto findRec(PageId root, Key key) -> std::optional<Record>;
  void remove(PageId root, Key key);
  auto contains(PageId root, Key key) -> bool;

private:
  auto getLeaf(PageId root, Key key) -> PageId;
  auto recursiveInsert(PageId root, Record record) -> SplitResult;
  Pager &pager;
};
