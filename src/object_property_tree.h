#ifndef OBJECT_PROPERTY_TREE_OBJECT_PROPERTY_TREE_H
#define OBJECT_PROPERTY_TREE_OBJECT_PROPERTY_TREE_H

#include "property_tree.h"
#include <boost/any.hpp>
#include <iostream>

/**
 * @brief Parsed path to object.
 */
typedef NodePath<std::string> ObjectPath;

/**
 * @brief Stores generic objects as boost any.
 */
typedef PropertyTree<std::string, boost::any>::PropertyNode ObjectNode;

class ObjectPropertyTree : public PropertyTree<std::string, boost::any> {

  /**
   * @brief Create and object property tree.
   */
  ObjectPropertyTree() = default;

  /**
   * @brief Set the object pointer at the path.
   * @tparam T The type of the pointer to set.
   * @param path The path where the pointer should be set.
   * @param object_pointer The pointer to set.
   */
  template <typename T>
  void SetObjectPointer(ObjectPath path, T *object_pointer) {
    auto shared_pointer = std::shared_ptr<T>(object_pointer);
    SetData(path, shared_pointer);
  }

  /**
   * @brief Get the pointer of the object stored in the object node.
   * @tparam T The type of the stored object.
   * @param object_node The object node containing the object in question.
   * @return A pointer to the object stored in the node.
   */
  template <typename T>
  T *GetObjectPointer(ObjectNode *object_node);

  /**
   * @brief Get the pointer of the object stored in the object tree at the path.
   * @tparam T The type of the stored object.
   * @param path The path to the object node containing the object in question.
   * @return A pointer to the object stored in the node.
   */
  template <typename T>
  T *GetObjectPointer(const ObjectPath &path);

  /**
   * @brief Recursively print out a node to and std stream.
   * @param output_stream The stream to print to.
   * @param object_node The object node to print from.
   * @param level The starting indentation level.
   */
  void PrintNode(std::ostream &output_stream, ObjectNode *object_node, unsigned long level = 0);

  /**
   * @brief Print the object tree into an std stream.
   * @param output_stream The stream to print to. Default is std::cout.
   */
  void PrintTree(std::ostream &output_stream = std::cout) {
    PrintNode(output_stream, rootNode());
  }



};

#endif //OBJECT_PROPERTY_TREE_OBJECT_PROPERTY_TREE_H
