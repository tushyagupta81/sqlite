#include "btree.hpp"
#include "internal_node.hpp"
#include "page_utils.hpp"
#include "page_layout.hpp"


Btree::Btree(Pager &pager): pager(pager){
}

void Btree::insert(PageId root, Key key, Record value){
}

auto Btree::search(PageId root, Key key) -> std::optional<Record>{
}

auto Btree::getLeaf(PageId root, Key key) -> PageId{
  PageId id = root;
  while(true) {
    Page &page = pager.read(id);

    if (getPageType(page) == PageType::Leaf) {
      return id;
    }

    InternalNode in_page(page);

    id = getChild(page, key);
  }
  return id;
}

void Btree::erase(PageId root, Key key){
}
