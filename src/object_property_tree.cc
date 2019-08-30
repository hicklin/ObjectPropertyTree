#include "object_property_tree.h"

void ObjectPropertyTree::PrintNode(std::ostream &output_stream, ObjectNode *object_node, unsigned long level) {
  if (object_node) {
    std::string indent(level*2, ' ');

    output_stream << indent << object_node->name() << ":" << object_node->data().type().name();
    if (object_node->data().empty()) {
      output_stream << ": EMPTY";
    } else {
      output_stream << " : Object";
    }
    output_stream << std::endl;

    if (!object_node->children().empty()) {
      level++;
      for (auto &child_node : object_node->children()) {
        PrintNode(output_stream, child_node.second, level); // recurse
      }
    }
  }
}
