#include "btree.hpp"
#include "internal_node.hpp"
#include "leaf_node.hpp"
#include "page_layout.hpp"
#include "page_utils.hpp"
#include "pager.hpp"

Btree::Btree(Pager &pager) : pager(pager) {}

auto Btree::recursiveInsert(PageId root, Record record)
    -> SplitResult {
  Page &page = pager.read(root);
  auto key = record.key;
  if (getPageType(page) == PageType::Leaf) {

    LeafNode leaf(page);
    auto res = leaf.insert(key, record);

    if (res.split) {
      PageId new_page_id = pager.allocatePage();
      Page &new_page = pager.read(new_page_id);
      return leaf.split(new_page, key, record);
    }

  } else {
    InternalNode node(page);
    PageId child_page = node.getChild(key);
    auto res = this->recursiveInsert(child_page, record);

    if (res.left_child != res.right_child) {
      auto insert_result =
          node.insert(res.sep, res.left_child, res.right_child);
      if (insert_result.split) {
        PageId new_page_id = pager.allocatePage();
        Page &new_page = pager.read(new_page_id);
        return node.split(new_page, res.sep, res.left_child, res.right_child);
      }
    }
  }
  return SplitResult{
      .left_child = 0,
      .right_child = 0,
  };
}

auto Btree::insert(PageId root, Record value) -> PageId {
  SplitResult res = this->recursiveInsert(root, value);

  if (res.left_child != res.right_child) {
    PageId new_page_id = pager.allocatePage();
    Page &new_page = pager.read(new_page_id);
    InternalNode node(new_page);
    node.init(res.left_child);
    node.insert(res.sep, res.left_child, res.right_child);
    return new_page_id;
  }
  return root;
}

auto Btree::find(PageId root, Key key) -> std::optional<Cursor> {
  PageId leaf_page_id = this->getLeaf(root, key);
  Page &leaf_page = pager.read(leaf_page_id);

  LeafNode leaf_node(leaf_page);

  if (!leaf_node.contains(key)) {
    return {};
  }

  auto slot_no = leaf_node.getRecordIdx(key);

  if(!slot_no) {
    return {};
  }

  Cursor cur(pager, leaf_page, slot_no.value());

  return cur;
}

auto Btree::findRec(PageId root, Key key) -> std::optional<Record> {
  PageId leaf_page_id = this->getLeaf(root, key);
  Page &leaf_page = pager.read(leaf_page_id);

  LeafNode leaf_node(leaf_page);

  if (!leaf_node.contains(key)) {
    return {};
  }

  auto res = leaf_node.getRecord(key);

  return res;
}

auto Btree::getLeaf(PageId root, Key key) -> PageId {
  PageId id = root;
  while (true) {
    Page &page = pager.read(id);

    if (getPageType(page) == PageType::Leaf) {
      return id;
    }

    InternalNode in_page(page);

    id = in_page.getChild(key);
  }
}

void Btree::remove(PageId root, Key key) {
  PageId leaf_page_id = this->getLeaf(root, key);
  Page &leaf_node_page = pager.read(leaf_page_id);
  LeafNode leaf(leaf_node_page);
  leaf.delete_rec(key);
}

auto Btree::contains(PageId root, Key key) -> bool {
  PageId leaf_page_id = this->getLeaf(root, key);
  Page &leaf_node_page = pager.read(leaf_page_id);
  LeafNode leaf(leaf_node_page);
  return leaf.contains(key);
}
