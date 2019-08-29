#include "property_tree.h"

template<typename K, typename T>
void PropertyTree<K, T>::clear() {
  WriteLock l(mutex_);
  root_.ClearChildren();
  SetChanged();
}

template<typename K, typename T>
template<typename P>
T &PropertyTree<K, T>::GetData(const P &path) {
  ReadLock l(mutex_);
  auto *p = root_.Find(path);
  if (p) {
    return p->data();
  }
  return default_data_;
}

template<typename K, typename T>
template<typename P>
void PropertyTree<K, T>::SetData(const P &path, const T &data) {
  auto node = root_.Find(path);
  if (!node) {
    WriteLock l(mutex_);
    node = root_.Add(path);
  }
  if (node) {
    WriteLock l(mutex_);
    node->SetData(data);
  }
  SetChanged();
}

template<typename K, typename T>
void PropertyTree<K, T>::GetFullPath(PropertyTree::PropertyNode *node, PropertyTree::Path &path) {
  path.clear();
  if (node) {
    ReadLock l(mutex_);
    while (node->parent() != nullptr) {
      path.push_back(node->name());
      node = node->parent();
    }
    std::reverse(std::begin(path), std::end(path));
  }
}

template<typename K, typename T>
T &PropertyTree<K, T>::GetChildData(PropertyTree::PropertyNode *node, const K &child_name, T &default_data) {
  ReadLock l(mutex_);
  if (node && node->HasChild(child_name)) {
    return node->GetChild(child_name)->data();
  }
  return default_data;
}

template<typename K, typename T>
void PropertyTree<K, T>::SetChildData(PropertyTree::PropertyNode *node, const K &child_name, const T &child_data) {
  if (node) {
    WriteLock l(mutex_);
    if (node->HasChild(child_name)) {

      node->GetChild(child_name)->SetData(child_data);
    } else {
      PropertyNode *c = node->CreateChild(child_name);
      c->SetData(child_data);
    }
    SetChanged();
  }
}

template<typename K, typename T>
template<typename P>
unsigned long PropertyTree<K, T>::ListChildren(const P &path, std::vector<K> &children_list) {
  auto i = Find(path);
  if (i) {
    ReadLock lx(mutex_);
    for (auto j = i->children().begin(); j != i->children().end(); j++) {
      children_list.push_back(j->first);
    }
  }
  return children_list.size();
}
