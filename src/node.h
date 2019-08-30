#ifndef OBJECT_PROPERTY_TREE_NODE_H
#define OBJECT_PROPERTY_TREE_NODE_H

#include <map>
#include <functional>
#include "node_path.h"

/**
 * @brief This is a class for handling the leaf nodes of the property tree.
 * @tparam K The type of the node name.
 * @tparam T The type of the node data.
 */
template<typename K, typename T>
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
    /**
     * @brief A function to iterate through the nodes.
     */
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
  virtual ~Node() {
    if (parent_) {
      parent_->children_.erase(name()); // detach
      parent_ = nullptr;
    }
    ClearChildren();
  };

  /**
   * @brief Delete all children.
   */
  void ClearChildren() {
    for (auto i = children_.begin(); i != children_.end(); i++) {
      Node *node = i->second;
      if (node) {
        node->parent_ = nullptr;
      }
    }
    children_.clear();
  }

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
  void SetData(const T &data) { data_ = data; }

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
  bool HasChild(const K &child_name) {
    if ( children_.find(child_name) == children_.end() ) {
      return false;
    } else {
      return children_[child_name] != nullptr;
    }
  }

  /**
   * @brief Add a child to the node.
   * @param child_node pointer to child node to add.
   */
  void AddChild(Node *child_node) {
    if (HasChild(child_node->name())) {
      delete children_[child_node->name()];
      children_.erase(child_node->name());
    }
    children_[child_node->name()] = child_node;
  };

  /**
   * @brief Creates a child with default data.
   * @param name The child's name.
   * @param parent_node Pointer to the parent node, this node by default.
   * @return Pointer to the created node.
   */
  Node *CreateChild(const K &name, Node *parent_node = nullptr) {
    if (!parent_node) parent_node = this;
    Node *node = new Node(name, parent_node);
    AddChild(node);
    return node;
  }

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
   * @param node_name The name to give the node.
   */
  void SetName(const K &node_name) {
    if (parent_) {
      parent_->children_.erase(name());
      parent_->children_[node_name] = this;
    }
    name_ = node_name;
  }

  /**
   * @return A pointer to the parent.
   */
  Node *parent() const { return parent_; }

  /**
   * @brief Set or replace the parent node.
   * @param parent_node A pointer to the parent node to set.
   */
  void SetParent(Node *parent_node) {
    if (parent_ && (parent_ != parent_node)) {
      parent_->children_.erase(name());
    }

    parent_ = parent_node;
    if (parent_) {
      parent_->children_[name()] = this;
    }
  };

  /**
   * @brief Get a pointer to the node at the path.
   * @param path The path to search in.
   * @param depth The depth in the path to use.
   * @return A pointer to the node at the path or nullptr is a node doesn't exist at that path.
   */
  Node *Find(const Path &path, int depth = 0) {
    Node *res = GetChild(path[depth]);// do we have the child at this level?
    if (res) {
      depth++;
      if (depth < (int) path.size()) {
        res = res->Find(path, depth);
      }
    }
    return res;
  }

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
  Node *Add(const Path &path) {
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
  void Remove(const Path &path) {
    Node *node = Find(path);
    if (node) {
      delete node;
    }
  }

  /**
   * @brief Removes a node from the tree at path starting with this node.
   * @param path The path with respect to this node.
   */
  void Remove(const K &path) {
    Path node_path;
    node_path.ToList(path);
    Remove(node_path);
  }

  /**
   * @brief Iterate this node and all children nodes using the given lambda function.
   * @param func The lambda function to iterate.
   * @return true on success.
   */
  bool IterateNodes(std::function<bool(Node &)> func) {
    if (func(*this)) {
      for (auto i = children().begin(); i != children().end(); i++) {
        (i->second)->iterateNodes(func);
      }
      return true;
    }
    return false;
  }


  /**
   * @brief Iterate this node and all children nodes using a non-lambda visitor.
   * @param func The function to iterate.
   * @return true on success.
   */
  void IterateNodes(NodeIteratorFunc &func) {
    func.Do(this); // action the function for the node
    for (auto i = children().begin(); i != children().end(); i++) {
      (i->second)->iterateNodes(func);
    }
  }

};
#endif //OBJECT_PROPERTY_TREE_NODE_H
