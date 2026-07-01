#include "internal_node.hpp"
#include "pager.hpp"

InternalNode::InternalNode(Page &page) : page(page) {}

auto InternalNode::header() -> InternalHeader & {
  return *reinterpret_cast<InternalHeader *>(page.data.data());
}

auto InternalNode::header() const -> const InternalHeader & {
  return *reinterpret_cast<const InternalHeader *>(page.data.data());
}
