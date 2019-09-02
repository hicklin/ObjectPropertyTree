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

/**
 * @brief A property tree capable of storing any object types and pointers.
 */
class ObjectPropertyTree : public PropertyTree<std::string, boost::any> {

 public:
  /**
   * @brief Create and object property tree.
   */
  ObjectPropertyTree() = default;

  /**
   * @brief Set an object pointer at a path.
   * @tparam T The type of the pointer to set.
   * @param path The path where the pointer should be set.
   * @param object_pointer The pointer to set.
   */
  template<typename T>
  void SetPointer(const ObjectPath &path, T *object_pointer) {
    std::string object_path;
    path.ToString(object_path);
    SetPointer(object_path, object_pointer);
  }

  /**
   * @brief Set an object pointer at a path.
   * @tparam T The type of the pointer to set.
   * @param path The path where the pointer should be set.
   * @param object_pointer The pointer to set.
   */
  template<typename T>
  void SetPointer(const std::string &path, T *object_pointer) {
    auto shared_pointer = std::shared_ptr<T>(object_pointer);
    SetData(path, shared_pointer);
  }

  /**
   * @brief Set and object at a path
   * @tparam T The type of the object.
   * @param path The path to store the object.
   * @param object The object.
   */
  template<typename T>
  void SetObject(std::string path, const T &object) {
    SetData(path, object);
  }

  /**
   * @brief Set and object at a path
   * @tparam T The type of the object.
   * @param path The path to store the object.
   * @param object The object.
   */
  template<typename T>
  void SetObject(ObjectPath path, const T &object) {
    SetData(path, object);
  }

  /**
   * @brief Get the pointer of the object stored in the object node.
   * @tparam T The type of the stored object.
   * @param object_node The object node containing the object in question.
   * @return A pointer to the object stored in the node.
   */
  template<typename T>
  T *GetPointer(ObjectNode *object_node) {
    if(object_node) {
      try {
        boost::any &object_data = object_node->data();
        if (!object_data.empty() && object_data.type().hash_code() == typeid(std::shared_ptr<T>).hash_code()) {
          std::shared_ptr<T> *object_pointer = boost::any_cast<std::shared_ptr<T> >(&object_data);
          return object_pointer->get();
        }
      }
      catch (...) {

      }
    }
    return nullptr;
  }

  /**
   * @brief Get the pointer of the object stored in the object tree at the path.
   * @tparam T The type of the stored object.
   * @param path The path to the object node containing the object in question.
   * @return A pointer to the object stored in the node.
   */
  template<typename T>
  T *GetPointer(const std::string &path) {
    ObjectNode *node = Find(path);
    if(node) {
      return GetPointer<T>(node);
    }
    return nullptr;
  }

  /**
   * @brief Get the pointer of the object stored in the object tree at the path.
   * @tparam T The type of the stored object.
   * @param path The path to the object node containing the object in question.
   * @return A pointer to the object stored in the node.
   */
  template<typename T>
  T *GetPointer(const ObjectPath &path) {
    std::string object_path;
    path.ToString(object_path);
    return GetPointer<T>(object_path);
  }

  /**
   * @brief Get the object at path.
   * @tparam T The type of the object to get.
   * @param path The path of the object in the tree.
   * @return The object.
   */
  template<typename T>
  T GetObject(const std::string &path) {
    ObjectNode *node = Find(path);
    if (node) {
      boost::any &object = node->data();
      if (!object.empty() && object.type().hash_code() == typeid(T).hash_code()) {
        return boost::any_cast<T>(object);
      }
    }
  }

  /**
   * @brief Get the object at path.
   * @tparam T The type of the object to get.
   * @param path The path of the object in the tree.
   * @return The object.
   */
  template<typename T>
  T GetObject(ObjectPath &path) {
    std::string path_string;
    path.ToString(path_string);
    return GetObject<T>(path_string);
  }

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
