#include "object_property_tree.h"

template<typename T>
T *ObjectPropertyTree::GetObjectPointer(ObjectNode *object_node) {
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

template<typename T>
T *ObjectPropertyTree::GetObjectPointer(const ObjectPath &path) {
  ObjectNode *node = Find(path);
  if(node) {
    return GetObjectPointer<T>(node);
  }
  return nullptr;
}

void ObjectPropertyTree::PrintNode(std::ostream &output_stream, ObjectNode *object_node, unsigned long level) {
  if (object_node) {
    std::string indent(level, ' ');

    output_stream << indent << object_node->name() << ":" << object_node->data().type().name();
    if (object_node->data().empty()) {
      output_stream << ": EMPTY";
    } else {
      output_stream << " : Object";
    }
    output_stream << std::endl;

    if (object_node->children().empty()) {
      level++;
      for (auto &child_node : object_node->children()) {
        PrintNode(output_stream, child_node.second, level); // recurse
      }
    }
  }
}
