#ifndef OBJECT_PROPERTY_TREE_PROPERTY_TREE_H
#define OBJECT_PROPERTY_TREE_PROPERTY_TREE_H

#include <boost/thread.hpp>
#include "node.h"
#include "node_path.h"

// Mutexs - tree access needs to be thread safe.
typedef boost::shared_mutex ReadWriteMutex;
typedef boost::shared_lock<boost::shared_mutex> ReadLock;
typedef boost::unique_lock<boost::shared_mutex> WriteLock;

template<typename K, typename T>
class PropertyTree {
  mutable ReadWriteMutex mutex_; ///< Mutex for read/write access.
  bool changed_ = false; ///< Track if any action may have changed the tree.

 public:
  typedef Node<K, T> PropertyNode;
  typedef NodePath<K> Path;

 private:
  PropertyNode root_; ///< The root node.

 public:

  /**
   * @brief Create a property tree.
   */
  PropertyTree() : root_("__ROOT__") {
    root_.ClearChildren();
  }

  /**
   * @brief Delete the property tree.
   */
  virtual ~PropertyTree() {
    root_.ClearChildren();
  }

  /**
   * @return The read/ write mutex.
   */
  ReadWriteMutex &mutex() { return mutex_; }

  /**
   * @return The changed flag. True if something has changed.
   */
  bool changed() const { return changed_; }

  /**
   * @brief Set changed to false.
   */
  void ClearChanged() { changed_ = false; }

  /**
   * @brief Set the changed flag. Default is true.
   * @param changed The flag to set.
   */
  void SetChanged(bool changed = true) { changed_ = changed; }

  /**
   * @brief Clear / delete all nodes (other than root) from the tree.
   */
  void clear() {
    WriteLock l(mutex_);
    root_.ClearChildren();
    SetChanged();
  }

  /**
   * @return A pointer to the root node.
   */
  PropertyNode *rootNode() { return &this->root_; }

  /**
   * @brief Set data for a node. Path is created if necessary.
   * @tparam P Path type.
   * @param path The path of the node to set.
   * @param data The data to set at path.
   */
  template<typename P>
  void SetData(const P &path, const T &data) {
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

  /**
   * @brief Get a data value reference from the tree by path.
   * @tparam P Path type.
   * @param path The path of the object to get.
   * @param data A reference to collect the data object.
   */
  template<typename P>
  void GetData(const P &path, T &data) {
    ReadLock l(mutex_);
    auto *p = root_.Find(path);
    if (p) {
      data = p->data();
    }
  }

  /**
   * @return A reference to the root node.
   */
  PropertyNode &GetRootNode() { return root_; }

  /**
   * @brief Get the node at a path
   * @tparam P Path type.
   * @param path The path of the object to get.
   * @return The Node at the given path.
   */
  template<typename P>
  PropertyNode *GetNode(const P &path) {
    ReadLock l(mutex_);
    auto *p = root_.Find(path);
    if (p) {
      return p;
    }
    return nullptr;
  }

  /**
   * @brief Find a node by path.
   * @tparam P Path type.
   * @param path The path of the node to get.
   * @return A pointer to the node at the path or nullptr is a node doesn't exist at that path.
   */
  template<typename P>
  PropertyNode *Find(const P &path) {
    ReadLock l(mutex_);
    return root_.Find(path);
  }

  /**
   * @brief Check if a node exists at the path.
   * @tparam P Path type.
   * @param path The path of the check.
   * @return true if a node exists at path.
   */
  template<typename P>
  bool exists(const P &path) {
    return root_.Find(path) != nullptr;
  }

  /**
   * @brief Remove the node at path from the tree.
   * @tparam P Path type.
   * @param path The path of the node to remove.
   */
  template<typename P>
  void remove(const P &path) {
    WriteLock l(mutex_);
    SetChanged();
    root_.Remove(path);
  }

  /**
   * @brief Get the full path to a node
   * @param node The node the get the full path for.
   * @param path A reference to collect the full path of the node.
   */
  void GetFullPath(PropertyNode *node, Path &path) {
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

  /**
   * @brief List the children of a node
   * @tparam P The path type.
   * @param path The path of the node to list.
   * @param children_list receives the list of child node names.
   * @return The length of the list.
   */
  template<typename P>
  unsigned long ListChildren(const P &path, std::vector<K> &children_list) {
    children_list.clear();
    auto i = Find(path);
    if (i) {
      ReadLock lx(mutex_);
      for (auto j = i->children().begin(); j != i->children().end(); j++) {
        children_list.push_back(j->first);
      }
    }
    return children_list.size();
  }

  /**
   * @brief List the children of the root node
   * @param children_list receives the list of child node names.
   * @return The length of the list.
   */
  unsigned long ListChildren(std::vector<K> &children_list) {
    children_list.clear();
    ReadLock lx(mutex_);
    for (auto j = root_.children().begin(); j != root_.children().end(); j++) {
      children_list.push_back(j->first);
    }
    return children_list.size();
  }

};
#endif //OBJECT_PROPERTY_TREE_PROPERTY_TREE_H
