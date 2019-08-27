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
  T default_data_; ///< The default data to be returned.
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
  void clear();

  /**
   * @brief Get a data value reference from the tree by path.
   * @tparam P Path type.
   * @param path The path of the object to get.
   * @return A reference to the object or default value if not found.
   */
  template<typename P>
  T &GetData(const P &path);

  /**
   * @return A reference to the root node.
   */
  PropertyNode &root() { return root_; }

  /**
   * @return A pointer to the root node.
   */
  PropertyNode *rootNode() { return &this->root_; }

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
   * @brief Set data for a node. Path is created if necessary.
   * @tparam P Path type.
   * @param path The path of the node to set.
   * @param data The data to set at path.
   */
  template<typename P>
  void SetData(const P &path, const T &data);

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
  void GetFullPath(PropertyNode *node, Path &path);

  /**
   * @brief Get the data of a child of the node.
   * @param node The node to in.
   * @param child_name The name of the child.
   * @param default_data The default data to return.
   * @return A reference to the data or default
   */
  T &GetChildData(PropertyNode *node, const K &child_name, T &default_data);

  /**
   * @brief Set the data of a node child.
   * @param node The node to in.
   * @param child_name The name of the child.
   * @param child_data The data to set for the child.
   */
  void SetChildData(PropertyNode *node, const K &child_name, const T &child_data);

  /**
   * @brief Iterates all nodes with a function.
   * @param func The lambda function to use.
   * @return true if iteration is to continue
   */
  bool IterateNodes(std::function<bool(PropertyNode &)> func) {
    ReadLock l(mutex_);
    return root_.IterateNodes(func);
  }

  /**
   * @brief Write on a stream
   * @tparam S The stream type.
   * @param output_stream A reference to collect the stream.
   */
  template<typename S>
  void write(S &output_stream) {
    ReadLock l(mutex_);
    root_.write(output_stream);
  }

  /**
   * @brief Read from a stream.
   * @tparam S The stream type.
   * @param input_stream A reference to the stream.
   */
  template<typename S>
  void read(S &input_stream) {
    WriteLock l(mutex_);
    root_.read(input_stream);
    SetChanged();
  }

  /**
   * @brief Copy to a tree.
   * @param destination The destination tree.
   */
  void CopyTo(PropertyTree &destination) {
    ReadLock l(mutex_);
    root_.CopyTo(&destination.root_);
    destination.SetChanged();
  }

  /**
   * @brief List the children of a node
   * @tparam P The path type.
   * @param path The path of the node to list.
   * @param children_list receives the list of child node names.
   * @return The length of the list.
   */
  template<typename P>
  unsigned long ListChildren(const P &path, std::vector<K> &children_list);

};
#endif //OBJECT_PROPERTY_TREE_PROPERTY_TREE_H
