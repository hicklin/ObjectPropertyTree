#ifndef OBJECT_PROPERTY_TREE_NODE_H
#define OBJECT_PROPERTY_TREE_NODE_H

#include <map>
#include <functional>
#include "node_path.h"

template<typename K, typename T>
/*!
    \brief The Node class
    This is a leaf in the property tree
*/

/**
 * @brief This is a class for habdling the leaf nodes of the property tree.
 * @tparam K The type of the node name.
 * @tparam T The type of the node data.
 */
class Node {
 public:
  typedef std::map<K, Node *> ChildMap; ///< map of children.
  typedef NodePath<K> Path; ///< path in a tree.

 private:
  K name_; ///< The name of the node.
  T data_; ///< The leaf data.
  Node *parent_ = nullptr; ///< The node's parent.
  ChildMap children_; ///< The children.

 public:
  /**
   * @brief A non-lambda visitor class.
   */
  class NodeIteratorFunc {
   public:
    virtual void Do(Node *) {}
  };

  /**
   * @brief Create a property tree node.
   */
  Node() = default;

  /**
   * @brief Create a property tree node.
   * @param name The name of the node.
   * @param parent A pointer to the parent node.
   */
  explicit Node(const K &name, Node *parent = nullptr) : name_(name), parent_(parent) {}

  /**
   * @brief Destruct and de-register from parent.
   */
  virtual ~Node();

  /**
   * @brief Delete all children.
   */
  void ClearChildren();

  /**
   * @return The dictionary of children.
   */
  ChildMap &children() { return children_; }

  /**
   * @return reference to the node data.
   */
  T &data() { return data_; }

  /**
   * @brief Set the node data value. The data type must have a copy constructor.
   * @param data The node data to set.
   */
  void setData(const T &data) { data_ = data; }

  /**
   * @brief Get a pointer to a child with a given name.
   * @param child_name The browse name of child to find.
   * @return pointer to the child or nullptr.
   */
  Node *GetChild(const K &child_name) { return children_[child_name]; }

  /**
   * @brief Checks if this node has a child with the name given.
   * @param child_name The browse name of child to find.
   * @return true if child exists.
   */
  bool HasChild(const K &child_name) { return children_[child_name] != nullptr; }

  /**
   * @brief Add a child to the node.
   * @param child_node pointer to child node to add.
   */
  void AddChild(Node *child_node);

  /**
   * @brief Creates a child with default data.
   * @param name The child's name.
   * @param parent_node Pointer to the parent node, this node by default.
   * @return the created node.
   */
  Node *CreateChild(const K &name, Node *parent_node = nullptr);

  /**
   * @brief Remove a child with the name given.
   * @param child_name The name of the child to remove.
   */
  void RemoveChild(const K &child_name) {
    if (HasChild(child_name)) {
      Node *child_node = GetChild(child_name);  // take the child node
      children_.erase(child_name);
      if (child_node) delete child_node;
    }
  }

  // accessors
  /**
   * @return The name of the node
   */
  const K &name() const { return name_; }

  /**
   * @brief Set the name of the node.
   * @param s
   */
  void SetName(const K &s) { name_ = s; }

  /**
   * @return A pointer to the parent.
   */
  Node *parent() const { return parent_; }

  /**
   * @brief Set or replace the parent node.
   * @param parent_node A pointer to the parent node to set.
   */
  void setParent(Node *parent_node);

  /**
   * @brief Get a pointer to the node at the path.
   * @param path The path to search in.
   * @param depth The depth in the path to use.
   * @return A pointer to the node at the path or nullptr is a node doesn't exist at that path.
   */
  Node *Find(const Path &path, int depth = 0);

  /**
   * @brief Get a pointer to the node at the path.
   * @param path The path to search in.
   * @return A pointer to the node at the path or nullptr is a node doesn't exist at that path.
   */
  Node *Find(const K &path) {
    Path p;
    p.ToList(path);
    return Find(p);
  }

  /**
   * @brief Adds a node to the tree starting with this node.
   * @param path The path with respect to this node.
   * @return A pointer to the new node or nullptr.
   */
  Node *Add(const Path &path);

  /**
   * @brief Adds a node to the tree starting with this node.
   * @param path The path with respect to this node.
   * @return A pointer to the new node or nullptr.
   */
  Node *Add(const K &path) {
    Path p;
    p.ToList(path);
    return Add(p);
  }

  /**
   * @brief Removes a node from the tree at path starting with this node.
   * @param path The path with respect to this node.
   */
  void Remove(const Path &path);

  /**
   * @brief Removes a node from the tree at path starting with this node.
   * @param path The path with respect to this node.
   */
  void Remove(const K &s) {
    Path p;
    p.ToList(s);
    Remove(p);
  }

  /**
   * @brief Iterate this node and all children nodes using the given lambda function.
   * @param func The lambda function to iterate.
   * @return true on success.
   */
  bool IterateNodes(std::function<bool(Node &)> func);

  /**
   * @brief Iterate this node and all children nodes using a non-lambda visitor.
   * @param func The function to iterate.
   * @return true on success.
   */
  void IterateNodes(NodeIteratorFunc &func);

  /**
   * @brief Write the node and its children to the stream.
   * @tparam STREAM The stream type.
   * @param output_stream A reference to collect the stream.
   */
  template<typename STREAM>
  void write(STREAM &output_stream);

  /**
   * @brief Read a node and its children from a stream.
   * @tparam STREAM The stream type.
   * @param input_stream A reference to the stream.
   */
  template<typename STREAM>
  void read(STREAM &input_stream);

  /**
   * @brief Copy recursively to a node.
   * @param node The node to copy to.
   */
  void CopyTo(Node *node);
};
#endif //OBJECT_PROPERTY_TREE_NODE_H
