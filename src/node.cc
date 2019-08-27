#include "node.h"

template<typename K, typename T>
Node<K, T>::~Node() {
  if (parent_) {
    parent_->children_.erase(name()); // detach
    parent_ = nullptr;
  }
  ClearChildren();
}

template<typename K, typename T>
void Node<K, T>::ClearChildren() {
  for (auto i = children_.begin(); i != children_.end(); i++) {
    Node *node = i->second;
    if (node) {
      node->parent_ = nullptr;
      delete node;
    }
  }
  children_.clear();
}

template<typename K, typename T>
void Node<K, T>::AddChild(Node *child_node) {
  if (HasChild(child_node->name())) {
    delete children_[child_node->name()];
    children_.erase(child_node->name());
  }
  children_[child_node->name()] = child_node;
}

template<typename K, typename T>
Node<K, T> *Node<K, T>::CreateChild(const K &name, Node *parent_node) {
  if (!parent_node) parent_node = this;
  Node *node = new Node(name, parent_node);
  AddChild(node);
  return node;
}

template<typename K, typename T>
void Node<K, T>::setParent(Node *parent_node) {
  if (parent_ && (parent_ != parent_node)) {
    parent_->children_.erase(name());
  }

  parent_ = parent_node;
  if (parent_) parent_->children_[name()] = this;
}

template<typename K, typename T>
Node<K, T> *Node<K, T>::Find(const Node::Path &path, int depth) {
  Node *res = GetChild(path[depth]);// do we have the child at this level?
  if (res) {
    depth++;
    if (depth < (int) path.size()) {
      res = res->Find(path, depth);
    }
  }
  return res;
}

template<typename K, typename T>
Node<K, T> *Node<K, T>::Add(const Node::Path &path) {
  Node *new_node = Find(path); // Only create if it does not exist.
  if (!new_node) {
    // Create the path as required.
    new_node = this;
    int depth = 0;
    while (new_node->HasChild(path[depth])) {
      new_node = new_node->GetChild(path[depth]);
      depth++;
    }
    // Create the rest.
    for (auto i = static_cast<unsigned int>(depth); i < path.size(); i++) {
      new_node = new_node->CreateChild(path[i]);
    }
  }

  return new_node;
}

template<typename K, typename T>
void Node<K, T>::Remove(const Node::Path &path) {
  Node *p = Find(path);

  if (p) {
    delete p;
  }
}

template<typename K, typename T>
bool Node<K, T>::IterateNodes(std::function<bool(Node &)> func) {
  if (func(*this)) {
    for (auto i = children().begin(); i != children().end(); i++) {
      (i->second)->iterateNodes(func);
    }
    return true;
  }
  return false;
}

template<typename K, typename T>
void Node<K, T>::IterateNodes(Node::NodeIteratorFunc &func) {
  func.Do(this); // action the function for the node
  for (auto i = children().begin(); i != children().end(); i++) {
    (i->second)->iterateNodes(func);
  }
}

template<typename K, typename T>
template<typename STREAM>
void Node<K, T>::write(STREAM &output_stream) {
  output_stream << name();
  output_stream << data();
  output_stream << static_cast<int>(children().size()); // this node's data
  // now recurse
  if (children().size() > 0) {
    for (auto i = children().begin(); i != children().end(); i++) {
      i->second->write(output_stream);
    }
  }
}

template<typename K, typename T>
template<typename STREAM>
void Node<K, T>::read(STREAM &input_stream) {
  int n = 0;
  ClearChildren();
  input_stream >> name_;
  input_stream >> data_;
  input_stream >> n;
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      Node *o = new Node();
      o->read(input_stream); // recurse
      AddChild(o); // add subtree to children
    }
  }
}

template<typename K, typename T>
void Node<K, T>::CopyTo(Node *node) {
  node->ClearChildren();
  node->setData(name());
  node->setData(data());
  if (children().size() > 0) {
    for (auto i = children().begin(); i != children().end(); i++) {
      Node *c = new Node();
      node->AddChild(node); // add the child
      i->second->copyTo(c); // now recurse
    }
  }
}
